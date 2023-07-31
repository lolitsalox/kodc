#pragma once

#include "token.h"

typedef struct lexer_t {
    const char* src;
    uint32_t length;

    char current_char;
    uint32_t current_index, current_line, current_column;
} lexer_t;

lexer_t lexer_init(const char* src, uint32_t length);
token_t* lexer_get_next_token(lexer_t* lexer);

int8_t str_compare(char* a, char* b, uint32_t length_a, uint32_t length_b);