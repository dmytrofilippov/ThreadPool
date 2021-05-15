#pragma once

#include "ThreadSaveQueue.hpp"

#include <atomic>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>

class ThreadPool {
public:
  ThreadPool() {
    const size_t countThreads = std::thread::hardware_concurrency();
    for (size_t i = 0; i < countThreads; ++i) {
      threads.emplace_back(std::thread(&ThreadPool::worker, this));
    }
  }

  ~ThreadPool() {
    if (!stopPool) {
      shutdown();
    }
  }

  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;

  void submit(std::function<void()>(func)) { workQueue.push(std::move(func)); }

  void shutdown() {
    for (size_t i = 0; i < threads.size(); ++i) {
      workQueue.push(nullptr);
    }

    for (auto &thread : threads) {
      thread.join();
    }

    stopPool = true;
  }

private:
  void worker() {
    while (true) {

      auto task = workQueue.wait_front_pop();

      if (task == nullptr) { // shutdown signal
        break;
      }
      task();
    }
  }

  std::vector<std::thread> threads;
  ThreadSaveQueue<std::function<void()>> workQueue;
  std::atomic_bool stopPool{false};
};