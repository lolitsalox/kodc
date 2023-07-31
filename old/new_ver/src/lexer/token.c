#include "token.h"

#include <stdio.h>
#include <stdlib.h>

enum STATUS token_new(Token token, Token** out, char** err) {
    if (!out) {
        *err = "Out parameter is NULL";
        return STATUS_FAIL;
    }
    
    #ifdef DEBUG_TOKEN
    LOG_ARGS("Creating a token from type %s\n", token_type_to_str(token.type))
    #endif

    *out = malloc(sizeof(Token));
    if (!*out) {
        *err = "Coudln't allocate for token";
        return STATUS_FAIL;
    }

    **out = token;
    return STATUS_OK;
}

const char* token_type_to_str(TokenType type) {
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

void token_print(const Token* token) {
    #ifdef DEBUG_TOKEN
    if (!token)
        LOG("Trying to print a NULL token\n")
    #endif
    if (!token) return;

    printf("(%d:%d - %s)", token->row, token->column, token_type_to_str(token->type));
    if (token->value)
        printf(": %.*s", token->length, token->value);
    printf("\n");
}

void token_free(Token* token) {
    if (!token) return;
    #ifdef DEBUG_TOKEN
    LOG_ARGS("Freeing token from type %s\n", token_type_to_str(token->type))
    #endif

    if (token->value)
        free(token->value);
    free(token);
}