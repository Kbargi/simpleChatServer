#pragma once

#include <queue>
#include <mutex>
#include <limits>
#include <stdexcept>
#include <memory>
#include <map>

#include <boost/lexical_cast.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

typedef boost::shared_mutex Lock;
typedef boost::lock_guard<Lock> WriteLock;
typedef boost::shared_lock<Lock> ReadLock;

template <class T>
class SharedQueue {
 public:
  SharedQueue(const size_t limit = std::numeric_limits<int>::max())
      : m_limit(limit) {}

  T front() {
    WriteLock l(m_mutex);
    if (m_queue.empty()) throw std::out_of_range("SharedQueue - empty");
    T temp = m_queue.front();
    m_queue.pop();
    return temp;
  }

  void push(T v) {
    WriteLock l(m_mutex);
    if (m_limit < m_queue.size())
      throw std::length_error(std::string("SharedQueue max size: ") +
                              boost::lexical_cast<std::string>(m_limit) +
                              std::string(" exceeded"));
    m_queue.push(v);
  }

  size_t size() {
    ReadLock l(m_mutex);
    return m_queue.size();
  }

 private:
  std::queue<T> m_queue;
  const size_t m_limit;
  Lock m_mutex;
};

template <class T, class Container, class Comparator>
class PrioritySharedQueue {
 public:
  PrioritySharedQueue(const size_t limit = std::numeric_limits<int>::max())
      : m_limit(limit) {}

  T top() {
    WriteLock l(m_mutex);
    if (m_queue.empty()) throw std::out_of_range("PrioritySharedQueue - empty");
    T temp = m_queue.top();
    m_queue.pop();
    return temp;
  }

  void push(T v) {
    WriteLock l(m_mutex);
    if (m_limit < m_queue.size())
      throw std::length_error(std::string("PrioritySharedQueue max size: ") +
                              boost::lexical_cast<std::string>(m_limit) +
                              std::string(" exceeded"));
    m_queue.push(v);
  }

  size_t size() {
    ReadLock l(m_mutex);
    return m_queue.size();
  }
  bool empty() {
    ReadLock l(m_mutex);
    return m_queue.empty();
  }

 private:
  std::priority_queue<T, Container, Comparator> m_queue;
  const size_t m_limit;
  Lock m_mutex;
};
