#pragma once

#include <atomic>
#include <chrono>
#include <thread>

#include "chat.pb.h"
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

enum class TaskPriority { HIGH, NORMAL, LOW };

class AbstractTask {
 public:
  AbstractTask(TaskPriority prior) : m_priority(prior) {}
  virtual ~AbstractTask(){};
  virtual void operator()() = 0;
  virtual void setPriority(TaskPriority p) { m_priority = p; }
  virtual TaskPriority getPriority() { return m_priority; }

 protected:
  TaskPriority m_priority;
};
