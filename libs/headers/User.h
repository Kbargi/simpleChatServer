#pragma once

#include "Room.h"
#include <unordered_map>
#include <memory>
#include <boost/lexical_cast.hpp>

class User {
   public:
      User() = delete;
      User(int s, std::string & name) : m_socket(s), m_name(name) {}

      void setPassword(const std::string& p) {m_password = p;}
      const std::string& getPassword() {return m_password;}

      void setRoomId(const std::string& p) {m_roomId = p;}
      const std::string& getRoomId() {return m_roomId;}

      int getSocket() {return m_socket;}

      const std::string& getName() {return m_name;}

      ~User() {}
   private:
      int m_socket;
      std::string m_name;
      std::string m_roomId;
      std::string m_password;
};
