#pragma once

#define BULKS 10

#include <sys/types.h>
#include <sys/socket.h>

#include <string>
#include <functional>
#include <utility>
#include <map>

#include "chat.pb.h"
#include "SharedContainers.hpp"
#include "User.hpp"

class Room;
class RoomManager {
 public:
  RoomManager() {}
  ~RoomManager() {}

  void processRequest(int, chat::Request&);
  void processEscape(int socket);

  static void sendResponse(const int socket, const std::string& roomName,
                           chat::Response_Type respType,
                           chat::Response_ResultCode rCode,
                           const std::string& desc,
                           const std::string& userName);

  static chat::Response_Type requestName2ResponseName(chat::Request_Type r) {
    switch (r) {
      case chat::Request::DELIVER:
        return chat::Response::DELIVER;
      case chat::Request::CREATE_ROOM:
        return chat::Response::CREATE_ROOM;
      case chat::Request::JOIN_2_ROOM:
        return chat::Response::JOIN_2_ROOM;
      default:
        return chat::Response::UNKNOWN;
    }
  }
  static chat::Request_Type responseName2RequestName(chat::Request_Type r) {
    switch (r) {
      case chat::Response::DELIVER:
        return chat::Request::DELIVER;
      case chat::Response::CREATE_ROOM:
        return chat::Request::CREATE_ROOM;
      case chat::Response::JOIN_2_ROOM:
        return chat::Request::JOIN_2_ROOM;
      default:
        return chat::Request::UNKNOWN;
    }
  }

 private:
  void processDelivery(int socket, const std::string& roomName,
                       const std::string& roomPassword,
                       const std::string& userName,
                       const std::string& userPassword,
                       const std::string& content);
  void processRoomCreation(int clientSocket, const std::string& roomName,
                           const std::string& roomPassword,
                           const std::string& userName,
                           const std::string& userPassword);
  void processJoin(int clientSocket, const std::string& roomName,
                   const std::string& roomPassword, const std::string& userName,
                   const std::string& userPassword);

  struct InternalData {
    Lock m_mutex;
    std::map<int /*socket*/, std::shared_ptr<User>> m_users;
    std::map<std::string /*room name*/, std::weak_ptr<Room>> m_rooms;
  } m_internalData;
};
