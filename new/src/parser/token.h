#pragma once

#include "../defines.h"

typedef enum TokenType_t {
    TOKEN_UNKNOWN,
    
    TOKEN_ADD,                //  +
    TOKEN_SUB,                //  -
    TOKEN_DIV,                //  /
    TOKEN_MUL,                //  *
    TOKEN_MOD,                //  %
    TOKEN_POW,                //  **

    TOKEN_ADD_EQ,                //  +=
    TOKEN_SUB_EQ,                //  -=
    TOKEN_DIV_EQ,                //  /=
    TOKEN_MUL_EQ,                //  *=
    TOKEN_MOD_EQ,                //  %=
    
    TOKEN_AND,                //  &
    TOKEN_OR,                 //  |
    TOKEN_HAT,                //  ^
    TOKEN_SHL,                //  <<
    TOKEN_SHR,                //  >>
    TOKEN_NOT,                //  ~
    
    TOKEN_BOOL_NOT,           //  !
    TOKEN_BOOL_EQ,            //  ==
    TOKEN_BOOL_NE,            //  !=
    TOKEN_BOOL_LT,            //  <
    TOKEN_BOOL_GT,            //  >
    TOKEN_BOOL_LTE,           //  <=
    TOKEN_BOOL_GTE,           //  >=
    TOKEN_BOOL_AND,           //  &&
    TOKEN_BOOL_OR,            //  ||
    TOKEN_ID,                 //  main x y foo
    TOKEN_KEYWORD,            //  NOT USED
    TOKEN_SIZEOF,             //  sizeof
    TOKEN_AS,                 //  as
    
    TOKEN_CHAR,               //  'a'
    TOKEN_STRING,             //  "Hello world"
    TOKEN_INT,                //  5 6 456
    TOKEN_FLOAT,              //  6.9 7893.6   
    
    TOKEN_LPAREN,             //  (   
    TOKEN_RPAREN,             //  )              
    TOKEN_LBRACKET,           //  [              
    TOKEN_RBRACKET,           //  ]              
    TOKEN_LBRACE,             //  {          
    TOKEN_RBRACE,             //  }
    
    TOKEN_EQUALS,             //  =   
    TOKEN_COMMA,              //  ,  
    TOKEN_DOT,                //  .  
    TOKEN_COLON,              //  :  
    TOKEN_NAMESPACE,          //  ::  
    TOKEN_SEMI,               //  ;   
    TOKEN_QUESTION,           //  ?   
    TOKEN_AT,                 //  @
    TOKEN_HASH,               //  #
    TOKEN_LINE_COMMENT,       // //
    TOKEN_MULTILINE_COMMENT_START,     // /*
    TOKEN_MULTILINE_COMMENT_END,       // */
    TOKEN_POINTER,            //  ->
    TOKEN_ARROW,              //  =>
    TOKEN_BACKSLASH,          // 

    TOKEN_NL,                 //  New line
    TOKEN_EOF,                //  The end of the file
} TokenType_t;

typedef enum KeywordType {
    KEYWORD_UNKNOWN,
    KEYWORD_NULL,
    KEYWORD_TRUE,
    KEYWORD_FALSE,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,
    KEYWORD_FOR,
    KEYWORD_RETURN,
    KEYWORD_IMPORT,
    KEYWORD_AS,
    KEYWORD_FROM,
} KeywordType;

typedef struct Token{
    TokenType_t type;
    
    char* value;
    u32 length;
    KeywordType keyword_type;
    
    u32 row, column;
} Token;

const char* token_type_to_str(TokenType_t type);
const char* keyword_type_to_str(KeywordType ktype);

Result token_new(Token token, Token** out);
void token_print(const Token* token);
void token_free(Token* token);

bool is_symbol(char c);
TokenType_t find_symbol(char* s, u32 length);
KeywordType find_keyword(char* s);