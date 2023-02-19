#include "lexer.h"

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#define MIN(a, b) (a < b ? a : a > b ? b : a)

static void skip_whitespace(lexer_t* lexer);
static void advance(lexer_t* lexer);
static bool can_advance(lexer_t* lexer);
static bool is_symbol(char c);
static token_type_t find_symbol(char* s, uint32_t length);
static keyword_type_t find_keyword(char* s, uint32_t length);

static token_t* collect_string(lexer_t* lexer);
static token_t* collect_number(lexer_t* lexer);
static token_t* collect_symbol(lexer_t* lexer);
static token_t* collect_id(lexer_t* lexer);

static token_t* create_token(lexer_t* lexer, token_type_t type) {
    return token_new(
            (token_t){
                .token_type=type, 
                .value=NULL,
                .length=0,
                .keyword_type=KEYWORD_UNKNOWN,
                .row=lexer->current_line,
                .column=lexer->current_column,
            }
    );
}

lexer_t lexer_init(const char* src, uint32_t length) {
    if (!src || length == 0) {
        printf("[lexer]: Warning - src is empty or length is 0\n");
        return (lexer_t) {0};
    }

    lexer_t lexer = { 0 };
    lexer.src = src;
    lexer.length = length;
    lexer.current_char = src[0];
    lexer.current_line = 1;
    lexer.current_column = 1;
    return lexer;
}

token_t* lexer_get_next_token(lexer_t* lexer) {
    // Validating first character
    if (lexer->current_char == '\0')
        return create_token(lexer, TOKEN_EOF);

    skip_whitespace(lexer);

    // Start of a string
    if (lexer->current_char == '"' || lexer->current_char == '\'')
        return collect_string(lexer);

    // Start of a number TODO: add support for numbers like ".123"
    if (isdigit(lexer->current_char))
        return collect_number(lexer);

    // Start of a symbol
    if (is_symbol(lexer->current_char))
        return collect_symbol(lexer);

    // If it's not any space 
    if (!isspace(lexer->current_char))
        return collect_id(lexer);

    // If it's a newline
    if (lexer->current_char == '\n') {
        token_t* tok = create_token(lexer, TOKEN_NL);
        advance(lexer);
        return tok;
    }

    printf("[lexer]: Error - unexpected token at %d:%d\n", lexer->current_line, lexer->current_column);
    return NULL;
}

static void skip_whitespace(lexer_t* lexer) {
    while (lexer->current_char == ' ' || lexer->current_char == '\r') advance(lexer);
}

