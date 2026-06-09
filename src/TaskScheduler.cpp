#include "TaskScheduler.h"
#include "Task.h"

void TaskScheduler::start(int numThreads) {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = true;
    for (int i = 0; i < numThreads; ++i) {
        workers_.emplace_back(&TaskScheduler::workerLoop, this);
    }
}

void TaskScheduler::stop() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        running_ = false;
    }
    cv_.notify_all();
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    workers_.clear();
}

void TaskScheduler::submit(Task* task) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(task);
    }
    cv_.notify_one();
}

void TaskScheduler::workerLoop() {
    while (true) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] {
            return !running_ || !queue_.empty();
        });

        if (!running_ && queue_.empty()) {
            return;
        }

        if (!queue_.empty()) {
            Task* task = queue_.top();
            queue_.pop();
            lock.unlock();
            task->execute();
            delete task;
        }
    }
}
