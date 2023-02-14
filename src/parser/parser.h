#pragma once

#include "ast.h"
#include "../lexer/lexer.h"

typedef struct parser_t {
    lexer_t* lexer;
    token_t* current_token;
} parser_t;

parser_t parser_init(lexer_t* lexer);
ast_node_t* parse(parser_t* parser);