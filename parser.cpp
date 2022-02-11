#include <memory>

#include "object.h"
#include "parser.h"
#include "error.h"

std::shared_ptr<Object> Read(Tokenizer* tokenizer, bool is_first) {
    Token token = tokenizer->GetToken();
    tokenizer->Next();
    std::shared_ptr<Object> ret_val;
    if (SymbolToken* symbol = std::get_if<SymbolToken>(&token)) {
        ret_val = std::make_shared<Symbol>(symbol->name);
    } else if (std::get_if<QuoteToken>(&token)) {
        ret_val = std::make_shared<Cell>(std::make_shared<Symbol>("'"), Read(tokenizer, false));
    } else if (std::get_if<QuoteTokenWord>(&token)) {
        ret_val = std::make_shared<Symbol>("'");
    } else if (std::get_if<DotToken>(&token)) {
        ret_val = std::make_shared<Symbol>(".");
    } else if (BracketToken* bracket = std::get_if<BracketToken>(&token)) {
        if (*bracket == BracketToken::OPEN) {
            ret_val = ReadList(tokenizer);
        } else if (*bracket == BracketToken::CLOSE) {
            ret_val = std::make_shared<Symbol>(")");
        }
    } else if (ConstantToken* constant = std::get_if<ConstantToken>(&token)) {
        ret_val = std::make_shared<Number>(constant->value);
    }
    // Ошибка
    if (is_first && !tokenizer->IsEnd()) {
        throw SyntaxError("is_first = true, but !tokenizer->IsEnd()");
    }
    return ret_val;
}

bool SymbolEqual(std::shared_ptr<Object> object_ptr, const std::string& str) {
    return Is<Symbol>(object_ptr) && As<Symbol>(object_ptr)->GetName() == str;
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    auto object_ptr = Read(tokenizer, false);
    if (SymbolEqual(object_ptr, ")")) {
        return nullptr;
    }
    if (SymbolEqual(object_ptr, ".")) {
        throw SyntaxError("SymbolEqual(object_ptr, .)");
    }
    // (quote
    if (SymbolEqual(object_ptr, "'")) {
        auto ret_val = std::make_shared<Cell>(object_ptr, Read(tokenizer, false));
        tokenizer->Next();
        return ret_val;
    }
    auto ret_cell = std::make_shared<Cell>(object_ptr, nullptr);
    auto last_cell = ret_cell;
    bool last_dot = false;
    while (true) {
        object_ptr = Read(tokenizer, false);
        if (!last_dot && SymbolEqual(object_ptr, ")")) {
            return ret_cell;
        }
        if (SymbolEqual(object_ptr, ".")) {
            last_dot = true;
            continue;
        }
        if (Is<Cell>(object_ptr) && !last_dot) {
            last_cell->SetSecond(std::make_shared<Cell>(object_ptr, nullptr));
            last_cell = As<Cell>(last_cell->GetSecond());
        } else if (last_dot) {
            last_cell->SetSecond(object_ptr);
            if (SymbolEqual(object_ptr, ")") || SymbolEqual(object_ptr, "(") ||
                SymbolEqual(object_ptr, ".")) {
                throw SyntaxError("Readlist() ')' or '(' or '.' in pair");
            }
            object_ptr = Read(tokenizer, false);
            if (!SymbolEqual(object_ptr, ")")) {
                throw SyntaxError("Readlist() expected ')' after pair");
            }
            return ret_cell;
        } else {
            last_cell->SetSecond(std::make_shared<Cell>(object_ptr, nullptr));
            last_cell = As<Cell>(last_cell->GetSecond());
        }
        last_dot = false;
    }
}
