#pragma once

#include <iostream>
#include <optional>
#include <string>

namespace kod {

enum class TokenType {
    UNKNOWN,

    ADD,                //  +
    SUB,                //  -
    DIV,                //  /
    MUL,                //  *
    MOD,                //  %
    POW,                //  **

    ADD_EQ,                //  +=
    SUB_EQ,                //  -=
    DIV_EQ,                //  /=
    MUL_EQ,                //  *=
    MOD_EQ,                //  %=

    AND,                //  &
    OR,                 //  |
    HAT,                //  ^
    SHL,                //  <<
    SHR,                //  >>
    NOT,                //  ~

    BOOL_NOT,           //  !
    BOOL_EQ,            //  ==
    BOOL_NE,            //  !=
    BOOL_LT,            //  <
    BOOL_GT,            //  >
    BOOL_LTE,           //  <=
    BOOL_GTE,           //  >=
    BOOL_AND,           //  &&
    BOOL_OR,            //  ||
    ID,                 //  main x y foo
    KEYWORD,            //  NOT USED
    SIZEOF,             //  sizeof
    AS,                 //  as

    CHAR,               //  'a'
    STRING,             //  "Hello world"
    INT,                //  5 6 456
    FLOAT,              //  6.9 7893.6   

    LPAREN,             //  (   
    RPAREN,             //  )              
    LBRACKET,           //  [              
    RBRACKET,           //  ]              
    LBRACE,             //  {          
    RBRACE,             //  }

    EQUALS,             //  =   
    COMMA,              //  ,  
    DOT,                //  .  
    COLON,              //  :  
    NAMESPACE,          //  ::  
    SEMI,               //  ;   
    QUESTION,           //  ?   
    AT,                 //  @
    HASH,               //  #
    LINE_COMMENT,       // //
    MULTILINE_COMMENT_START,     // /*
    MULTILINE_COMMENT_END,       // */
    POINTER,            //  ->
    ARROW,              //  =>
    BACKSLASH,          // 

    NEW_LINE,                 //  New line
    END_OF_FILE,                //  The end of the file
};

enum class KeywordType {
    UNKNOWN,
    NULL_K,
    TRUE,
    FALSE,
    IF,
    ELSE,
    WHILE,
    FOR,
    RETURN,
    IMPORT,
    AS,
    FROM,
    BREAK,
    CONTINUE,
};

struct Token {
    TokenType type = TokenType::UNKNOWN;
    KeywordType ktype = KeywordType::UNKNOWN;
    std::string value;
    int int_value;
    double float_value;
    size_t line = 0, column = 0;

    std::string to_string() const;

    Token(TokenType type, std::string value, size_t line, size_t column) : type(type), value(value), line(line), column(column) {}
    Token(TokenType type) : type(type) {}
    Token() {}
    ~Token() {}

};

struct Lexer {
    std::string input;
    int current_char = -1, current_index = 0;
    size_t line = 1, column = 1;

    Lexer(std::string input) : input(input) {
        if ((current_char = (input.empty() ? -1 : input[current_index])) == -1) {
            throw std::runtime_error("Unexpected end of file");
        }
    }
    ~Lexer() {
    }

    std::optional<Token> next();
    std::optional<Token> peek();

    static TokenType find_symbol_type(const std::string& s);
    static KeywordType find_keyword_type(const std::string& identifier);
    static bool is_symbol(int c);

private:
    void skip_whitespace();
    void skip_comment();
    void skip_until(int c);
    std::optional<Token> collect_identifier();
    std::optional<Token> collect_string();
    std::optional<Token> collect_number();
    std::optional<Token> collect_symbol();
    void advance();
    bool can_advance();
    bool is_start_of_comment();
    int peek_string(int offset);
};

std::ostream& operator<<(std::ostream& os, const kod::Token& token);

}
