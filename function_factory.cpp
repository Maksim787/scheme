#include <memory>
#include <string>

#include "function_factory.h"
#include "functions.h"

FunctionFactory::FunctionFactory() {
    AddFunction<Quote>();
    AddFunction<BooleanPred>();
    AddFunction<Not>();
    AddFunction<And>();
    AddFunction<Or>();
    AddFunction<NumberPred>();
    AddFunction<Equal>();
    AddFunction<Less>();
    AddFunction<Greater>();
    AddFunction<LessEqual>();
    AddFunction<GreaterEqual>();
    AddFunction<Addition>();
    AddFunction<Subtraction>();
    AddFunction<Multiplication>();
    AddFunction<Division>();
    AddFunction<Min>();
    AddFunction<Max>();
    AddFunction<Abs>();
    AddFunction<PairPred>();
    AddFunction<NullPred>();
    AddFunction<ListPred>();
    AddFunction<Cons>();
    AddFunction<Car>();
    AddFunction<Cdr>();
    AddFunction<List>();
    AddFunction<ListRef>();
    AddFunction<ListTail>();
    AddFunction<SymbolPred>();
    AddFunction<Define>();
    AddFunction<Set>();
    AddFunction<If>();
    AddFunction<SetCar>();
    AddFunction<SetCdr>();
    AddFunction<LambdaCreate>();
}

std::shared_ptr<Function> FunctionFactory::GetFunction(const std::string& name) {
    if (!map_.contains(name)) {
        throw RuntimeError("Function not found");
    }
    return map_[name];
}

bool FunctionFactory::HasFunction(const std::string& name) {
    return map_.contains(name);
}

template <typename T>
void FunctionFactory::AddFunction() {
    auto ptr = std::make_shared<T>();
    map_[ptr->Repr()] = ptr;
}

template <typename T>
void FunctionFactory::AddFunction(std::string name) {
    map_[name] = std::make_shared<T>(name);
}