static void advance(lexer_t* lexer) {
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

static bool can_advance(lexer_t* lexer) {
    return lexer->current_index < lexer->length && lexer->current_char != '\0';
}

static bool is_symbol(char c) {
    switch (c) {
        case '(':
        case ')':
        case '[':
        case ']':
        case '{':
        case '}':
        case '=':
        case '@':
        case '#':
        case ',':
        case '.':
        case ':':
        case ';':
        case '?':
        case '\\':
        case '+':
        case '-':
        case '/':
        case '*':
        case '%':
        case '&':
        case '|':
        case '^':
        case '<':
        case '>':
        case '~':
        case '!':
            return true;
        default:
            return false;
    }
}

static token_type_t find_symbol(char* s, uint32_t length) {
    if (length == 2) {
        switch (s[0]) {
            case ':': if (s[1] == s[0]) return TOKEN_NAMESPACE; break;
            case '&': if (s[1] == s[0]) return TOKEN_BOOL_AND;  break;
            case '|': if (s[1] == s[0]) return TOKEN_BOOL_OR;   break;

            case '*': 
                if (s[1] == s[0]) return TOKEN_POW;
                else if (s[1] == '/') return TOKEN_MULTILINE_COMMENT_END;
                break;
            
            case '/': 
                if (s[1] == s[0]) return TOKEN_LINE_COMMENT; 
                else if (s[1] == '*') return TOKEN_MULTILINE_COMMENT_START;
                break;

            case '-': 
                if (s[1] == '>') return TOKEN_POINTER; 
                break;

            case '!': 
                if (s[1] == '=') return TOKEN_BOOL_NOTE; 
                break;
            
            case '=': 
                if (s[1] == s[0]) return TOKEN_BOOL_EQ; 
                else if (s[1] == '>') return TOKEN_ARROW; 
                break;
            
            case '<': 
                if (s[1] == s[0]) return TOKEN_SHL; 
                else if (s[1] == '=') return TOKEN_BOOL_LTE; 
                break;
            
            case '>': 
                if (s[1] == s[0]) return TOKEN_SHR; 
                else if (s[1] == '=') return TOKEN_BOOL_GTE; 
                break;

            default: {
                return TOKEN_UNKNOWN;
            }
        }   
        return TOKEN_UNKNOWN;
    }

    switch (s[0]) {
        case '(': return TOKEN_LPAREN;
        case ')': return TOKEN_RPAREN;
        case '[': return TOKEN_LBRACKET;
        case ']': return TOKEN_RBRACKET;
        case '{': return TOKEN_LBRACE;
        case '}': return TOKEN_RBRACE;
        case '=': return TOKEN_EQUALS;
        case ',': return TOKEN_COMMA;
        case '.': return TOKEN_DOT;
        case ':': return TOKEN_COLON;
        case ';': return TOKEN_SEMI;
        case '?': return TOKEN_QUESTION;
        case '%': return TOKEN_MOD;
        case '\\': return TOKEN_BACKSLASH;
        case '#': return TOKEN_HASH;
        case '@': return TOKEN_AT;
        case '+': return TOKEN_ADD;
        case '-': return TOKEN_SUB;
        case '/': return TOKEN_DIV;
        case '*': return TOKEN_MUL;
        case '&': return TOKEN_AND;
        case '|': return TOKEN_OR;
        case '^': return TOKEN_HAT;
        case '<': return TOKEN_BOOL_LT;
        case '>': return TOKEN_BOOL_GT;
        case '~': return TOKEN_NOT;
        case '!': return TOKEN_BOOL_NOT;
        default:
            return TOKEN_UNKNOWN;
    }
}

int8_t str_compare(char* a, char* b, uint32_t length_a, uint32_t length_b) {
    uint32_t length = MIN(length_a, length_b);

    for(uint32_t i = 0; i < length; ++i, ++a, ++b) {
        if((*a) > (*b))
            return -1;

        if((*a) < (*b))
            return 1;
    }

    return 0;
}

static keyword_type_t find_keyword(char* s, uint32_t length) {
    if (str_compare(s, "if",    length, sizeof("if")) == 0)             return KEYWORD_IF;
    if (str_compare(s, "while", length, sizeof("while")) == 0)          return KEYWORD_WHILE;
    if (str_compare(s, "for",   length, sizeof("for")) == 0)            return KEYWORD_FOR;
    if (str_compare(s, "return",   length, sizeof("return")) == 0)      return KEYWORD_RETURN;

    return KEYWORD_UNKNOWN;
}

token_t* collect_string(lexer_t* lexer) {
    char* value = NULL;
    uint32_t row = 0, column = 0, length = 0;

    bool single_quote = lexer->current_char == '\'';

    // Eat quote
    advance(lexer);

    // Setting the start of the token
    value = (char*)lexer->src + lexer->current_index;
    row = lexer->current_line;
    column = lexer->current_column;

    // Collecting the string
    while (can_advance(lexer) && (
        // While can advance and it's not a quote that matches whatever we started with
        (lexer->current_char != '\'' && single_quote) ||
        (lexer->current_char != '"'  && !single_quote)
    )) {
        ++length;
        advance(lexer);
    }

    // Checking end of string
    if ((lexer->current_char != '\'' && single_quote) || 
        (lexer->current_char != '"' && !single_quote)) {
        
        printf(
            "[lexer]: Error - unexpected end of string at %d:%d\n"
            "%s%.*s\n", 
            lexer->current_line, lexer->current_column, (single_quote ? "'" : "\""), length, value
        );
        for (uint32_t i = 0; i < lexer->current_column - 1; ++i) printf(" ");
        printf("^\n");
        return NULL;
    }

    // Eating other quote
    advance(lexer);

    return token_new(
        (token_t){
            .token_type=TOKEN_STRING,
            .value=value,
            .length=length,
            .keyword_type=KEYWORD_UNKNOWN,
            .row=row,
            .column=column,
            }
        );
}

static token_t* collect_number(lexer_t* lexer) {
    char* value = NULL;
    uint32_t row = 0, column = 0, length = 0;

    // To know if it's a float or an integer
    bool dot = false;

    // Setting the start of the token
    value = (char*)lexer->src + lexer->current_index;
    row = lexer->current_line;
    column = lexer->current_column;

    // Collecting the number
    while (can_advance(lexer) && (
        // While can advance and it's a digit or its a dot and we still haven't encountered a dot
        isdigit(lexer->current_char) || (lexer->current_char == '.' && !dot)
    )) {
        if (lexer->current_char == '.') dot = true;
        ++length;
        advance(lexer);
    }
    
    return token_new(
        (token_t){
            .token_type=(dot ? TOKEN_FLOAT : TOKEN_INT),
            .value=value,
            .length=length,
            .keyword_type=KEYWORD_UNKNOWN,
            .row=row,
            .column=column,
            }
        );
}

static token_t* collect_symbol(lexer_t* lexer) {
    char* value = NULL;
    uint32_t row = 0, column = 0, length = 0;

    // Setting the start of the token
    value = (char*)lexer->src + lexer->current_index;
    row = lexer->current_line;
    column = lexer->current_column;

    // We got the first character of the symbol, advance next (max symbol length == 2)
    advance(lexer);
    ++length;

    token_type_t type = find_symbol(value, length);

    // If the next character is also a symbol
    if (can_advance(lexer) && is_symbol(lexer->current_char)) {
        token_type_t second_type = find_symbol(value, length + 1);
        if (second_type != TOKEN_UNKNOWN) {
            ++length;
            advance(lexer);
            type = second_type;
        }
    }
    
    return token_new(
        (token_t){
            .token_type=type,
            .value=value,
            .length=length,
            .keyword_type=KEYWORD_UNKNOWN,
            .row=row,
            .column=column,
            }
        );
}

static token_t* collect_id(lexer_t* lexer) {
    char* value = NULL;
    uint32_t row = 0, column = 0, length = 0;

    // Setting the start of the token
    value = (char*)lexer->src + lexer->current_index;
    row = lexer->current_line;
    column = lexer->current_column;

    // Collecting the id
    while (can_advance(lexer) && !isspace(lexer->current_char) && !is_symbol(lexer->current_char)) {
        ++length;
        advance(lexer);
    }

    token_type_t token_type = TOKEN_ID;
    keyword_type_t keyword_type = KEYWORD_UNKNOWN;

    if ((keyword_type = find_keyword(value, length)) != KEYWORD_UNKNOWN) {
        token_type = TOKEN_KEYWORD;
    }
    
    return token_new(
        (token_t){
            .token_type=token_type,
            .value=value,
            .length=length,
            .keyword_type=keyword_type,
            .row=row,
            .column=column,
            }
        );
}
