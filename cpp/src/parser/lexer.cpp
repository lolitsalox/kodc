#include <parser/lexer.hpp>

#include <cassert>
#include <vector>
#include <algorithm>
#include <sstream>

namespace kod {

std::string Token::to_string() const {
    std::ostringstream os;
    os << "(" << line << ":" << column << ") ";
    os << "[" << static_cast<int>(type) << "]: ";
    os << value;
    return os.str();
}

std::ostream& operator<<(std::ostream& os, const kod::Token& token) {
    // print location, type and value
    return os << token.to_string();
}

int Lexer::peek_string(int offset = 1) {
    auto index = current_index + offset;
    return index < (int)input.size() ? input[index] : -1;
}

bool Lexer::is_start_of_comment() {
    return (current_char == '/' && peek_string() == '/') 
    || (current_char == '/' && peek_string() == '*');
}

std::optional<Token> Lexer::next() {
    lexer_next:;
    if (!can_advance()) return std::make_optional<Token>(TokenType::END_OF_FILE);
    skip_whitespace();

    // if start of a string
    if (current_char == '\"' || current_char == '\'') {
        return collect_string();
    }

    //if start of a number
    if (std::isdigit(current_char)) {
        return collect_number();
    }

    // if start of an identifier
    if (std::isalpha(current_char) || current_char == '_') {
        return collect_identifier();
    }

    if (is_symbol(current_char)) {
        skip_comment();
        if (is_start_of_comment() || !is_symbol(current_char)) goto lexer_next;
        return collect_symbol();
    }

    if (current_char == '\n') {
        auto res = std::make_optional<Token>(TokenType::NEW_LINE, "", line, column);
        advance();
        return res;
    }

    return {};
}

std::optional<Token> Lexer::peek() {
    // Save current position
    auto old_line = line;
    auto old_column = column;
    auto old_char = current_char;
    auto old_index = current_index;
    
    auto token = next();
    
    // Restore current position
    line = old_line;
    column = old_column;
    current_char = old_char;
    current_index = old_index;
    return token;
}

bool Lexer::can_advance() {
    return current_char != -1;
}

void Lexer::advance() {
    if (!can_advance()) return;

    if (current_char == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }

    current_char = current_index < (int)input.size() ? input[++current_index] : -1;
}

void Lexer::skip_whitespace() {
    while (can_advance() && std::isspace(current_char) && 
        current_char != '\r' && current_char != '\n') {
        advance();
    }
}

void Lexer::skip_until(int c) {
    while (can_advance() && current_char != c) {
        advance();
    }
}

void Lexer::skip_comment() {
    if (!can_advance()) return;

    // Check if the next character is a /
    if (current_char != '/') return;

    // Check if the next character is a / or *
    auto peek = peek_string();
    switch (peek) {
        case '/': {
            skip_until('\n');
            advance();
        } break;

        case '*': {
            // skip until "*/"
            while (can_advance()) {
                if (current_char == '*' && peek_string() == '/') {
                    advance();
                    advance();
                    break;
                }
                advance();
            }
        } break;

        default: return;
    }
}

std::optional<Token> Lexer::collect_identifier() {
    // Collect the identifier and return the token
    std::string identifier;
    TokenType ttype = TokenType::ID;
    KeywordType ktype = KeywordType::UNKNOWN;
    auto this_line = line;
    auto this_column = column;

    while (can_advance() && (std::isalnum(current_char) || current_char == '_')) {
        identifier += current_char;
        advance();
    }

    // Check if it's a keyword
    if ((ktype = find_keyword_type(identifier)) != KeywordType::UNKNOWN) {
        ttype = TokenType::KEYWORD;
    }

    auto tok = Token(ttype, identifier, this_line, this_column);
    tok.ktype = ktype;

    return std::make_optional(tok);
}

std::optional<Token> Lexer::collect_string() {
    std::string string;
    TokenType ttype = TokenType::STRING;
    bool single_quote = current_char == '\'';
    auto this_line = line;
    auto this_column = column;

    // Eating first quote/s
    advance();

    while (can_advance() && ( 
        (current_char != '\'' && single_quote) ||
        (current_char != '"' && !single_quote)    
    )) {
        if (current_char == '\\') {
            advance();
            if (!can_advance()) break;
            // check what kind of escape
            switch (current_char) {
                case 'b':
                    string += '\b';
                    break;
                case 'n':
                    string += '\n';
                    break;
                case 't':
                    string += '\t';
                    break;
                case 'r':
                    string += '\r';
                    break;
                case '\'':
                    string += '\'';
                    break;
                case '\"':
                    string += '\"';
                    break;
                case '\\':
                    string += '\\';
                    break;
                // Add more escape characters if needed
                default:
                    // Invalid escape sequence
                    throw std::runtime_error("Invalid escape sequence");
            }
            advance();
            continue;
        }
        string += current_char;
        advance();
    }

    // Check if end of string
    if ((current_char != '\'' && single_quote) ||
        (current_char != '"' && !single_quote)) {
        throw std::runtime_error("Unterminated string");
    }

    // Eat the other quote/s
    advance();
    
    return std::make_optional<Token>(ttype, string, this_line, this_column);
}

std::optional<Token> Lexer::collect_number() {
    // Collect the number and return the token
    std::string number;
    auto this_line = line;
    auto this_column = column;
    
    bool dot = false;

    TokenType ttype = TokenType::INT;

    while (can_advance() && (std::isdigit(current_char) || current_char == '.')) {
        if (current_char == '.') {
            if (dot) break;
            dot = true;
        }
        number += current_char;
        advance();
    }

    if (dot) {
        while (can_advance() && std::isdigit(current_char)) {
            number += current_char;
            advance();
        }

        ttype = TokenType::FLOAT;
    }

    auto tok = Token(ttype, number, this_line, this_column);
    if (dot) tok.float_value = std::stod(number);
    else tok.int_value = std::stoi(number);
    return std::optional(tok);
}

std::optional<Token> Lexer::collect_symbol() {
    auto this_line = line;
    auto this_column = column;
    std::string symbol;
    symbol += current_char;
    advance();

    TokenType ttype = find_symbol_type(symbol);

    // If it's a double character symbol
    if (can_advance() && is_symbol(current_char)) {
        std::string new_symbol = symbol;
        new_symbol += current_char;
        
        TokenType second_type = find_symbol_type(new_symbol);
        if (second_type != TokenType::UNKNOWN) {
            ttype = second_type;
            symbol = new_symbol;
            advance();
        }
    }

    return std::make_optional<Token>(ttype, symbol, this_line, this_column);
}

KeywordType Lexer::find_keyword_type(const std::string& identifier) {
    // List of known keywords
    std::vector<std::pair<std::string, KeywordType>> keywords = {
        {"if", KeywordType::IF}, 
        {"else", KeywordType::ELSE},
        {"for", KeywordType::FOR},
        {"while", KeywordType::WHILE},
        {"return", KeywordType::RETURN},
        {"break", KeywordType::BREAK},
        {"continue", KeywordType::CONTINUE},
        {"import", KeywordType::IMPORT},
        {"as", KeywordType::AS},
        {"from", KeywordType::FROM},
    };

    // use a std function to find the keyword
    auto it = std::find_if(keywords.begin(), keywords.end(), [&](const auto& pair) {
        return pair.first == identifier;
    });

    if (it != keywords.end()) return it->second;
    return KeywordType::UNKNOWN;
}

TokenType Lexer::find_symbol_type(const std::string& s) {
    TokenType ttype = TokenType::UNKNOWN;
    if (s.length() == 2) {
        switch (s[0]) {
            case ':': if (s[1] == s[0]) return TokenType::NAMESPACE; break;
            case '&': if (s[1] == s[0]) return TokenType::BOOL_AND;  break;
            case '|': if (s[1] == s[0]) return TokenType::BOOL_OR;   break;

            case '*': 
                if (s[1] == s[0]) return TokenType::POW;
                else if (s[1] == '/') return TokenType::MULTILINE_COMMENT_END;
                break;
            
            case '/': 
                if (s[1] == s[0]) return TokenType::LINE_COMMENT; 
                else if (s[1] == '*') return TokenType::MULTILINE_COMMENT_START;
                break;

            case '-': 
                if (s[1] == '>') return TokenType::POINTER; 
                break;

            case '!': 
                if (s[1] == '=') return TokenType::BOOL_NE; 
                break;
            
            case '=': 
                if (s[1] == s[0]) return TokenType::BOOL_EQ; 
                else if (s[1] == '>') return TokenType::ARROW; 
                break;
            
            case '<': 
                if (s[1] == s[0]) return TokenType::SHL; 
                else if (s[1] == '=') return TokenType::BOOL_LTE; 
                break;
            
            case '>': 
                if (s[1] == s[0]) return TokenType::SHR; 
                else if (s[1] == '=') return TokenType::BOOL_GTE; 
                break;

            default: return ttype;
        }
        return ttype;
    }

    switch (s[0]) {
        case '(': return TokenType::LPAREN;
        case ')': return TokenType::RPAREN;
        case '[': return TokenType::LBRACKET;
        case ']': return TokenType::RBRACKET;
        case '{': return TokenType::LBRACE;
        case '}': return TokenType::RBRACE;
        case '=': return TokenType::EQUALS;
        case ',': return TokenType::COMMA;
        case ':': return TokenType::COLON;
        case ';': return TokenType::SEMI;
        case '?': return TokenType::QUESTION;
        case '%': return TokenType::MOD;
        case '\\': return TokenType::BACKSLASH;
        case '#': return TokenType::HASH;
        case '@': return TokenType::AT;
        case '+': return TokenType::ADD;
        case '-': return TokenType::SUB;
        case '/': return TokenType::DIV;
        case '*': return TokenType::MUL;
        case '&': return TokenType::AND;
        case '|': return TokenType::OR;
        case '^': return TokenType::HAT;
        case '<': return TokenType::BOOL_LT;
        case '>': return TokenType::BOOL_GT;
        case '~': return TokenType::NOT;
        case '!': return TokenType::BOOL_NOT;
        case '.': return TokenType::DOT;
        default: return TokenType::UNKNOWN;
    }
    return ttype;
}

bool Lexer::is_symbol(int c) {
    std::string s = "()[]{}=@#,.:;?\\+-/*%&|^<>!~";
    return s.find(c) != std::string::npos;
}

}