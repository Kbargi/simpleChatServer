#pragma once

#include "User.h"
#include "RoomManager.h"

#include "chat.pb.h"

class Room {
   typedef std::shared_ptr<User> UserP;
   typedef std::unordered_map<int, UserP> UsersMap;

   public:
      Room() = delete;
      Room(const std::string& roomId, const std::string& pass) : m_roomId(roomId), m_roomPassword(pass){}

      ~Room(){}

      const std::string& getPassword() {return m_roomPassword;}
      const std::string& getRoomId() {return m_roomId;}

      void addUser(UserP user) {/*zabezpieczyc przed nadpisaniem istniejacego usera*/m_users[user->getSocket()] = user;}
      void removeUser(int s, std::string& userPassword, bool force = false) {
         UsersMap::iterator it = m_users.find(s);
         if((it != m_users.end() && force) || (it!= m_users.end() && it->second->getPassword() == userPassword)) {
            m_users.erase(it);
         }
      }

      size_t getUserCounter() { return m_users.size();}

      void notifyAllUsers(int clientSocket, std::string& userPassword, std::string& content);
      void notifyOnEvent(int socket, const std::string& notification);

   private:
      UsersMap m_users; //zrobic z tego pointer
      const std::string m_roomId;
      const std::string m_roomPassword;
};
