#pragma once

#include <iostream>
#include <stdexcept>
#include <string>

class Task {
public:
    Task(int priority, std::string id)
        : priority_(priority), id_(std::move(id)) {}

    virtual ~Task() = default;

    virtual void execute() = 0;

    int priority() const { return priority_; }
    const std::string& id() const { return id_; }

protected:
    int priority_;
    std::string id_;
};

class PrintTask : public Task {
public:
    PrintTask(std::string message, int priority)
        : Task(priority, message), message_(std::move(message)) {}

    void execute() override {
        std::cout << message_ << std::endl;
    }

private:
    std::string message_;
};

class FaultyTask : public Task {
public:
    FaultyTask(std::string id, int priority)
        : Task(priority, std::move(id)) {}

    void execute() override {
        throw std::runtime_error("task failed");
    }
};
