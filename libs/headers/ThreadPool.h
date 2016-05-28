#pragma once

#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <limits>
#include <iostream>
#include "SharedContainers.h"
#include "AbstractTask.h"

class ThreadPool;

class TaskComparator {
 public:
  bool operator()(const std::shared_ptr<AbstractTask> t1,
                  const std::shared_ptr<AbstractTask> t2) {
    return t1->getPriority() > t2->getPriority();
  }
};

class ThreadPool {
 public:
  typedef std::shared_ptr<AbstractTask> PTask;
  typedef std::vector<PTask> VPTask;

  ThreadPool(size_t size = std::thread::hardware_concurrency());
  ~ThreadPool();
  void add(PTask);
  void stop();
  size_t size();

 private:
  void run();

  const size_t m_size;
  std::atomic<bool> stopFlag;
  std::condition_variable cv;
  std::mutex m_mutex;
  PrioritySharedQueue<PTask, VPTask, TaskComparator> m_tasks;
  std::vector<std::shared_ptr<std::thread>> m_pool;
};
