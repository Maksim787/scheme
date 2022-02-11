#pragma once

#include <memory>

#include "error.h"

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
};

class Number : public Object {
    int value_;

public:
    Number(int value) : value_(value) {
    }

    int GetValue() const {
        return value_;
    }
};

class Symbol : public Object {
    std::string name_;

public:
    Symbol(std::string name) : name_(name) {
    }

    const std::string& GetName() const {
        return name_;
    }
};

class Cell : public Object {
    std::shared_ptr<Object> first_, second_;

public:
    Cell() : first_(nullptr), second_(nullptr) {
    }

    Cell(std::shared_ptr<Object> first, std::shared_ptr<Object> second)
        : first_(first), second_(second) {
    }

    void SetFirst(std::shared_ptr<Object> first) {
        first_ = first;
    }

    void SetSecond(std::shared_ptr<Object> second) {
        second_ = second;
    }

    std::shared_ptr<Object> GetFirst() const {
        return first_;
    }

    std::shared_ptr<Object> GetSecond() const {
        return second_;
    }
};

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    if (!Is<T>(obj)) {
        throw SyntaxError("As()");
    }
    return std::static_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    return bool(std::dynamic_pointer_cast<T>(obj));
}
