#pragma once

#define ROOMS_ARRAY_LEN 1000

#include <sys/types.h>
#include <sys/socket.h>

#include <string>
#include <functional>
#include <utility>
#include <unordered_map>

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

#include "chat.pb.h"
#include "Room.h"
#include "SharedContainers.h"
class Room;
class RoomManager {
   typedef boost::shared_mutex Lock;
   typedef boost::unique_lock< Lock > WriteLock;
   typedef boost::shared_lock< Lock > ReadLock;

   public:
      RoomManager(){}
      ~RoomManager() {}

      void processRequest(int, chat::Request&);
      void static sendResponse(int, const std::string&, chat::Response_Type, chat::Response_ResultCode, const std::string&, const std::string&, const std::string&);
   private:

      void processDelivery(int, std::string&, std::string&, std::string&, std::string&);
      void processRoomCreation(int, std::string&, std::string&, std::string&, std::string&);
      void processJoin(int, std::string&, std::string&, std::string&, std::string&);
      void processLeave(int, std::string&, std::string&, std::string&);

      std::hash<std::string> m_hashFunc;

      struct RoomGroup {
         std::unordered_map<std::string, std::shared_ptr<Room>> rooms;
         Lock mutex;
      } m_rooms[ROOMS_ARRAY_LEN];

      SharedMap<int, std::string, std::unordered_map<int, std::string>> m_sock2RoomName;

      struct RoomGroup& getGroup(std::string& key) {
         return m_rooms[m_hashFunc(key) % ROOMS_ARRAY_LEN];
      }
};
