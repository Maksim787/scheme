#pragma once

#include <memory>
#include <string>

#include "object.h"
#include "types.h"
#include "function_factory.h"

class Interpreter {
private:
    FunctionFactory func_factory_;

    GarbageCollector collector_;

    std::shared_ptr<Type> ParseTypes(std::shared_ptr<Object> obj);

public:
    std::string Run(std::string str);

    std::string Evaluate(std::shared_ptr<Object> tree);
};
