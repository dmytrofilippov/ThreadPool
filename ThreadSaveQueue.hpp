#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

template <typename T> class ThreadSaveQueue {
public:
  void push(T &&value) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(std::move(value));
    condVar.notify_one();
  }

  std::optional<T> try_front_pop() {
    std::lock_guard<std::mutex> lock(mutex);
    if (queue.empty()) {
      return std::nullopt;
    }
    auto value = queue.front();
    queue.pop();
    return value;
  }

  T wait_front_pop() {
    std::unique_lock<std::mutex> lock(mutex);
    condVar.wait(lock, [this] { return !queue.empty(); });
    auto value = queue.front();
    queue.pop();
    return value;
  }

  bool empty() {
    std::lock_guard<std::mutex> lock(mutex);
    return queue.empty();
  }

private:
  std::mutex mutex;
  std::queue<T> queue;
  std::condition_variable condVar;
};