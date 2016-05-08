#pragma once
#define LOCK(name) std::lock_guard<std::mutex> l (name);

#include <queue>
#include <mutex>
#include <limits>
#include <stdexcept>
#include <memory>

#include <boost/lexical_cast.hpp>
#include <boost/thread/shared_mutex.hpp>

template <class T>
class SharedQueue {
   public:
	  SharedQueue(const size_t limit = std::numeric_limits<int>::max()) : m_limit(limit) {}

      T front() {
         LOCK(m_mutex);
         if(m_queue.empty())
        	 throw std::out_of_range("SharedQueue - empty");
         T temp = m_queue.front();
         m_queue.pop();
         return temp;
      }

      void push(T v) {
    	  LOCK(m_mutex);
    	  if(m_limit < m_queue.size())
    		  throw std::length_error(std::string("SharedQueue max size: ") + boost::lexical_cast<std::string>(m_limit) + std::string(" exceeded"));
    	  m_queue.push(v);
      }

      const size_t size() {LOCK(m_mutex); return m_queue.size();}

   private:

      std::queue<T> m_queue;
      const size_t m_limit;
      std::mutex m_mutex;
};
template <class key, class value, class Container = std::unordered_map<key, value>>
class SharedMap {
   public:
      void remove(key k) {
         LOCK(m_mutex);
         m_container.erase(k);
      }
      void add(key k, value v) {
         LOCK(m_mutex);
         m_container[k] = v;
      }
      value get(key k) {
         LOCK(m_mutex);
         Container::iterator it = m_container.find(k);
         return it != m_container.end() ? it->second : value();
      }
   private:
      Container m_container;
      std::mutex m_mutex;
};
template <class T, class Container, class Comparator>
class PrioritySharedQueue {
   public:
      PrioritySharedQueue(const size_t limit = std::numeric_limits<int>::max()) : m_limit(limit) {}

      T top() {
          LOCK(m_mutex);
          if(m_queue.empty())
    	      throw std::out_of_range("PrioritySharedQueue - empty");
    	  T temp = m_queue.top();
    	  m_queue.pop();
    	  return temp;
      }

      void push(T v) {
    	  LOCK(m_mutex);
    	  if(m_limit < m_queue.size())
    		  throw std::length_error(std::string("PrioritySharedQueue max size: ") + boost::lexical_cast<std::string>(m_limit) + std::string(" exceeded"));
    	  m_queue.push(v);
      }

      const size_t size() {LOCK(m_mutex); return m_queue.size();}
      bool empty() {LOCK(m_mutex); return m_queue.empty();}

   private:

      std::priority_queue<T, Container, Comparator> m_queue;
      const size_t m_limit;
      std::mutex m_mutex;
};

class FileDescriptorSharedWrapper {
   public:
      FileDescriptorSharedWrapper() : maxFd(-9999) {}
      void clearAll() {
         LOCK(m_mutex);
         FD_ZERO(&mMasterSet);
	  }
      void add(int newfd) {
         LOCK(m_mutex);
         maxFd = newfd > maxFd ? newfd : maxFd;
         FD_SET(newfd, &mMasterSet); // add to master set
      }
      void remove(int fd) {
         LOCK(m_mutex);
         FD_CLR(fd, &mMasterSet); // remove from master set
      }
      bool isSet(int fd) {
         LOCK(m_mutex);
         return FD_ISSET(fd , &mMasterSet);
      }
      fd_set getCopy() {
         LOCK(m_mutex);
         return mMasterSet;
      }
      int getMaxFd() {
         return maxFd;
      }
   private:
      std::mutex m_mutex;
      fd_set mMasterSet;
      int maxFd;
};
