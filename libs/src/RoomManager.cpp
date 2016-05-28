#include "RoomManager.h"

void RoomManager::processEscape(int socket) {
    std::shared_ptr<User> userP;
    {
        WriteLock l(m_internalData.m_mutex);
        std::map<int, std::shared_ptr<User>>::iterator userIt = m_internalData.m_users.find(socket);
        if(userIt != m_internalData.m_users.end()) {
        	userP = userIt->second;
        	m_internalData.m_users.erase(userIt);
        }
        if(userP) {
        	std::shared_ptr<Room> room = userP->getRoom();
        	std::cout << "escaping ptr cnt: " << room.use_count() << "\n";
        	userP.reset();
        	ReadLock l(room->m_mutex);
        	if(room.use_count() <= 1) {
        		std::cout << "erasing room\n";
        		m_internalData.m_rooms.erase(room->getName());
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

void RoomManager::processDelivery(int clientSocket, const std::string& roomName, const std::string& roomPassword, const std::string& userName, const std::string& userPassword, const std::string& content)
{
    std::shared_ptr<User> userP;
    {
        ReadLock l(m_internalData.m_mutex);
        std::map<int, std::shared_ptr<User>>::iterator it = m_internalData.m_users.find(clientSocket);
        if(it != m_internalData.m_users.end()) {
            userP = it->second;
        }
    }
    if(!userP) {
        RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::DELIVER), chat::Response::ERROR, std::string("User not connected to room"), userName);
        return;
    } else {
        if(userP->getPassword() != userPassword || userP->getName() != userName) {
            RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::DELIVER), chat::Response::ERROR, std::string("Wrong password"), userName);
            return;
        }
        std::shared_ptr<Room> roomP = userP->getRoom();
        if(roomP) {
            if(roomP->getPassword() != roomPassword || roomP->getName() != roomName) {
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
                        if(user->getSocket() == clientSocket) { ++it; continue; }
                        RoomManager::sendResponse(user->getSocket(), roomName, requestName2ResponseName(chat::Request::DELIVER), chat::Response::OK, content, userName);
                        ++it;
                    } else {
                        it = usersSet.erase(it);
                        std::cout << "Old user erased\n";
                    }
                }
            }
            RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::DELIVER), chat::Response::OK, std::string(), userName);
        }
    }
}

void RoomManager::processRoomCreation(int clientSocket, const std::string& roomName, const std::string& roomPassword, const std::string& userName, const std::string& userPassword)
{
    {
        ReadLock l(m_internalData.m_mutex);
        std::map<int, std::shared_ptr<User>>::iterator it = m_internalData.m_users.find(clientSocket);
        if(it != m_internalData.m_users.end()) {
            RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::CREATE_ROOM), chat::Response::ERROR, std::string("User exists"), userName);
            return;
        }
        std::map<std::string, std::weak_ptr<Room>>::iterator itr = m_internalData.m_rooms.find(roomName);
        if(itr != m_internalData.m_rooms.end()) {
            RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::CREATE_ROOM), chat::Response::ERROR, std::string("Room already exists"), userName);
            return;
        }
    }
    std::shared_ptr<User> user (std::make_shared<User>(userName, userPassword, clientSocket));
    std::shared_ptr<Room> room (std::make_shared<Room>(roomName, roomPassword));
    room->addUser(user);
    user->setRoom(room);
    {
        WriteLock l(m_internalData.m_mutex);
        m_internalData.m_users[clientSocket] = user;
        m_internalData.m_rooms[roomName] = room;
    }
    RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::CREATE_ROOM), chat::Response::OK, std::string("Room has been created"), userName);
}

void RoomManager::processJoin(int clientSocket, const std::string& roomName, const std::string& roomPassword, const std::string& userName, const std::string& userPassword) {
    {
        ReadLock l(m_internalData.m_mutex);
        std::map<int, std::shared_ptr<User>>::iterator it = m_internalData.m_users.find(clientSocket);
        if(it != m_internalData.m_users.end()) {
            RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::JOIN_2_ROOM), chat::Response::ERROR, std::string("User exists"), userName);
            return;
        }
    }

    std::shared_ptr<Room> room;
    {
        ReadLock l(m_internalData.m_mutex);
        std::map<std::string, std::weak_ptr<Room>>::iterator it = m_internalData.m_rooms.find(roomName);
        if(it == m_internalData.m_rooms.end()) {
            RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::JOIN_2_ROOM), chat::Response::ERROR, std::string("Room does not exist"), userName);
            return;
        }
        room = it->second.lock();
        if(!room) {
            RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::JOIN_2_ROOM), chat::Response::ERROR, std::string("Room has been destroyed"), userName);
            return;
        }
        if(room->getPassword() != roomPassword || room->getName() != roomName) {
            RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::JOIN_2_ROOM), chat::Response::ERROR, std::string("Wrong room password"), userName);
            return;
        }
    }
    std::shared_ptr<User> user(std::make_shared<User>(userName, userPassword, clientSocket));
    user->setRoom(room);
    room->addUser(user);
    {
        WriteLock l(m_internalData.m_mutex);
        m_internalData.m_users[clientSocket] = user;
    }
    /*TODO: dodac notyfikacje o dolaczeniu do innych userow z roomu*/
    RoomManager::sendResponse(clientSocket, roomName, requestName2ResponseName(chat::Request::JOIN_2_ROOM), chat::Response::OK, std::string("You have joined to room"), userName);
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
    }
}
