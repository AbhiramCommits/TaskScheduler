#pragma once

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
