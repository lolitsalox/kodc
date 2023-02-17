#include "token.h"

#include <stdio.h>
#include <stdlib.h>

token_t* token_new(token_t token) {
    token_t* token_pointer = malloc(sizeof(token_t));
    if (!token_pointer) {
        printf("[token]: Error - coudln't allocate for token\n");
        return NULL;
    }

    *token_pointer = token;
    return token_pointer;
}

const char* token_type_to_str(token_type_t type) {
    switch (type) {
        case TOKEN_ADD:             return          "ADD";
        case TOKEN_SUB:             return          "SUB";               
        case TOKEN_DIV:             return          "DIV";               
        case TOKEN_MUL:             return          "MUL";               
        case TOKEN_MOD:             return          "MOD";               
        case TOKEN_POW:             return          "POW";               
        case TOKEN_AND:             return          "AND";               
        case TOKEN_OR:              return          "OR";               
        case TOKEN_HAT:             return          "HAT";               
        case TOKEN_SHL:             return          "SHL";               
        case TOKEN_SHR:             return          "SHR";               
        case TOKEN_NOT:             return          "NOT";               
        case TOKEN_BOOL_NOT:        return          "BOOL_NOT";          
        case TOKEN_BOOL_EQ:         return          "BOOL_EQ";           
        case TOKEN_BOOL_NOTE:       return          "BOOL_NOTE";         
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

void token_print(const token_t* token) {
    if (!token) return;

    printf("(%d:%d - %s)", token->row, token->column, token_type_to_str(token->token_type));
    if (token->value)
        printf(": %.*s", token->length, token->value);
    printf("\n");
}
