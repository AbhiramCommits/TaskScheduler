#include <chrono>
#include <iostream>
#include <thread>

#include "Task.h"
#include "TaskScheduler.h"

int main() {
    TaskScheduler scheduler;
    scheduler.start(4);

    scheduler.submit(new PrintTask("Task A - low priority", 1));
    scheduler.submit(new PrintTask("Task B - medium priority", 5));
    scheduler.submit(new PrintTask("Task C - high priority", 10));
    scheduler.submit(new PrintTask("Task D - low priority", 2));
    scheduler.submit(new PrintTask("Task E - medium priority", 7));
    scheduler.submit(new PrintTask("Task F - highest priority", 15));

    std::this_thread::sleep_for(std::chrono::seconds(1));
    scheduler.stop();

    return 0;
}
