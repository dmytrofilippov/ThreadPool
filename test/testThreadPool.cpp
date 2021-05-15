#include "../ThreadPool.hpp"

#include <atomic>
#include <chrono>
#include <iostream>

#include <catch2/catch.hpp>

using namespace std::chrono_literals;

TEST_CASE("testing ThreadPool") {

  SECTION("simple test") {
    ThreadPool pool;
    pool.submit([] { std::cout << "Hello from pool\n"; });

    pool.shutdown();
  }

  SECTION("many tasks") {
    ThreadPool pool;

    constexpr uint Tasks = 42;
    std::atomic<uint> poolTasks{0};

    for (size_t i = 0; i < Tasks; ++i) {
      pool.submit([&] { ++poolTasks; });
    }

    pool.shutdown();

    CHECK(Tasks == poolTasks.load());
  }

  SECTION("shutdown") {
    ThreadPool pool;

    std::atomic_bool done{false};
    pool.submit([&] { done = true; });

    CHECK(done.load() == false);

    pool.shutdown();

    CHECK(done.load() == true);
  }

  SECTION("parallel") {
    ThreadPool pool;

    std::atomic_int tasks{0};

    pool.submit([&] {
      std::this_thread::sleep_for(500ms);
      ++tasks;
    });

    pool.submit([&] { ++tasks; });

    std::this_thread::sleep_for(200ms);

    CHECK(tasks.load() == 1);

    pool.shutdown();

    CHECK(tasks.load() == 2);
  }
}