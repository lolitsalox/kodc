#pragma once

#include <defines.h>
#include "token.h"

typedef struct Lexer {
    const char* src;
    u32 length;

    char current_char;
    u32 current_index, current_line, current_column;
} Lexer;

Lexer lexer_init(const char* src, u32 length);
enum STATUS lexer_get_next_token(Lexer* lexer, Token** out, char** err);

char* keyword_type_to_str(KeywordType ktype);