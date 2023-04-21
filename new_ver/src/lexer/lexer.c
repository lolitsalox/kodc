#include "lexer.h"

#define MIN(a, b) (a < b ? a : a > b ? b : a)

static void skip_whitespace(Lexer* lexer);
static void advance(Lexer* lexer);
static bool can_advance(Lexer* lexer);
static bool is_symbol(char c);
static TokenType find_symbol(char* s, u32 length);
static KeywordType find_keyword(char* s, u32 length);

static enum STATUS collect_string(Lexer* lexer, Token** out, char** err);
static enum STATUS collect_number(Lexer* lexer, Token** out, char** err);
static enum STATUS collect_symbol(Lexer* lexer, Token** out, char** err);
static enum STATUS collect_id(Lexer* lexer, Token** out, char** err);

static enum STATUS create_token(Lexer* lexer, TokenType type, Token** out, char** err) {
    return token_new(
        (Token){
            .type=type, 
            .value=NULL,
            .length=0,
            .keyword_type=KEYWORD_UNKNOWN,
            .row=lexer->current_line,
            .column=lexer->current_column,
        },
        out,
        err
    );
}

Lexer lexer_init(const char* src, u32 length) {
    if (!src || length == 0) {
        printf("[lexer]: Warning - src is empty or length is 0\n");
        return (Lexer) {0};
    }

    Lexer lexer = { 0 };
    lexer.src = src;
    lexer.length = length;
    lexer.current_char = src[0];
    lexer.current_line = 1;
    lexer.current_column = 1;
    return lexer;
}

enum STATUS lexer_get_next_token(Lexer* lexer, Token** out, char** err) {
    if (!lexer) {
        *err = "Lexer is NULL";
        return STATUS_FAIL;
    }

    // Validating first character
    if (lexer->current_char == '\0')
        return create_token(lexer, TOKEN_EOF, out, err);

    skip_whitespace(lexer);

    // Start of a string
    if (lexer->current_char == '"' || lexer->current_char == '\'')
        return collect_string(lexer, out, err);

    // Start of a number TODO: add support for numbers like ".123"
    if (isdigit(lexer->current_char))
        return collect_number(lexer, out, err);

    // Start of a symbol
    if (is_symbol(lexer->current_char))
        return collect_symbol(lexer, out, err);

    // If it's not any space 
    if (!isspace(lexer->current_char))
        return collect_id(lexer, out, err);

    // If it's a newline
    if (lexer->current_char == '\n') {
        advance(lexer);
        return create_token(lexer, TOKEN_NL, out, err);
    }

    ERROR_ARGS("Lexer", "Unexpected token at %d:%d\n", lexer->current_line, lexer->current_column);
    *err = "Unexpected token";
    return STATUS_FAIL;
}

static void skip_whitespace(Lexer* lexer) {
    if (!lexer) return;
    while (lexer->current_char == ' ' || lexer->current_char == '\r') advance(lexer);
}

