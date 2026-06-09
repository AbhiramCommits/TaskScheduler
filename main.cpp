#include <chrono>
#include <iostream>
#include <thread>

#include "Task.h"
#include "TaskScheduler.h"

int main() {
    TaskScheduler scheduler;
    scheduler.start(4);

    scheduler.submit(new PrintTask("Task A - low priority", 1));
    scheduler.submit(new FaultyTask("Faulty-1", 10));
    scheduler.submit(new PrintTask("Task B - medium priority", 5));
    scheduler.submit(new FaultyTask("Faulty-2", 3));
    scheduler.submit(new PrintTask("Task C - high priority", 12));
    scheduler.submit(new PrintTask("Task D - low priority", 2));

    std::this_thread::sleep_for(std::chrono::seconds(1));
    scheduler.stop();

    try {
        scheduler.submit(new PrintTask("This should throw", 1));
    } catch (const std::exception& e) {
        std::cout << "Caught: " << e.what() << std::endl;
    }

    return 0;
}
