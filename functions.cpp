#include <memory>

#include "types.h"
#include "functions.h"

// -----------------------------------------------------------
// Quote
std::shared_ptr<Type> Quote::Apply(std::shared_ptr<Type> arg, Context*) {
    return arg;
}

// -----------------------------------------------------------
// BooleanPred
std::shared_ptr<Type> BooleanPred::Apply(std::shared_ptr<Type> arg, Context* context) {
    arg = Helper::GetOneEvaluated(arg, context);
    return std::make_shared<Bool>(IsType<Bool>(arg));
}

// -----------------------------------------------------------
// Not
std::shared_ptr<Type> Not::Apply(std::shared_ptr<Type> arg, Context* context) {
    arg = Helper::GetOneEvaluated(arg, context);
    return std::make_shared<Bool>(!(Helper::ConvertToBool(arg)));
}

// -----------------------------------------------------------
// And
std::shared_ptr<Type> And::Apply(std::shared_ptr<Type> arg, Context* context) {
    auto args = Helper::GetAll(arg);
    arg = std::make_shared<Bool>(true);
    for (size_t i = 0; i < args.size(); ++i) {
        arg = args[i]->Evaluate(context);
        if (!Helper::ConvertToBool(arg)) {
            break;
        }
    }
    return arg;
}

// -----------------------------------------------------------
// Or
std::shared_ptr<Type> Or::Apply(std::shared_ptr<Type> arg, Context* context) {
    auto args = Helper::GetAll(arg);
    arg = std::make_shared<Bool>(false);
    for (size_t i = 0; i < args.size(); ++i) {
        arg = args[i]->Evaluate(context);
        if (Helper::ConvertToBool(arg)) {
            break;
        }
    }
    return arg;
}

// -----------------------------------------------------------
// NumberPred
std::shared_ptr<Type> NumberPred::Apply(std::shared_ptr<Type> arg, Context* context) {
    arg = Helper::GetOneEvaluated(arg, context);
    return std::make_shared<Bool>(IsType<Integer>(arg));
}

// -----------------------------------------------------------
// IntegerOperation
std::shared_ptr<Type> IntegerOperation::Apply(std::shared_ptr<Type> arg, Context* context) {
    auto args = Helper::GetAll(arg);
    std::shared_ptr<Integer> result = FirstElem();
    size_t start_ind = 0;
    // нет начального значения
    if (result->GetValue() == -1) {
        if (args.size() < 2) {
            throw RuntimeError("Too few arguments for IntegerOperation");
        }
        result = Operation(AsType<Integer>(args[0]->Evaluate(context)),
                           AsType<Integer>(args[1]->Evaluate(context)));
        start_ind = 2;
    }
    for (size_t i = start_ind; i < args.size(); ++i) {
        result = Operation(result, AsType<Integer>(args[i]->Evaluate(context)));
    }
    return result;
}

// -----------------------------------------------------------
// Compare
std::shared_ptr<Type> Compare::Apply(std::shared_ptr<Type> arg, Context* context) {
    auto args = Helper::GetAll(arg);
    if (args.empty()) {
        return std::make_shared<Bool>(true);
    }
    if (args.size() == 1) {
        throw RuntimeError("Compare 1 element");
    }
    for (size_t i = 0; i < args.size() - 1; ++i) {
        if (!Comparator(AsType<Integer>(args[i]->Evaluate(context)),
                        AsType<Integer>(args[i + 1]->Evaluate(context)))) {
            return std::make_shared<Bool>(false);
        }
    }
    return std::make_shared<Bool>(true);
}

// -----------------------------------------------------------
// MinMax
std::shared_ptr<Type> MinMax::Apply(std::shared_ptr<Type> arg, Context* context) {
    auto args = Helper::GetAll(arg);
    if (args.empty()) {
        throw RuntimeError("Empty args in MinMax");
    }
    std::shared_ptr<Integer> result = AsType<Integer>(args[0]->Evaluate(context));
    for (size_t i = 1; i < args.size(); ++i) {
        result = Operation(result, AsType<Integer>(args[i]->Evaluate(context)));
    }
    return result;
}

// -----------------------------------------------------------
// Abs
std::shared_ptr<Type> Abs::Apply(std::shared_ptr<Type> arg, Context* context) {
    arg = Helper::GetOneEvaluated(arg, context);
    return std::make_shared<Integer>(std::abs(AsType<Integer>(arg)->GetValue()));
}

