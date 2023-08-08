#include "lexer.h"

static void skip_whitespace(Lexer* lexer);
static void skip_until(Lexer* lexer, char c);
static Result skip_comments(Lexer* lexer, TokenType_t comment_type);
static void advance(Lexer* lexer);
static bool can_advance(Lexer* lexer);
static bool can_collect_string(Lexer* lexer, bool single_quote);

static Result collect_string(Lexer* lexer, Token* out);
static Result collect_number(Lexer* lexer, Token* out);
static Result collect_symbol(Lexer* lexer, Token* out);
static Result collect_id    (Lexer* lexer, Token* out);

static Token create_token(Lexer* lexer, TokenType_t type) {
    return (Token){
        .type=type, 
        .value=NULL,
        .length=0,
        .keyword_type=KEYWORD_UNKNOWN,
        .row=lexer->current_line,
        .column=lexer->current_column,
    };
}

Lexer lexer_init(const char* src, u32 length) {
    assert(src && length != 0 && "One of the parameters is illegal");
    
    return (Lexer) {
        .src=src,
        .length=length,
        .current_char=src[0],
        .current_line=1,
        .current_column=1,
    };
}

Result lexer_get_next_token(Lexer* lexer, Token* out) {
    assert(out && "Out parameter is NULL");

    // Validating first character
    if (lexer->current_char == '\0') {
        *out = create_token(lexer, TOKEN_EOF);
        return result_ok();
    }

    skip_whitespace(lexer);

    // Start of a string
    if (lexer->current_char == '"' || lexer->current_char == '\'')
        return collect_string(lexer, out);

    // Start of a number TODO: add support for numbers like ".123"
    if (isdigit(lexer->current_char))
        return collect_number(lexer, out);

    // Start of a symbol
    if (is_symbol(lexer->current_char)) {
        Result res = collect_symbol(lexer, out);
        
        if (res.is_err) return res;
        TokenType_t comment_type = out->type;
        if (comment_type == TOKEN_LINE_COMMENT || comment_type == TOKEN_MULTILINE_COMMENT_START) {
            *out = create_token(lexer, TOKEN_NL);
            return skip_comments(lexer, comment_type);
        }

        return res;
    }
    
    // If it's not any space 
    if (!isspace(lexer->current_char))
        return collect_id(lexer, out);

    // If it's a newline
    if (lexer->current_char == '\n') {
        // Creating before so that the number line doesnt go up
        *out = create_token(lexer, TOKEN_NL);
        advance(lexer);
        return (Result) {0};
    }

    ERROR_ARGS("Lexer", "Unexpected token at %d:%d\n", lexer->current_line, lexer->current_column);
    return result_error("Unexpected token");
}

void skip_whitespace(Lexer* lexer) {
    while ( lexer->current_char == ' ' || 
            lexer->current_char == '\r' || 
            lexer->current_char == '\t') {
        advance(lexer);
    }
}

void skip_until(Lexer* lexer, char c) {
    while (can_advance(lexer) && lexer->current_char != c) {
        advance(lexer);
    }
}

Result skip_comments(Lexer* lexer, TokenType_t comment_type) {
    switch (comment_type) {
        case TOKEN_LINE_COMMENT:
            skip_until(lexer, '\n');
            advance(lexer);
            break;

        case TOKEN_MULTILINE_COMMENT_START:
            Token tok = {0};
            do {
                unwrap(lexer_get_next_token(lexer, &tok));
                token_free(&tok);
            } while (tok.type != TOKEN_MULTILINE_COMMENT_END && tok.type != TOKEN_EOF);
            
            if (tok.type == TOKEN_EOF) return result_error("Multiline comment was not closed.");
            break;

        default: result_error("Invalid comment_type");
    }
    return result_ok();
}

void advance(Lexer* lexer) {
    if (can_advance(lexer)) {
        // If we're advancing on a new line, increment line number and set column to 0
        if (lexer->current_char == '\n') {
            ++lexer->current_line;
            lexer->current_column = 1;
        } else {
            // Else increment coloum
            ++lexer->current_column;
        }
        
        // Incrementing index and setting new current char
        lexer->current_char = lexer->src[++lexer->current_index];
        return;
    }

    lexer->current_char = '\0';
}

bool can_advance(Lexer* lexer) {
    return lexer->current_index < lexer->length && lexer->current_char != '\0';
}

bool can_collect_string(Lexer* lexer, bool single_quote) {
    return can_advance(lexer) && (
        // While can advance and it's not a quote that matches whatever we started with
        (lexer->current_char != '\'' && single_quote) ||
        (lexer->current_char != '"'  && !single_quote)
    );
}

