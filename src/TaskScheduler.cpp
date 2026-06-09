#include "TaskScheduler.h"
#include "Task.h"

#include <iostream>
#include <stdexcept>

void TaskScheduler::start(int numThreads) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (running_) {
        throw std::runtime_error("scheduler is already running");
    }
    running_ = true;
    for (int i = 0; i < numThreads; ++i) {
        workers_.emplace_back(&TaskScheduler::workerLoop, this);
    }
}

void TaskScheduler::stop() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        running_ = false;
        while (!queue_.empty()) {
            delete queue_.top();
            queue_.pop();
        }
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
        if (!running_) {
            throw std::runtime_error("cannot submit task: scheduler is not running");
        }
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
            try {
                task->execute();
            } catch (const std::exception& e) {
                std::cerr << "Task exception: " << e.what() << std::endl;
            }
            delete task;
        }
    }
}
