#pragma once

#include "../defines.h"
#include "ast.h"
#include "../lexer/lexer.h"

typedef struct Parser {
    Lexer* lexer;
    Token* current_token;
    bool getting_params;
} Parser;

Parser parser_init(Lexer* lexer);
enum STATUS parse(Parser* parser, AstNode** out, char** err);