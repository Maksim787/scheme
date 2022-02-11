#pragma once

#include <memory>

#include "types.h"
#include "error.h"

struct Quote : public Function {
    std::string Repr() override {
        return "'";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

struct BooleanPred : public Function {

    std::string Repr() override {
        return "boolean?";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

struct Not : public Function {
    std::string Repr() override {
        return "not";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

struct And : public Function {
    std::string Repr() override {
        return "and";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

struct Or : public Function {
    std::string Repr() override {
        return "or";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

struct NumberPred : public Function {
    std::string Repr() override {
        return "number?";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

struct IntegerOperation : public Function {
    virtual std::shared_ptr<Integer> Operation(std::shared_ptr<Integer> one,
                                               std::shared_ptr<Integer> two) = 0;

    virtual std::shared_ptr<Integer> FirstElem() = 0;

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

struct Addition : public IntegerOperation {
    std::string Repr() override {
        return "+";
    }

    std::shared_ptr<Integer> Operation(std::shared_ptr<Integer> one,
                                       std::shared_ptr<Integer> two) override {
        return std::make_shared<Integer>(one->GetValue() + two->GetValue());
    }

    std::shared_ptr<Integer> FirstElem() override {
        return std::make_shared<Integer>(0);
    }
};

struct Subtraction : public IntegerOperation {
    std::string Repr() override {
        return "-";
    }

    std::shared_ptr<Integer> Operation(std::shared_ptr<Integer> one,
                                       std::shared_ptr<Integer> two) override {
        return std::make_shared<Integer>(one->GetValue() - two->GetValue());
    }

    std::shared_ptr<Integer> FirstElem() override {
        return std::make_shared<Integer>(-1);
    }
};

struct Multiplication : public IntegerOperation {
    std::string Repr() override {
        return "*";
    }

    std::shared_ptr<Integer> Operation(std::shared_ptr<Integer> one,
                                       std::shared_ptr<Integer> two) override {
        return std::make_shared<Integer>(one->GetValue() * two->GetValue());
    }

    std::shared_ptr<Integer> FirstElem() override {
        return std::make_shared<Integer>(1);
    }
};

struct Division : public IntegerOperation {
    std::string Repr() override {
        return "/";
    }

    std::shared_ptr<Integer> Operation(std::shared_ptr<Integer> one,
                                       std::shared_ptr<Integer> two) override {
        if (AsType<Integer>(two)->GetValue() == 0) {
            throw RuntimeError("Division by zero");
        }
        return std::make_shared<Integer>(AsType<Integer>(one)->GetValue() /
                                         AsType<Integer>(two)->GetValue());
    }

    std::shared_ptr<Integer> FirstElem() override {
        return std::make_shared<Integer>(-1);
    }
};

struct Compare : public Function {
    virtual bool Comparator(std::shared_ptr<Integer>, std::shared_ptr<Integer>) = 0;

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

struct Equal : public Compare {
    std::string Repr() override {
        return "=";
    }

    bool Comparator(std::shared_ptr<Integer> one, std::shared_ptr<Integer> two) override {
        return one->GetValue() == two->GetValue();
    }
};

struct Less : public Compare {
    std::string Repr() override {
        return "<";
    }

    bool Comparator(std::shared_ptr<Integer> one, std::shared_ptr<Integer> two) override {
        return one->GetValue() < two->GetValue();
    }
};

struct Greater : public Compare {
    std::string Repr() override {
        return ">";
    }

    bool Comparator(std::shared_ptr<Integer> one, std::shared_ptr<Integer> two) override {
        return one->GetValue() > two->GetValue();
    }
};

struct LessEqual : public Compare {
    std::string Repr() override {
        return "<=";
    }

    bool Comparator(std::shared_ptr<Integer> one, std::shared_ptr<Integer> two) override {
        return one->GetValue() <= two->GetValue();
    }
};

struct GreaterEqual : public Compare {
    std::string Repr() override {
        return ">=";
    }

    bool Comparator(std::shared_ptr<Integer> one, std::shared_ptr<Integer> two) override {
        return one->GetValue() >= two->GetValue();
    }
};

struct MinMax : public Function {
    virtual std::shared_ptr<Integer> Operation(std::shared_ptr<Integer> one,
                                               std::shared_ptr<Integer> two) = 0;

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

struct Min : public MinMax {
    std::string Repr() override {
        return "min";
    }

    std::shared_ptr<Integer> Operation(std::shared_ptr<Integer> one,
                                       std::shared_ptr<Integer> two) override {
        return std::make_shared<Integer>(std::min(one->GetValue(), two->GetValue()));
    }
};

struct Max : public MinMax {
    std::string Repr() override {
        return "max";
    }

    std::shared_ptr<Integer> Operation(std::shared_ptr<Integer> one,
                                       std::shared_ptr<Integer> two) override {
        return std::make_shared<Integer>(std::max(one->GetValue(), two->GetValue()));
    }
};

struct Abs : public Function {
    std::string Repr() override {
        return "abs";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

struct PairPred : public Function {
    std::string Repr() override {
        return "pair?";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

struct NullPred : public Function {
    std::string Repr() override {
        return "null?";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

struct ListPred : public Function {
    std::string Repr() override {
        return "list?";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

// сделать пару из списка
struct Cons : public Function {
    std::string Repr() override {
        return "cons";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

// первый элемент пары
struct Car : public Function {
    std::string Repr() override {
        return "car";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

// второй элемент пары
struct Cdr : public Function {
    std::string Repr() override {
        return "cdr";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

// сделать лист из следующих элементов
struct List : public Function {
    std::string Repr() override {
        return "list";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

// взять i-ый элемент по индексу
struct ListRef : public Function {
    std::string Repr() override {
        return "list-ref";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

// убрать первые n элементов из листа
struct ListTail : public Function {
    std::string Repr() override {
        return "list-tail";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

struct SymbolPred : public Function {
    std::string Repr() override {
        return "symbol?";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

struct Define : public Function {
    std::string Repr() override {
        return "define";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

struct Set : public Function {
    std::string Repr() override {
        return "set!";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

struct If : public Function {
    std::string Repr() override {
        return "if";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

struct SetCar : public Function {
    std::string Repr() override {
        return "set-car!";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

struct SetCdr : public Function {
    std::string Repr() override {
        return "set-cdr!";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

struct Lambda : public Function {
    std::vector<std::string> args;
    std::shared_ptr<Type> body;
    Context* created_context;

    Lambda(std::vector<std::string> args, std::shared_ptr<Pair> body, Context* created_context)
        : args(args), body(body), created_context(created_context) {
        if (body->Empty()) {
            throw SyntaxError("Empty Lambda body");
        }
    }

    std::string Repr() override {
        return "unknown lambda";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

struct LambdaCreate : public Function {
    std::string Repr() override {
        return "lambda";
    }

    std::shared_ptr<Type> Apply(std::shared_ptr<Type> arg, Context* context) override;
};

struct Helper {
    static bool ConvertToBool(std::shared_ptr<Type> obj) {
        if (IsType<Bool>(obj)) {
            return AsType<Bool>(obj)->GetValue();
        }
        return true;
    }

    static std::shared_ptr<Type> GetOneEvaluated(std::shared_ptr<Type> obj, Context* context) {
        if (IsType<Pair>(obj)) {
            auto next = AsType<Pair>(obj)->GetSecond();
            if (IsType<Pair>(next) && AsType<Pair>(next)->Empty()) {
                return AsType<Pair>(obj)->GetFirst()->Evaluate(context);
            }
            // else error
        }
        return obj->Evaluate(context);
    }

    static std::vector<std::shared_ptr<Type>> GetAll(std::shared_ptr<Type> obj) {
        auto head = AsType<Pair>(obj);
        if (!head->ProperList()) {
            throw RuntimeError("GetAll() got not proper list");
        }
        std::vector<std::shared_ptr<Type>> result;
        while (!head->Empty()) {
            result.push_back(head->GetFirst());
            head = AsType<Pair>(head->GetSecond());
        }
        return result;
    }

    static void CheckPair(std::shared_ptr<Type> obj) {
        std::vector<std::shared_ptr<Type>> args;
        try {
            args = Helper::GetAll(obj);
        } catch (RuntimeError) {
            throw SyntaxError("CheckPair::GeAll()");
        }
        if (args.size() != 2) {
            throw SyntaxError("CheckPair::args.size()");
        }
    }
};
