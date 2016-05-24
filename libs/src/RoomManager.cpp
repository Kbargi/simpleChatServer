#include "RoomManager.h"

void RoomManager::processEscape(int socket) {
    std::shared_ptr<User> userP;
    {
        std::map<int, std::shared_ptr<User>>::iterator userIt;
        UsersBulk& bulk = getBulk(socket);
        WriteLock l(bulk.m_usersMutex);
        userIt = bulk.m_users.find(socket);
        if(userIt != bulk.m_users.end()) {
        	userP = userIt->second;
        	bulk.m_users.erase(userIt);
        }
        if(userP) {
        	std::shared_ptr<Room> m_room = userP->getRoom();
        	ReadLock l(m_room->m_mutex);
        	if(m_room.use_count() <= 1) {
        		bulk.m_rooms.erase(m_room->getName());
        	} else {
                /*TODO: notyfikuj o opuszczeniu pokoju*/
            }
        }
    }
    close(socket);
}

void RoomManager::processRequest(int clientSocket, chat::Request& req) {
   std::string roomPassword, roomName, userPassword, content, userName;
   roomName = req.roomname();
   roomPassword = req.roompassword();
   userPassword = req.userpassword();
   userName = req.username();
   content = req.content();

   if(roomName.empty() || userName.empty()) {
      RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(req.operation()), chat::Response::ERROR, std::string("Unknown operation"), userName);
      return;
   }
   try {
      switch(req.operation()) {
         case chat::Request::DELIVER: //most common - keep this first
            processDelivery(clientSocket, roomName, roomPassword, userName, userPassword, content);
            break;
         case chat::Request::CREATE_ROOM:
            processRoomCreation(clientSocket, roomName, roomPassword, userName, userPassword);
            break;
         case chat::Request::JOIN_2_ROOM:
            processJoin(clientSocket, roomName, roomPassword, userName, userPassword);
            break;
         default:
            RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(req.operation()), chat::Response::ERROR, std::string("Unknown operation"), userName);
      }
   } catch(...) {
      std::cout << "Unknown expcetion while processing processRequest\n";
      RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(req.operation()), chat::Response::ERROR, std::string("Unknown exception while processing request"), userName);
   }
}

void RoomManager::processDelivery(int clientSocket, const std::string& roomName, const std::string& roomPassword, const std::string& userName, const std::string& userPassword, const std::string& content) {
    UsersBulk& bulk = getBulk(clientSocket);
    std::shared_ptr<User> userP;
    {
        ReadLock l(bulk.m_usersMutex);
        std::map<int, std::shared_ptr<User>>::iterator it = bulk.m_users.find(clientSocket);
        if(it != bulk.m_users.end()) {
            userP = it->second;
        }
    }
    if(!userP) {
        RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::DELIVER), chat::Response::ERROR, std::string("User not connected to room"), userName);
        return;
    } else {
        if(userP->getPassword() != userPassword) {
            RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::DELIVER), chat::Response::ERROR, std::string("Wrong password"), userName);
            return;
        }
        std::shared_ptr<Room> roomP = userP->getRoom();
        if(roomP) {
            if(roomP->getPassword() != roomPassword) {
                RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::DELIVER), chat::Response::ERROR, std::string("Wrong password"), userName);
                return;
            }
            {
                ReadLock l (roomP->m_mutex);
                std::vector<std::weak_ptr<User>> usersSet = roomP->getUsers();
                std::shared_ptr<User> user;
                for(std::vector<std::weak_ptr<User>>::iterator it = usersSet.begin() ; it != usersSet.end() ;) {
                    user = it->lock();
                    if(user) {
                        RoomManager::sendResponse(user->getSocket(), roomName, requestName2ResponseName(chat::Request::DELIVER), chat::Response::OK, content, userName);
                        ++it;
                    } else {
                        it = usersSet.erase(it);
                    }
                }
            }
            RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::DELIVER), chat::Response::OK, std::string(), userName);
        }
    }
}

