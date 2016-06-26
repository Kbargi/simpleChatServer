#pragma once

#include <memory>
#include <mutex>

template <typename T>
class Singleton {
 public:
  Singleton() = delete;
  Singleton(const Singleton&) = delete;

  static T& getInstance() {
    std::call_once(m_flag, [] { m_instance.reset(new T); });
    return *m_instance.get();
  }

 private:
  static std::unique_ptr<T> m_instance;
  static std::once_flag m_flag;
};

template <typename T>
std::once_flag Singleton<T>::m_flag;

template <typename T>
std::unique_ptr<T> Singleton<T>::m_instance;
