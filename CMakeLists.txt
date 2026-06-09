cmake_minimum_required(VERSION 3.10)
project(TaskScheduler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(scheduler_lib
    src/Task.cpp
    src/TaskScheduler.cpp
)

target_include_directories(scheduler_lib PUBLIC include)

add_executable(TaskScheduler main.cpp)
target_link_libraries(TaskScheduler PRIVATE scheduler_lib)
