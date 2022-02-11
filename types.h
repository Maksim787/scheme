#pragma once

#include <memory>
#include <queue>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "object.h"
#include "error.h"

struct Context;

class GarbageCollector;

struct Type : public std::enable_shared_from_this<Type> {
    virtual std::string Repr() = 0;

    virtual ~Type() = default;

    virtual std::shared_ptr<Type> Evaluate(Context* context) = 0;
};

struct Context {
    GarbageCollector* collector = nullptr;
    Context* parent = nullptr;
    std::unordered_map<std::string, std::shared_ptr<Type>> var;

    Context(GarbageCollector* collector) : collector(collector) {
    }

    Context(Context* parent) : collector(parent->collector), parent(parent) {
    }

    Context(const Context& other)
        : collector(other.collector), parent(other.parent), var(other.var) {
    }

    std::shared_ptr<Type> Add(const std::string& name, std::shared_ptr<Type> val) {
        var[name] = val;
        return val;
    }

    std::shared_ptr<Type>& Get(const std::string& name) {
        if (!var.contains(name)) {
            if (!parent) {
                throw NameError("Get() got unknown name");
            }
            return parent->Get(name);  // Get(name);
        }
        return var[name];
    }
};

class GarbageCollector {
private:
    std::vector<std::unique_ptr<Context>> memory_;
    Context* root_;

public:
    GarbageCollector() : root_(new Context(this)) {
        memory_.emplace_back(root_);
    }

    Context* GetRoot() {
        return root_;
    }

    Context* Allocate(Context* parent) {
        memory_.emplace_back(new Context(parent));
        return memory_.back().get();
    }

    void Clear();
};

struct Function : public Type {
public:
    virtual ~Function() = default;

    virtual std::shared_ptr<Type> Apply(std::shared_ptr<Type> arguments, Context* context) = 0;

    virtual std::shared_ptr<Type> Evaluate(Context*) override {
        return shared_from_this();
    }
};

class Integer : public Type {
    int value_;

public:
    Integer(int value) : value_(value) {
    }

    int GetValue() {
        return value_;
    }

    std::string Repr() override {
        return std::to_string(value_);
    }

    std::shared_ptr<Type> Evaluate(Context*) override {
        return shared_from_this();
    }
};

class Bool : public Type {
private:
    bool value_;

public:
    explicit Bool(bool value) : value_(value) {
    }

    Bool(std::string value) : value_(value == "#t") {
    }

    bool GetValue() {
        return value_;
    }

    std::string Repr() override {
        return value_ ? "#t" : "#f";
    }

    std::shared_ptr<Type> Evaluate(Context*) override {
        return shared_from_this();
    }
};

class Pair : public Type {
private:
    bool proper_list_ = false;
    std::shared_ptr<Type> first_;
    std::shared_ptr<Type> second_;

public:
    Pair(std::shared_ptr<Type> first, std::shared_ptr<Type> second);

    std::string Repr() override;

    std::shared_ptr<Type> Evaluate(Context*) override;

    std::shared_ptr<Type> GetFirst() {
        if (!first_) {
            throw RuntimeError("first=nullptr");
        }
        return first_;
    }

    std::shared_ptr<Type> GetSecond() {
        if (!second_) {
            throw RuntimeError("second=nullptr");
        }
        return second_;
    }

    void SetFirst(std::shared_ptr<Type> first) {
        first_ = first;
    }

    void SetSecond(std::shared_ptr<Type> second) {
        second_ = second;
    }

    bool ProperList() {
        return proper_list_;
    }

    bool Empty() {
        return !first_;
    }

    static std::shared_ptr<Pair> EmptyPair() {
        return std::make_shared<Pair>(nullptr, nullptr);
    }
};

struct UnknownSymbol : public Type {
    std::string name;

    UnknownSymbol(std::string name);

    std::string Repr() override;

    std::shared_ptr<Type> Evaluate(Context* context) override;
};

template <typename T>
bool IsType(std::shared_ptr<Type> type);

template <typename T>
std::shared_ptr<T> AsType(std::shared_ptr<Type> type);
