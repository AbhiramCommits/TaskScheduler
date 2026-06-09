# TaskScheduler

A thread-safe C++17 priority task scheduler that uses a configurable worker thread pool, a mutex-guarded `std::priority_queue`, and RAII synchronization to safely execute user-defined tasks. Tasks are submitted as heap-allocated objects and automatically cleaned up after execution. The scheduler catches task exceptions to keep worker threads alive and provides graceful shutdown with queue draining.

## Architecture

```
Task (abstract base, priority + id + execute())
├── PrintTask       (prints a message)
├── FaultyTask      (always throws, used for testing)
└── CounterTask     (increments an atomic<int>, used in tests)

TaskScheduler
├── std::priority_queue<Task*>   (ordered by priority, highest first)
├── std::vector<std::thread>     (worker thread pool)
├── std::mutex                   (guards queue access)
├── std::condition_variable      (signals workers on new tasks / shutdown)
└── bool running_                (controls worker loop lifecycle)
```

## Build Instructions

```bash
# Configure
cmake -S . -B build

# Build
cmake --build build

# Run tests
cd build && ctest --output-on-failure
```

Dependencies are fetched automatically — GoogleTest v1.14.0 is pulled via CMake `FetchContent`. A C++17 compiler and CMake 3.14+ are required.

## Usage Example

```cpp
#include "Task.h"
#include "TaskScheduler.h"

class MyTask : public Task {
public:
    MyTask(std::string id, int priority) : Task(priority, std::move(id)) {}
    void execute() override {
        // do work here
    }
};

int main() {
    TaskScheduler scheduler;
    scheduler.start(4);

    scheduler.submit(new MyTask("task-1", 10));
    scheduler.submit(new MyTask("task-2", 5));

    scheduler.stop();
    return 0;
}
```

## Design Decisions

- **`std::priority_queue` over manual sorting** — provides O(log n) insertion and O(1) extraction of the highest-priority task. A custom `TaskCompare` functor orders by descending priority so workers always pick the most important pending task.
- **`std::condition_variable` over busy-wait** — workers sleep on the condition variable until a task is available or shutdown is signaled, avoiding CPU waste. `cv.notify_one()` wakes exactly one thread per submission, reducing unnecessary wake-ups.
- **RAII over manual lock/unlock** — `std::lock_guard` and `std::unique_lock` scope-lock the mutex, eliminating the risk of forgetting an unlock in an early return or exception path.
- **Exception isolation** — every `task->execute()` call is wrapped in a try/catch block so a single failing task cannot crash a worker thread.
- **Leak-free shutdown** — `stop()` drains any unprocessed queue entries and deletes their pointers before joining threads. The destructor calls `stop()` automatically.
