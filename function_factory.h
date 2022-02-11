#pragma once

#include <memory>
#include <unordered_map>

#include "types.h"

class FunctionFactory {
private:
    std::unordered_map<std::string, std::shared_ptr<Function>> map_;

public:
    FunctionFactory();

    bool HasFunction(const std::string& name);

    std::shared_ptr<Function> GetFunction(const std::string& name);

    template <typename T>
    void AddFunction();

    template <typename T>
    void AddFunction(std::string name);
};
