#pragma once

#include "User.h"
#include "RoomManager.h"

#include "chat.pb.h"

class Room {
   typedef std::shared_ptr<User> UserP;
   typedef std::unordered_map<int, UserP> UsersMap;

   public:
      Room() = delete;
      Room(const std::string& roomId, const std::string& pass) : m_roomId(roomId), m_roomPassword(pass), m_userCounter(0) {}

      ~Room(){}

      const std::string& getPassword() {return m_roomPassword;}
      const std::string& getRoomId() {return m_roomId;}

      void addUser(UserP user) {/*zabezpieczyc przed nadpisaniem istniejacego usera*/m_users[user->getSocket()] = user; ++m_userCounter;}
      void removeUser(int s, std::string& userPassword) {
         UsersMap::iterator it = m_users.find(s);
         if(it != m_users.end() && it->second->getPassword() == userPassword) {
            --m_userCounter;
            m_users.erase(it);
         }
      }

      size_t getUserCounter() { return m_userCounter;}

      void notifyAllUsers(int clientSocket, std::string& userPassword, std::string& content);
      void notifyEscape(std::string& escapedName, std::string& userPassword);

   private:
      UsersMap m_users; //zrobic z tego pointer
      const std::string m_roomId;
      const std::string m_roomPassword;

      size_t m_userCounter;
};