// -----------------------------------------------------------
// PairPred
std::shared_ptr<Type> PairPred::Apply(std::shared_ptr<Type> arg, Context* context) {
    arg = Helper::GetOneEvaluated(arg, context);
    return std::make_shared<Bool>(IsType<Pair>(arg) && !AsType<Pair>(arg)->Empty());
}

// -----------------------------------------------------------
// NullPred
std::shared_ptr<Type> NullPred::Apply(std::shared_ptr<Type> arg, Context* context) {
    arg = Helper::GetOneEvaluated(arg, context);
    return std::make_shared<Bool>(IsType<Pair>(arg) && AsType<Pair>(arg)->Empty());
}

// -----------------------------------------------------------
// ListPred
std::shared_ptr<Type> ListPred::Apply(std::shared_ptr<Type> arg, Context* context) {
    arg = Helper::GetOneEvaluated(arg, context);
    return std::make_shared<Bool>(IsType<Pair>(arg) && AsType<Pair>(arg)->ProperList());
}

// -----------------------------------------------------------
// Cons
std::shared_ptr<Type> Cons::Apply(std::shared_ptr<Type> arg, Context* context) {
    auto args = Helper::GetAll(arg);
    if (args.size() != 2) {
        throw RuntimeError("Cons take 2 arguments");
    }
    return std::make_shared<Pair>(args[0]->Evaluate(context), args[1]->Evaluate(context));
}

// -----------------------------------------------------------
// Car
std::shared_ptr<Type> Car::Apply(std::shared_ptr<Type> arg, Context* context) {
    arg = Helper::GetOneEvaluated(arg, context);
    return AsType<Pair>(arg)->GetFirst()->Evaluate(context);
}

// -----------------------------------------------------------
// Cdr
std::shared_ptr<Type> Cdr::Apply(std::shared_ptr<Type> arg, Context* context) {
    arg = Helper::GetOneEvaluated(arg, context);
    return AsType<Pair>(arg)->GetSecond();
}

// -----------------------------------------------------------
// List
std::shared_ptr<Type> List::Apply(std::shared_ptr<Type> arg, Context* context) {
    auto args = Helper::GetAll(arg);
    if (args.empty()) {
        return Pair::EmptyPair();
    }
    auto ret_val = std::make_shared<Pair>(args[0]->Evaluate(context), Pair::EmptyPair());
    auto curr = ret_val;
    for (size_t i = 1; i < args.size(); ++i) {
        curr->SetSecond(std::make_shared<Pair>(args[i]->Evaluate(context), Pair::EmptyPair()));
        curr = AsType<Pair>(curr->GetSecond());
    }
    return ret_val;
}

// -----------------------------------------------------------
// ListRef
std::shared_ptr<Type> ListRef::Apply(std::shared_ptr<Type> arg, Context* context) {
    auto args = Helper::GetAll(arg);
    if (args.size() != 2) {
        throw RuntimeError("ListRef take 2 arguments");
    }
    auto list = AsType<Pair>(args[0]->Evaluate(context));
    size_t index = AsType<Integer>(args[1]->Evaluate(context))->GetValue();
    for (size_t i = 0; i < index; ++i) {
        list = AsType<Pair>(list->GetSecond());
    }
    return list->GetFirst();
}

// -----------------------------------------------------------
// ListTail
std::shared_ptr<Type> ListTail::Apply(std::shared_ptr<Type> arg, Context* context) {
    auto args = Helper::GetAll(arg);
    if (args.size() != 2) {
        throw RuntimeError("ListTail take 2 arguments");
    }
    auto list = AsType<Pair>(args[0]->Evaluate(context));
    size_t index = AsType<Integer>(args[1]->Evaluate(context))->GetValue();
    for (size_t i = 0; i < index; ++i) {
        list = AsType<Pair>(list->GetSecond());
    }
    return list;
}

// -----------------------------------------------------------
// SymbolPred
std::shared_ptr<Type> SymbolPred::Apply(std::shared_ptr<Type> arg, Context* context) {
    arg = Helper::GetOneEvaluated(arg, context);
    return std::make_shared<Bool>(IsType<UnknownSymbol>(arg));
}

