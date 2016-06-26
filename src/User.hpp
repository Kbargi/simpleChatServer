#pragma once
#include "Room.hpp"
class User {
 public:
  User(const std::string& name, const std::string& password, const int socket)
      : m_name(name), m_password(password), m_socket(socket), m_room(nullptr) {}

  void setRoom(std::shared_ptr<Room> room) {
    WriteLock l(m_setRoomMutex);
    if (m_room) {
      throw std::logic_error(m_name + std::string(" already has a room"));
    }
    if (!room) {
      throw std::runtime_error(std::string("given room ptr is null"));
    }
    m_room = (room);
  }
  std::shared_ptr<Room> getRoom() {
    ReadLock l(m_setRoomMutex);
    return m_room;
  }

  const std::string& getName() { return m_name; }
  const std::string& getPassword() { return m_password; }
  int getSocket() { return m_socket; }

 private:
  const std::string m_name;
  const std::string m_password;
  const int m_socket;
  Lock m_setRoomMutex;
  std::shared_ptr<Room> m_room;
};
