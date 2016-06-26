#include <chrono>

#include "ThreadPool.hpp"

ThreadPool::ThreadPool(size_t threads_limit)
    : m_size(threads_limit < 2 ? 2 : threads_limit) {
  stopFlag.store(false);
  threads_limit = m_size;
  while (threads_limit--) {
    m_pool.push_back(std::make_shared<std::thread>(&ThreadPool::run, this));
  }
  std::cout << "THREADS: " << m_size << "\n";
}

void ThreadPool::add(PTask task) {
  if (task) {
    try {
      std::unique_lock<std::mutex> lck(m_mutex);
      m_tasks.push(task);
    } catch (std::length_error& e) {
      std::cout << e.what() << "\n";
    } catch (...) {
      std::cout << "Unknown exception while trying to push task\n";
    }
    cv.notify_one();
  }
}

void ThreadPool::run() {
  PTask task;
  while (!stopFlag.load()) {
    task.reset();
    {
      std::unique_lock<std::mutex> lck(m_mutex);
      while (m_tasks.empty() && !stopFlag.load()) cv.wait(lck);
      try {
        task = m_tasks.top();
      } catch (std::out_of_range& e) {
        std::cout << e.what() << "\n";
      } catch (...) {
        std::cout << "Unknown exception while trying to get task\n";
      }
    }
    try {
      if (task) (*task)();  // call task
    } catch (...) {
      std::cout << "Unknown exception while trying to execute task\n";
    }
  }
}
size_t ThreadPool::size() { return m_tasks.size(); }
void ThreadPool::stop() {
  stopFlag.store(true);
  cv.notify_all();

  for (std::vector<std::shared_ptr<std::thread>>::iterator it = m_pool.begin();
       it != m_pool.end(); ++it) {
    if ((*it)->joinable()) {
      (*it)->join();
    }
  }
}

ThreadPool::~ThreadPool() { this->stop(); }