// -----------------------------------------------------------
// Define
std::shared_ptr<Type> Define::Apply(std::shared_ptr<Type> arg, Context* context) {
    if (!IsType<Pair>(arg)) {
        throw SyntaxError("Define got not Pair");
    }
    auto pair = AsType<Pair>(arg);
    if (pair->Empty()) {
        throw SyntaxError("Empty define");
    }
    if (IsType<UnknownSymbol>(pair->GetFirst())) {
        // def a value
        Helper::CheckPair(pair);
        auto symbol = AsType<UnknownSymbol>(pair->GetFirst());
        auto value = Helper::GetOneEvaluated(pair->GetSecond(), context);
        return context->Add(symbol->Repr(), value);
    } else if (IsType<Pair>(pair->GetFirst())) {
        // def (fn args) body
        auto func_and_args = AsType<Pair>(pair->GetFirst());
        auto func = AsType<UnknownSymbol>(func_and_args->GetFirst());
        auto lambda_args = func_and_args->GetSecond();
        auto body = pair->GetSecond();
        auto lambda_create_arg = std::make_shared<Pair>(lambda_args, body);
        return context->Add(func->Repr(), LambdaCreate().Apply(lambda_create_arg, context));
    } else {
        throw SyntaxError("Define got Pair with first not Pair");
    }
}

// -----------------------------------------------------------
// Set
std::shared_ptr<Type> Set::Apply(std::shared_ptr<Type> arg, Context* context) {
    Helper::CheckPair(arg);
    auto pair = AsType<Pair>(arg);
    auto symbol = AsType<UnknownSymbol>(pair->GetFirst());
    auto value = Helper::GetOneEvaluated(pair->GetSecond(), context);
    context->Get(symbol->Repr()) = value;
    return value;
}

// -----------------------------------------------------------
// If
std::shared_ptr<Type> If::Apply(std::shared_ptr<Type> arg, Context* context) {
    std::vector<std::shared_ptr<Type>> args;
    try {
        args = Helper::GetAll(arg);
    } catch (RuntimeError) {
        throw SyntaxError("Set::GeAll()");
    }
    if (args.size() != 2 && args.size() != 3) {
        throw SyntaxError("Invalid number of args in If");
    }
    bool condition = Helper::ConvertToBool(args[0]->Evaluate(context));
    if (condition) {
        return args[1]->Evaluate(context);
    } else {
        if (args.size() == 2) {
            return Pair::EmptyPair();
        }
        return args[2]->Evaluate(context);
    }
}

// -----------------------------------------------------------
// SetCar
std::shared_ptr<Type> SetCar::Apply(std::shared_ptr<Type> arg, Context* context) {
    Helper::CheckPair(arg);
    auto args = Helper::GetAll(arg);
    std::string name = AsType<UnknownSymbol>(args[0])->Repr();
    auto val = args[1]->Evaluate(context);
    AsType<Pair>(context->Get(name))->SetFirst(val);
    return val;
}

// -----------------------------------------------------------
// SetCdr
std::shared_ptr<Type> SetCdr::Apply(std::shared_ptr<Type> arg, Context* context) {
    Helper::CheckPair(arg);
    auto args = Helper::GetAll(arg);
    std::string name = AsType<UnknownSymbol>(args[0])->Repr();
    auto val = args[1]->Evaluate(context);
    AsType<Pair>(context->Get(name))->SetSecond(val);
    return val;
}

// -----------------------------------------------------------
// Lambda
std::shared_ptr<Type> Lambda::Apply(std::shared_ptr<Type> arg, Context* context) {
    auto passed_args = Helper::GetAll(arg);
    if (passed_args.size() != args.size()) {
        throw RuntimeError("Invalid number of args in Lambda");
    }
    Context* eval_context = context->collector->Allocate(created_context);
    for (size_t i = 0; i < args.size(); ++i) {
        eval_context->Add(args[i], passed_args[i]->Evaluate(context));
    }
    auto body_lines = Helper::GetAll(body);
    if (body_lines.empty()) {
        throw RuntimeError("Empty Lambda body");
    }
    for (size_t i = 0; i < body_lines.size() - 1; ++i) {
        body_lines[i]->Evaluate(eval_context);
    }
    return body_lines[body_lines.size() - 1]->Evaluate(eval_context);
}

// -----------------------------------------------------------
// LambdaCreate
std::shared_ptr<Type> LambdaCreate::Apply(std::shared_ptr<Type> arg, Context* context) {
    try {
        auto pair = AsType<Pair>(arg);
        auto lambda_args = Helper::GetAll(pair->GetFirst());
        auto body = pair->GetSecond();
        std::vector<std::string> args_name(lambda_args.size());
        for (size_t i = 0; i < lambda_args.size(); ++i) {
            args_name[i] = AsType<UnknownSymbol>(lambda_args[i])->Repr();
        }
        return std::make_shared<Lambda>(args_name, AsType<Pair>(body), context);
    } catch (RuntimeError) {
        throw SyntaxError("LambdaCreate::Apply()");
    }
}

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
