cmake_minimum_required(VERSION 3.14)
project(TaskScheduler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

add_library(scheduler_lib
    src/Task.cpp
    src/TaskScheduler.cpp
)

target_include_directories(scheduler_lib PUBLIC include)
target_link_libraries(scheduler_lib PUBLIC Threads::Threads)

add_executable(TaskScheduler main.cpp)
target_link_libraries(TaskScheduler PRIVATE scheduler_lib)

enable_testing()

add_executable(TaskSchedulerTests tests/TaskSchedulerTests.cpp)
target_link_libraries(TaskSchedulerTests PRIVATE scheduler_lib gtest_main)

add_test(NAME TaskSchedulerTests COMMAND TaskSchedulerTests)
