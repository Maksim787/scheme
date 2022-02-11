#include <memory>
#include <string>
#include <sstream>

#include "scheme.h"
#include "object.h"
#include "tokenizer.h"
#include "parser.h"
#include "types.h"

std::shared_ptr<Type> Interpreter::ParseTypes(std::shared_ptr<Object> obj) {
    if (!obj) {
        return Pair::EmptyPair();
    }
    if (Is<Number>(obj)) {
        return std::make_shared<Integer>(As<Number>(obj)->GetValue());
    }
    if (Is<Symbol>(obj)) {
        auto sym = As<Symbol>(obj);
        std::string name = sym->GetName();
        if (name == "#t" || name == "#f") {
            return std::make_shared<Bool>(name);
        }
        if (func_factory_.HasFunction(name)) {
            return func_factory_.GetFunction(name);
        } else {
            return std::make_shared<UnknownSymbol>(As<Symbol>(obj)->GetName());
        }
    }
    if (Is<Cell>(obj)) {
        auto cell = As<Cell>(obj);
        std::shared_ptr<Type> first = ParseTypes(cell->GetFirst());
        std::shared_ptr<Type> second = ParseTypes(cell->GetSecond());
        return std::make_shared<Pair>(first, second);
    }
    throw RuntimeError("You can not be here");
}

std::string Interpreter::Run(std::string str) {
    if (str.empty()) {
        return "";
    }
    std::stringstream ss(std::move(str));
    Tokenizer tokenizer(&ss);
    std::shared_ptr<Object> tree = Read(&tokenizer);
    if (!tokenizer.IsEnd()) {
        throw SyntaxError("!!tokenizer.IsEnd() in Run()");
    }
    std::string evaluated = Evaluate(tree);
    collector_.Clear();
    return evaluated;
}

std::string Interpreter::Evaluate(std::shared_ptr<Object> tree) {
    std::shared_ptr<Type> type = ParseTypes(tree);
    return type->Evaluate(collector_.GetRoot())->Repr();
}