void RoomManager::processRoomCreation(int clientSocket, const std::string& roomName, const std::string& roomPassword, const std::string& userName, const std::string& userPassword) {
    UsersBulk& bulk = getBulk(clientSocket);
    {
        ReadLock l(bulk.m_usersMutex);
        std::map<int, std::shared_ptr<User>>::iterator it = bulk.m_users.find(clientSocket);
        if(it != bulk.m_users.end()) {
            RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::CREATE_ROOM), chat::Response::ERROR, std::string("User exists"), userName);
            return;
        }
        std::map<std::string, std::weak_ptr<Room>>::iterator itr = bulk.m_rooms.find(roomName);
        if(itr != bulk.m_rooms.end()) {
            RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::CREATE_ROOM), chat::Response::ERROR, std::string("Room already exists"), userName);
            return;
        }
    }
    std::shared_ptr<User> user;
    user = std::make_shared<User>(userName, userPassword, clientSocket);
    std::shared_ptr<Room> room;
    room = std::make_shared<Room>(roomName, roomPassword);
    room->addUser(user);
    user->setRoom(room);
    {
        WriteLock l(bulk.m_usersMutex);
        bulk.m_users[clientSocket] = user;
        bulk.m_rooms[roomName] = room;
    }
    RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::CREATE_ROOM), chat::Response::OK, std::string("Room has been created"), userName);
}

void RoomManager::processJoin(int clientSocket, const std::string& roomName, const std::string& roomPassword, const std::string& userName, const std::string& userPassword) {
    UsersBulk& bulk = getBulk(clientSocket);
    {
        ReadLock l(bulk.m_usersMutex);
        std::map<int, std::shared_ptr<User>>::iterator it = bulk.m_users.find(clientSocket);
        if(it != bulk.m_users.end()) {
            RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::CREATE_ROOM), chat::Response::ERROR, std::string("User exists"), userName);
            return;
        }
    }

    std::shared_ptr<Room> room;
    {
        std::map<std::string, std::weak_ptr<Room>>::iterator it;
        ReadLock l(bulk.m_usersMutex);
        it = bulk.m_rooms.find(roomName);
        if(it == bulk.m_rooms.end()) {
            RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::CREATE_ROOM), chat::Response::ERROR, std::string("Room does not exist"), userName);
            return;
        }
        room = it->second.lock();
        if(!room) {
            RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::CREATE_ROOM), chat::Response::ERROR, std::string("Room has been destroyed"), userName);
            return;
        }
        if(room->getPassword() != roomPassword) {
            RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::CREATE_ROOM), chat::Response::ERROR, std::string("Wrong room password"), userName);
            return;
        }
    }
    std::shared_ptr<User> user = std::make_shared<User>(userName, userPassword, clientSocket);
    user->setRoom(room);
    room->addUser(user);
    /*TODO: dodac notyfikacje o dolaczeniu do innych userow z roomu*/
    RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::CREATE_ROOM), chat::Response::OK, std::string("You have joined to room"), userName);
}

void RoomManager::sendResponse(const int socket, const std::string& roomName, chat::Response_Type respType, chat::Response_ResultCode rCode, const std::string& desc, const std::string& userName) {
    chat::Response rsp;

    rsp.set_operation(respType);
    rsp.set_roomname(roomName);
    rsp.set_resultcode(rCode);
    rsp.set_resultdescription(desc);
    rsp.set_sendername(userName);

    std::string response;
    int total = 0;
    int n;

    rsp.SerializeToString(&response);
    const char* buf = response.c_str();
    int len = response.size();
    int bytesleft = len;
    while(total < len) {
        n = send(socket, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
        std::cout << total << "/" <<len<<"\n";
    }
    std::cout << "Sent " << total << "/" << len << " bytes to " << socket << "\n";
}
