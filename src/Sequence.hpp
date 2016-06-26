#pragma once

#include <atomic>
#include <limits>

class Sequence {
 public:
  typedef unsigned long long int valueType;

  Sequence() = delete;
  Sequence(const valueType min = 0,
           const valueType range = std::numeric_limits<valueType>::max() - 1)
      : m_value(min), m_min(min), m_range(range) {}
  ~Sequence() {}

  const valueType nextVal() const { return (m_value++) % m_range + m_min; }
  const valueType currVal() const { return m_value % m_range + m_min; }
  void setVal(const valueType v) { m_value = (v < m_min) ? m_min : v; }

 private:
  mutable std::atomic<valueType> m_value;
  const valueType m_min;
  const valueType m_range;
};
