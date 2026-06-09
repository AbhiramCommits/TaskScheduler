#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <stdexcept>
#include <thread>

#include "Task.h"
#include "TaskScheduler.h"

class CounterTask : public Task {
public:
    CounterTask(std::atomic<int>& counter, int priority)
        : Task(priority, "counter"), counter_(counter) {}

    void execute() override {
        ++counter_;
    }

private:
    std::atomic<int>& counter_;
};

// === Task unit tests ===

TEST(TaskTest, PrintTask_ExecuteDoesNotThrow) {
    PrintTask task("hello", 1);
    EXPECT_NO_THROW(task.execute());
}

TEST(TaskTest, FaultyTask_ExecuteThrows) {
    FaultyTask task("faulty", 1);
    EXPECT_THROW(task.execute(), std::runtime_error);
}

// === TaskScheduler unit tests ===

TEST(TaskSchedulerTest, StartAndStop) {
    TaskScheduler scheduler;
    scheduler.start(2);
    scheduler.stop();
    SUCCEED();
}

TEST(TaskSchedulerTest, SubmitAfterStop_Throws) {
    TaskScheduler scheduler;
    scheduler.start(1);
    scheduler.stop();
    PrintTask task("test", 1);
    EXPECT_THROW(scheduler.submit(&task), std::runtime_error);
}

TEST(TaskSchedulerTest, DoubleStart_Throws) {
    TaskScheduler scheduler;
    scheduler.start(1);
    EXPECT_THROW(scheduler.start(1), std::runtime_error);
    scheduler.stop();
}

// === Integration tests ===

TEST(TaskSchedulerIntegrationTest, ProcessesAllSubmittedTasks) {
    TaskScheduler scheduler;
    scheduler.start(4);
    std::atomic<int> counter{0};

    for (int i = 0; i < 10; ++i) {
        scheduler.submit(new CounterTask(counter, i));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    scheduler.stop();

    EXPECT_EQ(counter.load(), 10);
}

TEST(TaskSchedulerIntegrationTest, HandlesExceptionsAndContinues) {
    TaskScheduler scheduler;
    scheduler.start(4);

    scheduler.submit(new FaultyTask("faulty-1", 1));
    scheduler.submit(new PrintTask("print-1", 2));
    scheduler.submit(new FaultyTask("faulty-2", 3));
    scheduler.submit(new PrintTask("print-2", 4));
    scheduler.submit(new FaultyTask("faulty-3", 5));
    scheduler.submit(new PrintTask("print-3", 6));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::atomic<int> alive{0};
    EXPECT_NO_THROW(scheduler.submit(new CounterTask(alive, 1)));

    scheduler.stop();
    SUCCEED();
}
