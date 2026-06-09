#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "Task.h"

struct TaskCompare {
    bool operator()(const Task* a, const Task* b) const {
        return a->priority() < b->priority();
    }
};

class TaskScheduler {
public:
    TaskScheduler() = default;

    ~TaskScheduler() {
        if (running_) {
            stop();
        }
    }

    void start(int numThreads);
    void stop();
    void submit(Task* task);

private:
    void workerLoop();

    std::priority_queue<Task*, std::vector<Task*>, TaskCompare> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool running_ = false;
    std::vector<std::thread> workers_;
};
