#include <memory>
#include <queue>
#include <vector>
#include <unordered_set>
#include <iostream>

#include "types.h"
#include "functions.h"

template <typename T>
bool IsType(std::shared_ptr<Type> type) {
    return bool(std::dynamic_pointer_cast<T>(type));
}

template <typename T>
std::shared_ptr<T> AsType(std::shared_ptr<Type> type) {
    if (!IsType<T>(type)) {
        throw RuntimeError("Invalid type in AsType()");
    }
    return std::static_pointer_cast<T>(type);
}

void GarbageCollector::Clear() {
    std::queue<Context*> bfs;
    std::unordered_set<Context*> keep;
    bfs.push(root_);
    keep.insert(root_);
    while (!bfs.empty()) {
        Context* context = bfs.front();
        bfs.pop();
        for (auto [v, type_ptr] : context->var) {
            if (!IsType<Lambda>(type_ptr)) {
                continue;
            }
            auto func_ptr = AsType<Lambda>(type_ptr);
            Context* next_context = func_ptr->created_context;
            if (!keep.contains(next_context)) {
                keep.insert(next_context);
                bfs.push(next_context);
            }
        }
    }
    std::vector<std::unique_ptr<Context>> new_memory;
    for (size_t i = 0; i < memory_.size(); ++i) {
        if (keep.contains(memory_[i].get())) {
            new_memory.push_back(std::move(memory_[i]));
        }
    }
    size_t cleared = memory_.size() - new_memory.size();
//    if (cleared > 0) {
//        std::cout << "Cleared: " << cleared << "\n";
//    }
    memory_ = std::move(new_memory);
}

Pair::Pair(std::shared_ptr<Type> first, std::shared_ptr<Type> second)
    : first_(first), second_(second) {
    if (!first_ && !second_) {
        proper_list_ = true;
        return;
    }
    if (IsType<Pair>(second)) {
        auto pair = AsType<Pair>(second);
        if (pair->proper_list_) {
            proper_list_ = true;
        }
    }
}

std::string Pair::Repr() {
    if (Empty()) {
        return "()";
    }
    std::shared_ptr<Pair> curr = AsType<Pair>(shared_from_this());
    std::string result = "(";
    while (true) {
        if (curr->Empty()) {
            result.back() = ')';
            return result;
        }
        result += curr->first_->Repr();
        if (IsType<Pair>(curr->second_)) {
            result += " ";
            curr = AsType<Pair>(curr->second_);
        } else {
            result += " . ";
            result += curr->second_->Repr();
            result += ")";
            return result;
        }
    }
}

std::shared_ptr<Type> Pair::Evaluate(Context* context) {
    if (Empty()) {
        throw RuntimeError("Evaluate empty list ()");
    }
    return AsType<Function>(first_->Evaluate(context))->Apply(second_, context);
}

UnknownSymbol::UnknownSymbol(std::string name) : name(std::move(name)) {
}

std::string UnknownSymbol::Repr() {
    return name;
}

std::shared_ptr<Type> UnknownSymbol::Evaluate(Context* context) {
    return context->Get(name);
}
