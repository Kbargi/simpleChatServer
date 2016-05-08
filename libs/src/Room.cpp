#include "Room.h"

void Room::notifyAllUsers(int clientSocket, std::string& userPassword, std::string& content) {
   UsersMap::const_iterator clientIt = m_users.find(clientSocket);
   UsersMap::const_iterator it = m_users.begin();

   int counter = 0;

   if(clientIt != m_users.end()) {
      if(clientIt->second->getPassword() != userPassword) {
         RoomManager::sendResponse(clientIt->second->getSocket(), m_roomId, chat::Response::DELIVERY_CONFIRMATION, chat::Response::LIMITED_ACCESS, std::string("Wrong user password"), content, std::string());
      }
      for( ; it != m_users.end() ; ++it) {
         if(it == clientIt) continue;

         counter++;
         RoomManager::sendResponse(it->second->getSocket(), m_roomId, chat::Response::NOTIFICATION, chat::Response::OK, std::string("OK"), content, clientIt->second->getName());
      }
      RoomManager::sendResponse(clientIt->second->getSocket(), m_roomId, chat::Response::DELIVERY_CONFIRMATION, chat::Response::OK, std::string("Sent to ") + boost::lexical_cast<std::string>(counter) + std::string(" users"), std::string(), std::string());
   } else {
      RoomManager::sendResponse(clientIt->second->getSocket(), m_roomId, chat::Response::DELIVERY_CONFIRMATION, chat::Response::NOT_FOUND, std::string("User not found"), std::string(), std::string());
   }
}
