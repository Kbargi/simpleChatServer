#include "RoomManager.h"

void RoomManager::processRequest(int clientSocket, chat::Request& req) {
   std::string roomPassword, roomName, userPassword, content, userName;
   roomName = req.roomname();
   roomPassword = req.roompassword();
   userPassword = req.userpassword();
   userName = req.username();
   content = req.content();

   if(roomName.empty()) {
      RoomManager::sendResponse(clientSocket, roomName, chat::Response::UNKNOWN, chat::Response::NOT_SUPPORTED, std::string("Unknown operation"), std::string(), std::string());
      return;
   }
   try {
      switch(req.operation()) {
         case chat::Request::DELIVER: //most common - keep this first
            processDelivery(clientSocket, roomName, roomPassword, userPassword, content);
            break;
         case chat::Request::CREATE_ROOM:
            processRoomCreation(clientSocket, roomName, roomPassword, userPassword, userName);
            break;
         case chat::Request::JOIN_2_ROOM:
            processJoin(clientSocket, roomName, roomPassword, userPassword, userName);
            break;
         case chat::Request::LEAVE_ROOM:
            processLeave(clientSocket, roomName, roomPassword, userPassword);
            break;
         default:
            RoomManager::sendResponse(clientSocket, roomName, chat::Response::UNKNOWN, chat::Response::NOT_SUPPORTED, std::string("Unknown operation"), std::string(), std::string());
      }
   } catch(...) {
      std::cout << "Unknown expcetion while processing processRequest\n";
      RoomManager::sendResponse(clientSocket, roomName, chat::Response::UNKNOWN, chat::Response::NOT_SUPPORTED, std::string("Unknown error"), std::string(), std::string());
   }
}
void RoomManager::processDelivery(int clientSocket, std::string& roomName, std::string& roomPassword, std::string& userPassword, std::string& content) {
   struct RoomGroup& group = getGroup(roomName);
   {
      ReadLock lock(group.mutex);
      std::unordered_map<std::string, std::shared_ptr<Room>>::iterator it = group.rooms.find(roomName);
      if( it != group.rooms.end() ) {
         if(it->second->getRoomId() == roomName && it->second->getPassword() == roomPassword) {
             it->second->notifyAllUsers(clientSocket, userPassword, content);
         } else {
            RoomManager::sendResponse(clientSocket, roomName, chat::Response::DELIVERY_CONFIRMATION, chat::Response::LIMITED_ACCESS, std::string("Wrong login or password"), std::string(), std::string());
            return;
         }
      } else {
         RoomManager::sendResponse(clientSocket, roomName, chat::Response::DELIVERY_CONFIRMATION, chat::Response::NOT_FOUND, std::string("Room does not exists"), std::string(), std::string());
         return;
      }
   }
   RoomManager::sendResponse(clientSocket, roomName, chat::Response::DELIVERY_CONFIRMATION, chat::Response::OK, std::string("OK"), std::string(), std::string());
}
void RoomManager::processRoomCreation(int clientSocket, std::string& roomName, std::string& roomPassword, std::string& userPassword, std::string& userName) {
   struct RoomGroup& group = getGroup(roomName);

   std::shared_ptr<Room> room (std::make_shared<Room>(roomName, roomPassword));
   std::shared_ptr<User> user (std::make_shared<User>(clientSocket, userName));

   user->setRoomId(roomName);
   user->setPassword(userPassword);
   room->addUser(user);

   {
      WriteLock lock(group.mutex);
      if( group.rooms.find(roomName) == group.rooms.end() ) {
         group.rooms[roomName] = room;
      } else {
         RoomManager::sendResponse(clientSocket, roomName, chat::Response::CREATE_ROOM, chat::Response::DUPLICATION, std::string("Room already exists"), std::string(), std::string());
         return;
      }
   }
   m_sock2RoomName.add(clientSocket, roomName);
   RoomManager::sendResponse(clientSocket, roomName, chat::Response::CREATE_ROOM, chat::Response::OK, std::string("OK"), std::string(), std::string());
}
void RoomManager::processJoin(int clientSocket, std::string& roomName, std::string& roomPassword, std::string& userPassword, std::string& userName) {
   struct RoomGroup& group = getGroup(roomName);
   std::shared_ptr<User> user (std::make_shared<User>(clientSocket, userName));
   user->setRoomId(roomName);
   user->setPassword(userPassword);
   {
      WriteLock lock(group.mutex);
      std::unordered_map<std::string, std::shared_ptr<Room>>::iterator it = group.rooms.find(roomName);
      if( it != group.rooms.end() ) {
         if(it->second->getPassword() == roomPassword && it->second->getRoomId() == roomName) {
            it->second->addUser(user);
            it->second->notifyOnEvent(clientSocket, std::string("joined room"));
         } else {
            RoomManager::sendResponse(clientSocket, roomName, chat::Response::JOIN_2_ROOM, chat::Response::LIMITED_ACCESS, std::string("Wrong login or password"), std::string(), std::string());
            return;
         }
      } else {
         RoomManager::sendResponse(clientSocket, roomName, chat::Response::JOIN_2_ROOM, chat::Response::NOT_FOUND, std::string("Room does not exists"), std::string(), std::string());
         return;
      }
   }
   m_sock2RoomName.add(clientSocket, roomName);
   RoomManager::sendResponse(clientSocket, roomName, chat::Response::JOIN_2_ROOM, chat::Response::OK, std::string("OK"), std::string(), std::string());
}
void RoomManager::processLeave(int clientSocket, std::string& roomName, std::string& roomPassword, std::string& userPassword, bool force) {
   struct RoomGroup& group = getGroup(roomName);
   m_sock2RoomName.remove(clientSocket);
   {
      WriteLock lock(group.mutex);
      std::unordered_map<std::string, std::shared_ptr<Room>>::iterator it = group.rooms.find(roomName);
      if( it != group.rooms.end()) {
         if(force || (it->second->getPassword() == roomPassword && it->second->getRoomId() == roomName)) {
            if(! (it->second->getUserCounter() - 1) ) {
               group.rooms.erase(roomName);
               return;
            } else {
               it->second->notifyOnEvent(clientSocket, std::string("left room"));
            }
            it->second->removeUser(clientSocket, userPassword, force);
         } else {
            RoomManager::sendResponse(clientSocket, roomName, chat::Response::LEAVE_ROOM, chat::Response::LIMITED_ACCESS, std::string("Wrong login or password"), std::string(), std::string());
            return;
         }
      } else {
         RoomManager::sendResponse(clientSocket, roomName, chat::Response::LEAVE_ROOM, chat::Response::NOT_FOUND, std::string("Room does not exists"), std::string(), std::string());
         return;
      }
   }
}

void RoomManager::notifyUserEscape(int socket) {
   std::string roomName = m_sock2RoomName.get(socket);

   if(!roomName.empty()) {
      processLeave(socket, roomName, roomName, roomName, true);
   }
}

void RoomManager::sendResponse(int s, const std::string& roomName, chat::Response_Type type, chat::Response_ResultCode res, const std::string& description, const std::string& content, const std::string& sender) {
   std::string response;
   chat::Response rsp;

   rsp.set_operation(type);
   rsp.set_roomname(roomName);
   rsp.set_resultcode(res);
   rsp.set_resultdescription(description);
   rsp.set_content(content);
   rsp.set_username(sender);

   rsp.SerializeToString(&response);

   int bytes = send(s, (const char*)response.c_str(), response.size(), 0);
   std::cout << "Sent " << bytes << " bytes\n";
}
