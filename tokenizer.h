#pragma once

#include <iostream>
#include <variant>
#include <optional>
#include <istream>
#include <cctype>

#include "error.h"

struct SymbolToken {
    std::string name;

    bool operator==(const SymbolToken& other) const {
        return name == other.name;
    }
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const {
        return true;
    }
};

struct QuoteTokenWord {
    bool operator==(const QuoteTokenWord&) const {
        return true;
    }
};

struct DotToken {
    bool operator==(const DotToken&) const {
        return true;
    }
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int value;

    bool operator==(const ConstantToken& other) const {
        return value == other.value;
    }
};

using Token =
    std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, QuoteTokenWord, DotToken>;

class Tokenizer {
    std::istream* in_;

    Token token_;
    bool has_token_ = false;
    bool is_end_ = false;

    bool StartSymbol(char c) {
        //        a-zA-Z<=>*#
        return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '<' || c == '=' ||
               c == '/' || c == '>' || c == '*' || c == '#';
    }

    bool IsSymbol(char c) {
        //        a-zA-Z<=>*#0-9?!-
        return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '<' || c == '=' ||
               c == '>' || c == '*' || c == '#' || ('0' <= c && c <= '9') || c == '?' || c == '!' ||
               c == '-';
    }

    bool IsDigit(char c) {
        return '0' <= c && c <= '9';
    }

    int ParseNumber(int start_number = 0) {
        int number = start_number;
        while (IsDigit(in_->peek())) {
            number *= 10;
            number += in_->get() - '0';
        }
        return number;
    }

    std::string ParseSymbol() {
        std::string result;
        while (IsSymbol(in_->peek())) {
            result.push_back(in_->get());
        }
        return result;
    }

public:
    Tokenizer(std::istream* in) : in_(in) {
        GetToken();
    }

    bool IsEnd() {
        return is_end_;
    }

    void Next() {
        if (IsEnd()) {
            throw SyntaxError("tokenizer Next() IsEnd()");
        }
        while (in_->peek() == ' ' || in_->peek() == '\n') {
            in_->get();
        }
        if (in_->peek() == EOF) {
            is_end_ = true;
            return;
        }
        char c = in_->get();
        if (c == '(') {
            token_ = BracketToken::OPEN;
        } else if (c == ')') {
            token_ = BracketToken::CLOSE;
        } else if (c == '\'') {
            token_ = QuoteToken{};
        } else if (c == '.') {
            token_ = DotToken{};
        } else if (c == '-' || c == '+') {
            if (IsDigit(in_->peek())) {
                token_ = ConstantToken{ParseNumber() * (c == '-' ? -1 : 1)};
            } else {
                token_ = SymbolToken{std::string(1, c)};
            }
        } else if (IsDigit(c)) {
            token_ = ConstantToken{ParseNumber(c - '0')};
        } else if (StartSymbol(c)) {
            std::string symbol = c + ParseSymbol();
            if (symbol == "quote") {
                token_ = QuoteTokenWord{};
            } else {
                token_ = SymbolToken{symbol};
            }
        } else {
            throw SyntaxError("tokenizer Next()");
        }
        has_token_ = true;
    }

    Token GetToken() {
        if (!has_token_) {
            Next();
        }
        return token_;
    }
};
