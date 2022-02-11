#pragma once

#include <memory>

#include "object.h"
#include "tokenizer.h"

std::shared_ptr<Object> Read(Tokenizer* tokenizer, bool is_first = true);

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer);
