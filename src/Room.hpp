#pragma once
#include "User.hpp"
typedef boost::shared_mutex Lock;
typedef boost::lock_guard<Lock> WriteLock;
typedef boost::shared_lock<Lock> ReadLock;
class User;
class Room {
  typedef std::vector<std::weak_ptr<User> > VECTOR;

 public:
  Room(const std::string& name, const std::string& password)
      : m_name(name), m_password(password) {}

  const std::string& getName() { return m_name; }
  const std::string& getPassword() { return m_password; }
  VECTOR& getUsers() { return m_users; }

  void addUser(std::shared_ptr<User> u) {
    WriteLock l(m_mutex);
    m_users.push_back(u);
  }
  Lock m_mutex;

 private:
  const std::string m_name;
  const std::string m_password;
  VECTOR m_users;
};
