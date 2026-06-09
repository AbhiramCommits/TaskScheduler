#pragma once

class Task;

class TaskScheduler {
public:
    TaskScheduler() = default;
    ~TaskScheduler() = default;

    void start();
    void stop();
    void submit(Task* task);
};