Result collect_string(Lexer* lexer, Token* out) {
    assert(out && "Out parameter is NULL");

    char* start = NULL;
    u32 row = 0, column = 0, length = 0;
    char buf[2048] = {0};

    bool single_quote = lexer->current_char == '\'';
    start = (char*)lexer->src + lexer->current_index;

    // Eat quote
    advance(lexer);

    // Setting the start of the token
    row = lexer->current_line;
    column = lexer->current_column - 1; // bc of quote

    // Collecting the string
    while (can_collect_string(lexer, single_quote)) {
        buf[length] = lexer->current_char;

        if (lexer->current_char == '\\') {
            advance(lexer); // Eating '\'
            if (!can_collect_string(lexer, single_quote)) {
                printf("%*s\n%*c\n", length, start, length + 2, '^');

                ERROR_ARGS("Syntax", "Unterminated string literal (at line: %d)", row);
                return result_error("Unterminated string literal");
            }

            switch (lexer->current_char) {
                case 'n': 
                    buf[length] = '\n';
                    break;
                default: 
                    printf("%*s\n%*c\n", length, start, length + 2, '^');
                    ERROR_ARGS("Syntax", "Escape character %c hasn't been implemented yet.", lexer->current_char);
                    return result_error("Unimplemented escape character");
            }
        }

        ++length;
        advance(lexer);
    }

    // Checking end of string
    if ((lexer->current_char != '\'' && single_quote) || 
        (lexer->current_char != '"' && !single_quote)) {
        
        ERROR_ARGS("Lexer", "Unexpected end of string at %d:%d\n", lexer->current_line, lexer->current_column);
        return result_error("Unexpected end of string");
    }

    // Eating other quote
    advance(lexer);

    char* token_value = kod_calloc((length + 1), sizeof(char));
    if (!token_value) {
        return result_error("Coudln't allocate for token value");
    }

    memcpy(token_value, buf, length);

    *out = (Token){
        .type=TOKEN_STRING,
        .value=token_value,
        .length=length,
        .keyword_type=KEYWORD_UNKNOWN,
        .row=row,
        .column=column,
    };
    return result_ok();
}

Result collect_number(Lexer* lexer, Token* out) {
    assert(out && "Out parameter is NULL");

    u32 row = 0, column = 0, length = 0;
    char buf[2048] = {0};

    // To know if it's a float or an integer
    bool dot = false;

    // Setting the start of the token
    row = lexer->current_line;
    column = lexer->current_column;

    // Collecting the number
    while (can_advance(lexer) && (
        // While can advance and it's a digit or its a dot and we still haven't encountered a dot
        isdigit(lexer->current_char) || (lexer->current_char == '.' && !dot)
    )) {
        if (lexer->current_char == '.') dot = true;
        buf[length] = lexer->current_char;
        ++length;
        advance(lexer);
    }

    char* token_value = kod_calloc((length + 1), sizeof(char));
    if (!token_value) {
        return result_error("Coudln't allocate for token value");
    }

    memcpy(token_value, buf, length);

    *out = (Token){
        .type=(dot ? TOKEN_FLOAT : TOKEN_INT),
        .value=token_value,
        .length=length,
        .keyword_type=KEYWORD_UNKNOWN,
        .row=row,
        .column=column,
    };
    return result_ok();
}

Result collect_symbol(Lexer* lexer, Token* out) {
    assert(out && "Out parameter is NULL");

    u32 row = 0, column = 0, length = 0;
    char buf[2] = {0};

    // Setting the start of the token
    buf[length] = lexer->current_char;
    row = lexer->current_line;
    column = lexer->current_column;

    // We got the first character of the symbol, advance next (max symbol length == 2)
    advance(lexer);
    ++length;
    
    TokenType_t type = find_symbol(buf, length);

    // If the next character is also a symbol
    if (can_advance(lexer) && is_symbol(lexer->current_char)) {
        TokenType_t second_type = find_symbol((char[]){buf[0], lexer->current_char, 0}, length + 1);
        if (second_type != TOKEN_UNKNOWN) {
            buf[length] = lexer->current_char;
            ++length;
            advance(lexer);
            type = second_type;
        }
    }

    *out = (Token){
        .type=type,
        .value=NULL,
        .length=0,
        .keyword_type=KEYWORD_UNKNOWN,
        .row=row,
        .column=column,
    };
    return result_ok();
}

Result collect_id(Lexer* lexer, Token* out) {
    assert(out && "Out parameter is NULL");

    u32 row = 0, column = 0, length = 0;
    char buf[2048] = {0};

    // Setting the start of the token
    row = lexer->current_line;
    column = lexer->current_column;

    // Collecting the id
    while (can_advance(lexer) && !isspace(lexer->current_char) && !is_symbol(lexer->current_char)) {
        buf[length] = lexer->current_char;
        ++length;
        advance(lexer);
    }

    TokenType_t type = TOKEN_ID;
    KeywordType keyword_type = KEYWORD_UNKNOWN;

    char* token_value = kod_calloc((length + 1), sizeof(char));
    if (!token_value) {
        return result_error("Coudln't allocate for token value");
    }

    memcpy(token_value, buf, length);

    if ((keyword_type = find_keyword(token_value)) != KEYWORD_UNKNOWN) {
        type = TOKEN_KEYWORD;
    }

    *out = (Token){
        .type=type,
        .value=token_value,
        .length=length,
        .keyword_type=keyword_type,
        .row=row,
        .column=column,
    };
    return (Result) {0};
}

