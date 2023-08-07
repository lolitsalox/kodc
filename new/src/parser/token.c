#include "token.h"

#include <stdio.h>
#include <stdlib.h>

Result token_new(Token token, Token** out) {
    assert(out && "Out is NULL");
    
    Result res = {0};

    if (!out) {
        res.what = "out parameter is NULL";
        return res;
    }
    
    #ifdef DEBUG_TOKEN
    INFO_ARGS("Creating a token from type %s\n", token_type_to_str(token.type));
    #endif

    *out = malloc(sizeof(Token));
    if (!*out) {
        res.what = "Coudln't allocate for token";
        return res;
    }

    **out = token;
    return res;
}

const char* token_type_to_str(TokenType_t type) {
    switch (type) {
        case TOKEN_ADD:             return          "ADD";
        case TOKEN_SUB:             return          "SUB";               
        case TOKEN_DIV:             return          "DIV";               
        case TOKEN_MUL:             return          "MUL";               
        case TOKEN_MOD:             return          "MOD";               
        case TOKEN_POW:             return          "POW";               
        case TOKEN_ADD_EQ:          return          "TOKEN_ADD_EQ";
        case TOKEN_SUB_EQ:          return          "TOKEN_SUB_EQ";
        case TOKEN_DIV_EQ:          return          "TOKEN_DIV_EQ";
        case TOKEN_MUL_EQ:          return          "TOKEN_MUL_EQ";
        case TOKEN_MOD_EQ:          return          "TOKEN_MOD_EQ";
        case TOKEN_AND:             return          "AND";               
        case TOKEN_OR:              return          "OR";               
        case TOKEN_HAT:             return          "HAT";               
        case TOKEN_SHL:             return          "SHL";               
        case TOKEN_SHR:             return          "SHR";               
        case TOKEN_NOT:             return          "NOT";               
        case TOKEN_BOOL_NOT:        return          "BOOL_NOT";          
        case TOKEN_BOOL_EQ:         return          "BOOL_EQ";           
        case TOKEN_BOOL_NE:       return            "BOOL_NE";         
        case TOKEN_BOOL_LT:         return          "BOOL_LT";           
        case TOKEN_BOOL_GT:         return          "BOOL_GT";           
        case TOKEN_BOOL_LTE:        return          "BOOL_LTE";          
        case TOKEN_BOOL_GTE:        return          "BOOL_GTE";          
        case TOKEN_BOOL_AND:        return          "BOOL_AND";          
        case TOKEN_BOOL_OR:         return          "BOOL_OR";         
        case TOKEN_ID:              return          "ID";              
        case TOKEN_SIZEOF:          return          "SIZEOF";              
        case TOKEN_AS:              return          "AS";              
        case TOKEN_KEYWORD:         return          "KEYWORD";         
        case TOKEN_CHAR:            return          "CHAR";              
        case TOKEN_STRING:          return          "STRING";            
        case TOKEN_INT:             return          "INT";               
        case TOKEN_FLOAT:           return          "FLOAT";             
        case TOKEN_LPAREN:          return          "LPAREN";            
        case TOKEN_RPAREN:          return          "RPAREN";            
        case TOKEN_LBRACKET:        return          "LBRACKET";          
        case TOKEN_RBRACKET:        return          "RBRACKET";          
        case TOKEN_LBRACE:          return          "LBRACE";            
        case TOKEN_RBRACE:          return          "RBRACE";            
        case TOKEN_EQUALS:          return          "EQUALS";            
        case TOKEN_COMMA:           return          "COMMA";             
        case TOKEN_DOT:             return          "DOT";               
        case TOKEN_COLON:           return          "COLON";             
        case TOKEN_NAMESPACE:       return          "NAMESPACE";         
        case TOKEN_SEMI:            return          "SEMI";              
        case TOKEN_QUESTION:        return          "QUESTION";          
        case TOKEN_AT:              return          "AT";               
        case TOKEN_HASH:            return          "HASH";               
        case TOKEN_LINE_COMMENT:    return          "LINE_COMMENT";      
        case TOKEN_MULTILINE_COMMENT_START: return  "MULTILINE_COMMENT_START";   
        case TOKEN_MULTILINE_COMMENT_END:   return  "MULTILINE_COMMENT_END";     
        case TOKEN_POINTER:         return          "POINTER";           
        case TOKEN_ARROW:           return          "ARROW";             
        case TOKEN_BACKSLASH:       return          "BACKSLASH";         
        case TOKEN_NL:              return          "NL";               
        case TOKEN_EOF:             return          "EOF";   
        case TOKEN_UNKNOWN:         return          "UNKNOWN";
    }
    return "UNKNOWN";
}

const char* keyword_type_to_str(KeywordType ktype) {
    switch (ktype) {
        case KEYWORD_NULL: return "null";
        case KEYWORD_TRUE: return "true";
        case KEYWORD_FALSE: return "false";
        case KEYWORD_IF:    return "if";
        case KEYWORD_ELSE:  return "else";
        case KEYWORD_WHILE: return "while";
        case KEYWORD_FOR:   return "for";
        case KEYWORD_RETURN: return "return";
        case KEYWORD_IMPORT: return "import";
        case KEYWORD_AS:    return "as";
        case KEYWORD_FROM:  return "from";
        default:            return "KEYWORD_UNKNOWN";
    }
}

void token_print(const Token* token) {
    assert(token && "Token is NULL");

    printf("(%d:%d - %s)", token->row, token->column, token_type_to_str(token->type));
    if (token->value) printf(": %s", token->value);
    puts("");
}

// DOES NOT FREE THE TOKEN ITSELF, ONLY THE CONTENTS
void token_free(Token* token) {
    assert(token && "Token is NULL");

    #ifdef DEBUG_TOKEN
    INFO_ARGS("Freeing token from type %s", token_type_to_str(token->type));
    #endif

    if (token->value) free(token->value);
}

bool is_symbol(char c) {
    return strchr("()[]{}=@#,.:;?\\+-/*%&|^<>~!", c) != NULL;
}

TokenType_t find_symbol(char* s, u32 length) {
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

KeywordType find_keyword(char* s) {
    if (strcmp(s, "null") == 0)         return KEYWORD_NULL;
    if (strcmp(s, "true") == 0)         return KEYWORD_TRUE;
    if (strcmp(s, "false") == 0)        return KEYWORD_FALSE;
    if (strcmp(s, "if") == 0)           return KEYWORD_IF;
    if (strcmp(s, "else") == 0)         return KEYWORD_ELSE;
    if (strcmp(s, "while") == 0)        return KEYWORD_WHILE;
    if (strcmp(s, "for") == 0)          return KEYWORD_FOR;
    if (strcmp(s, "return") == 0)       return KEYWORD_RETURN;
    if (strcmp(s, "import") == 0)       return KEYWORD_IMPORT;
    if (strcmp(s, "as") == 0)           return KEYWORD_AS;
    if (strcmp(s, "from") == 0)         return KEYWORD_FROM;

    return KEYWORD_UNKNOWN;
}