static void advance(Lexer* lexer) {
    if (!lexer) return;

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

static bool can_advance(Lexer* lexer) {
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

static TokenType find_symbol(char* s, u32 length) {
    if (length == 2) {
        switch (s[0]) {
            case ':': if (s[1] == s[0]) return TOKEN_NAMESPACE; break;
            case '&': if (s[1] == s[0]) return TOKEN_BOOL_AND;  break;
            case '|': if (s[1] == s[0]) return TOKEN_BOOL_OR;   break;

            case '+': 
                if (s[1] == '=') return TOKEN_ADD_EQ;
                break;

            case '%': 
                if (s[1] == '=') return TOKEN_MOD_EQ;
                break;

            case '*': 
                if (s[1] == s[0]) return TOKEN_POW;
                else if (s[1] == '=') return TOKEN_MUL_EQ;
                else if (s[1] == '/') return TOKEN_MULTILINE_COMMENT_END;
                break;
            
            case '/': 
                if (s[1] == s[0]) return TOKEN_LINE_COMMENT; 
                else if (s[1] == '=') return TOKEN_DIV_EQ;
                else if (s[1] == '*') return TOKEN_MULTILINE_COMMENT_START;
                break;

            case '-': 
                if (s[1] == '>') return TOKEN_POINTER; 
                else if (s[1] == '=') return TOKEN_SUB_EQ;
                break;

            case '!': 
                if (s[1] == '=') return TOKEN_BOOL_NE; 
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

s8 str_compare(char* a, char* b, u32 length_a, u32 length_b) {
    if (length_a < length_b) return 1;
    if (length_a > length_b) return -1;

    for(u32 i = 0; i < length_a; ++i, ++a, ++b) {
        if((*a) > (*b))
            return -1;

        if((*a) < (*b))
            return 1;
    }

    return 0;
}

static KeywordType find_keyword(char* s, u32 length) {
    if (str_compare(s, "null",    length, sizeof("null") - 1) == 0)         return KEYWORD_NULL;
    if (str_compare(s, "true",    length, sizeof("true") - 1) == 0)         return KEYWORD_TRUE;
    if (str_compare(s, "false",    length, sizeof("false") - 1) == 0)       return KEYWORD_FALSE;
    if (str_compare(s, "if",    length, sizeof("if") - 1) == 0)             return KEYWORD_IF;
    if (str_compare(s, "while", length, sizeof("while") - 1) == 0)          return KEYWORD_WHILE;
    if (str_compare(s, "for",   length, sizeof("for") - 1) == 0)            return KEYWORD_FOR;
    if (str_compare(s, "return",   length, sizeof("return") - 1) == 0)      return KEYWORD_RETURN;

    return KEYWORD_UNKNOWN;
}

enum STATUS collect_string(Lexer* lexer, Token** out, char** err) {
    if (!lexer) {
        *err = "Lexer is NULL";
        return STATUS_FAIL;
    }

    char* value = NULL;
    u32 row = 0, column = 0, length = 0;

    bool single_quote = lexer->current_char == '\'';

    // Eat quote
    advance(lexer);

    // Setting the start of the token
    value = (char*)lexer->src + lexer->current_index;
    row = lexer->current_line;
    column = lexer->current_column;

    // TODO: add support for escaped chars

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
        
        ERROR_ARGS("Lexer", "Unexpected end of string at %d:%d\n", lexer->current_line, lexer->current_column);
        *err = "Unexpected end of string";
        return STATUS_FAIL;
    }

    // Eating other quote
    advance(lexer);

    char* token_value = calloc(1, sizeof(char) * (length + 1));
    strncpy(token_value, value, length);

    return token_new(
        (Token){
            .type=TOKEN_STRING,
            .value=token_value,
            .length=length,
            .keyword_type=KEYWORD_UNKNOWN,
            .row=row,
            .column=column,
        },
        out, 
        err
    );
}

static enum STATUS collect_number(Lexer* lexer, Token** out, char** err) {
    if (!lexer) {
        *err = "Lexer is NULL";
        return STATUS_FAIL;
    }

    char* value = NULL;
    u32 row = 0, column = 0, length = 0;

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
    
    char* token_value = calloc(1, sizeof(char) * (length + 1));
    strncpy(token_value, value, length);

    return token_new(
        (Token){
            .type=(dot ? TOKEN_FLOAT : TOKEN_INT),
            .value=token_value,
            .length=length,
            .keyword_type=KEYWORD_UNKNOWN,
            .row=row,
            .column=column,
        },
        out,
        err
    );
}

static enum STATUS collect_symbol(Lexer* lexer, Token** out, char** err) {
    if (!lexer) {
        *err = "Lexer is NULL";
        return STATUS_FAIL;
    }

    char* value = NULL;
    u32 row = 0, column = 0, length = 0;

    // Setting the start of the token
    value = (char*)lexer->src + lexer->current_index;
    row = lexer->current_line;
    column = lexer->current_column;

    // We got the first character of the symbol, advance next (max symbol length == 2)
    advance(lexer);
    ++length;

    TokenType type = find_symbol(value, length);

    // If the next character is also a symbol
    if (can_advance(lexer) && is_symbol(lexer->current_char)) {
        TokenType second_type = find_symbol(value, length + 1);
        if (second_type != TOKEN_UNKNOWN) {
            ++length;
            advance(lexer);
            type = second_type;
        }
    }

    char* token_value = calloc(1, sizeof(char) * (length + 1));
    strncpy(token_value, value, length);
    
    return token_new(
        (Token){
            .type=type,
            .value=token_value,
            .length=length,
            .keyword_type=KEYWORD_UNKNOWN,
            .row=row,
            .column=column,
        },
        out,
        err
    );
}

static enum STATUS collect_id(Lexer* lexer, Token** out, char** err) {
    if (!lexer) {
        *err = "Lexer is NULL";
        return STATUS_FAIL;
    }

    char* value = NULL;
    u32 row = 0, column = 0, length = 0;

    // Setting the start of the token
    value = (char*)lexer->src + lexer->current_index;
    row = lexer->current_line;
    column = lexer->current_column;

    // Collecting the id
    while (can_advance(lexer) && !isspace(lexer->current_char) && !is_symbol(lexer->current_char)) {
        ++length;
        advance(lexer);
    }

    TokenType Tokenype = TOKEN_ID;
    KeywordType keyword_type = KEYWORD_UNKNOWN;

    if ((keyword_type = find_keyword(value, length)) != KEYWORD_UNKNOWN) {
        Tokenype = TOKEN_KEYWORD;
    }

    char* token_value = calloc(1, sizeof(char) * (length + 1));
    strncpy(token_value, value, length);
    
    return token_new(
        (Token){
            .type=Tokenype,
            .value=token_value,
            .length=length,
            .keyword_type=keyword_type,
            .row=row,
            .column=column,
        },
        out,
        err
    );
}
