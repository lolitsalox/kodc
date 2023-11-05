#include <parser/parser.hpp>
#include <algorithm>

namespace kod {

std::string ProgramNode::to_string() const {
    std::string result = "ProgramNode:\n";
    for (auto const& statement : statements) {
        result += statement->to_string() + "\n";
    }
    return result;
}

std::string BinaryOpNode::to_string() const {
    return "BinaryOpNode: " + left->to_string() + " " + std::to_string(static_cast<int>(op)) + " " + right->to_string();
}

std::string UnaryOpNode::to_string() const {
    return "UnaryOpNode: " + std::to_string(static_cast<int>(op)) + " " + value->to_string();
}

std::string AssignmentNode::to_string() const {
    return "AssignmentNode:\n" + left->to_string() + " = " + right->to_string();
}

std::string CallNode::to_string() const {
    std::string result = "CallNode: " + callee->to_string();
    for (auto const& arg : args) {
        result += " " + arg->to_string();
    }
    return result;
}

std::string FunctionDefNode::to_string() const {
    std::string result = "FunctionDefNode: " + callee->to_string() + "\nArgs:\n";
    for (auto const& arg : args) {
        result += " " + arg->to_string();
    }
    result += "\nBody:\n";
    for (auto const& st : body) {
        result += " " + st->to_string() + "\n";
    }
    return result;
}

std::string LambdaNode::to_string() const {
    std::string result = "LambdaNode:\nArgs:\n";
    for (auto const& arg : args) {
        result += " " + arg->to_string();
    }
    result += "\nBody:\n";
    for (auto const& st : body) {
        result += " " + st->to_string() + "\n";
    }
    return result;
}

std::string ReturnNode::to_string() const {
    return "ReturnNode: " + (value ? value.value()->to_string() : std::string("NULL"));
}

std::string IntegerNode::to_string() const {
    return "IntegerNode: " + std::to_string(value);
}

std::string FloatNode::to_string() const {
    return "FloatNode: " + std::to_string(value);
}

std::string StringNode::to_string() const {
    return "StringNode: " + value;
}

std::string BooleanNode::to_string() const {
    return std::string("BooleanNode: ") + (value ? "true" : "false");
}

std::string IdentifierNode::to_string() const {
    return "IdentifierNode: " + value;
}

Parser::Parser(kod::Lexer& lexer) 
    : lexer(lexer){

}

void Parser::skip_newlines() {
    while (lexer.peek().value_or(Token{}).type == TokenType::NEW_LINE || lexer.peek().value_or(Token{}).type == TokenType::SEMI) {
        lexer.next();
    }
}

void Parser::eat(kod::TokenType type) {
    if (lexer.peek().value_or(Token{}).type != type) {
        throw std::runtime_error("Unexpected token: " + lexer.peek().value_or(Token{}).to_string() + ".\nExpected: " + std::to_string(static_cast<int>(type)));
    }

    lexer.next();
}

std::unique_ptr<ProgramNode> Parser::parse_program() {
    auto result = std::make_unique<ProgramNode>();
    while (true) {
        skip_newlines();
        auto statement = parse_statement();
        if (!statement) break;
        result->statements.push_back(std::move(statement.value()));
    }
    return result;
}

std::optional<std::unique_ptr<Node>> Parser::parse_statement() {
    return parse_expression();
}

std::optional<std::unique_ptr<Node>> Parser::parse_expression() {
    return parse_assignment();
}

std::optional<std::unique_ptr<Node>> Parser::parse_assignment() {
    auto left = parse_bool_or();
    if (!left) return {};
    
    // Check if current token is EQUALS
    Token token = lexer.peek().value_or(Token{});
    if (token.type != TokenType::EQUALS) {
        return left;
    }
    eat(TokenType::EQUALS);

    auto right = parse_bool_or();
    if (!right) {
        throw std::runtime_error("Error parsing assignment");
    }
    
    return std::make_unique<AssignmentNode>(
        std::move(left.value()), 
        std::move(right.value())
    );
}

std::optional<std::unique_ptr<Node>> 
Parser::parseBinaryOperators(
    const std::vector<TokenType>& operators, 
    std::optional<std::unique_ptr<Node>> (Parser::*parseFunc)()
) {
    auto left = (this->*parseFunc)();

    while (true && !getting_params) {
        TokenType op = lexer.peek().value_or(Token{}).type;
        if (std::find(operators.begin(), operators.end(), op) == operators.end()) break;

        eat(op);
        auto right = (this->*parseFunc)();
        if (!right) {
            throw std::runtime_error("Error parsing binary operator");
        }
        left = std::make_unique<BinaryOpNode>(op, std::move(left.value()), std::move(right.value()));
    }

    return left;
}

std::optional<std::unique_ptr<Node>> Parser::parse_bool_or() {
    return parseBinaryOperators({TokenType::BOOL_OR}, &Parser::parse_bool_and);    
}

std::optional<std::unique_ptr<Node>> Parser::parse_bool_and() {
    return parseBinaryOperators({TokenType::BOOL_AND}, &Parser::parse_bitwise_or);
}

std::optional<std::unique_ptr<Node>> Parser::parse_bitwise_or() {
    return parseBinaryOperators({TokenType::OR}, &Parser::parse_bitwise_xor);
}

std::optional<std::unique_ptr<Node>> Parser::parse_bitwise_xor() {
    return parseBinaryOperators({TokenType::HAT}, &Parser::parse_bitwise_and);
}

std::optional<std::unique_ptr<Node>> Parser::parse_bitwise_and() {
    return parseBinaryOperators({TokenType::AND}, &Parser::parse_bool_equals);
}

std::optional<std::unique_ptr<Node>> Parser::parse_bool_equals() {
    return parseBinaryOperators({TokenType::BOOL_EQ, TokenType::BOOL_NE}, &Parser::parse_bool_gtlt);
}

std::optional<std::unique_ptr<Node>> Parser::parse_bool_gtlt() {
    return parseBinaryOperators({TokenType::BOOL_GT, TokenType::BOOL_LT}, &Parser::parse_bitwise_shlr);
}

std::optional<std::unique_ptr<Node>> Parser::parse_bitwise_shlr() {
    return parseBinaryOperators({TokenType::SHL, TokenType::SHR}, &Parser::parse_add_sub);
}

std::optional<std::unique_ptr<Node>> Parser::parse_add_sub() {
    return parseBinaryOperators({TokenType::ADD, TokenType::SUB}, &Parser::parse_mul_div_mod);
}

std::optional<std::unique_ptr<Node>> Parser::parse_mul_div_mod() {
    return parseBinaryOperators({TokenType::MUL, TokenType::DIV, TokenType::MOD}, &Parser::parse_pow);
}

std::optional<std::unique_ptr<Node>> Parser::parse_pow() {
    return parseBinaryOperators({TokenType::POW}, &Parser::parse_before);
}

std::optional<std::unique_ptr<Node>> Parser::parse_before() {
    static std::vector<TokenType> unary_ops = {TokenType::NOT, TokenType::BOOL_NOT, TokenType::ADD, TokenType::SUB};

    auto ttype = lexer.peek().value_or(Token{}).type;
    if (std::find(unary_ops.begin(), unary_ops.end(), ttype) != unary_ops.end()) {
        eat(ttype);
        
        auto value = parse_before();

        if (!value) {
            throw std::runtime_error("Error parsing unary operator");
        }

        if (IntegerNode* int_value = dynamic_cast<IntegerNode*>(value.value().get())) {
            switch (ttype) {
                case TokenType::ADD: break;
                case TokenType::SUB: int_value->value *= -1; break;
                case TokenType::BOOL_NOT: int_value->value = !int_value->value; break;
                case TokenType::NOT: int_value->value = ~int_value->value; break;
                default: break;
            }
            return value;

        } else if (FloatNode* float_value = dynamic_cast<FloatNode*>(value.value().get())) {
            switch (ttype) {
                case TokenType::ADD: break;
                case TokenType::SUB: float_value->value *= -1; break;
                case TokenType::BOOL_NOT: float_value->value = !float_value->value; break;
                case TokenType::NOT: throw std::runtime_error("Can't use ~ on a float");
                default: break;
            }
            return value;

        }

        return std::make_unique<UnaryOpNode>(ttype, std::move(value.value()));
    }

    return parse_after({});
}

std::optional<std::unique_ptr<Node>> Parser::parse_after(std::optional<std::unique_ptr<Node>> value) {
    value = value ? std::move(value) : parse_factor();
    if (!value) return value;

    switch (lexer.peek().value_or(Token{}).type) {
        // Function definition/Call
        case TokenType::LPAREN: {
            // Parse a list of args/params
            std::vector<std::unique_ptr<Node>> args = parse_tuple(); // parses a () list

            if (lexer.peek().value_or(Token{}).type == TokenType::LBRACE) {
                // Function definition
                auto block = parse_block();
                return std::make_unique<FunctionDefNode>(std::move(value.value()), std::move(args), std::move(block));
            }

            // Function call
            return std::make_unique<CallNode>(std::move(value.value()), std::move(args));

        } break;
        
        default: break;
    }

    return value;
}

std::optional<std::unique_ptr<Node>> Parser::parse_factor() {
    switch (lexer.peek().value_or(Token{}).type) {
    case TokenType::INT:
        return parse_integer();

    case TokenType::FLOAT:
        return parse_float();

    case TokenType::STRING:
        return parse_string();

    case TokenType::ID:
        return parse_identifier();

    case TokenType::LPAREN: {
        eat(TokenType::LPAREN);
        auto result = parse_expression();
        eat(TokenType::RPAREN);
        return result;
    } break;

    case TokenType::OR: {
        auto params = parse_lambda_params();
        auto block = parse_block();
        return std::make_unique<LambdaNode>(std::move(params), std::move(block));
    } break;

    case TokenType::KEYWORD: {
        auto keyword = lexer.next().value_or(Token{});

        switch (keyword.ktype) {
            case KeywordType::RETURN: {
                auto value = parse_expression();
                return std::make_unique<ReturnNode>(std::move(value));
            } break;

            default: throw std::runtime_error("Unexpected keyword: " + keyword.to_string());
        }
    } break;

    case TokenType::END_OF_FILE:
        return {};
    
    default:
        throw std::runtime_error("Unexpected token: " + lexer.peek().value_or(Token{}).to_string());
        break;
    }
}

std::optional<std::unique_ptr<Node>> Parser::parse_integer() {
    return std::make_optional(
        std::make_unique<IntegerNode>(lexer.next().value().int_value)
    );
}

std::optional<std::unique_ptr<Node>> Parser::parse_float() {
    return std::make_optional(
        std::make_unique<FloatNode>(lexer.next().value().float_value)
    );
}

std::optional<std::unique_ptr<Node>> Parser::parse_string() {
    return std::make_optional(
        std::make_unique<StringNode>(lexer.next().value().value)
    );
}

std::optional<std::unique_ptr<Node>> Parser::parse_identifier() {
    return std::make_optional(
        std::make_unique<IdentifierNode>(lexer.next().value().value)
    );
}

std::vector<std::unique_ptr<Node>> Parser::parse_body(TokenType left_delim, TokenType right_delim, bool parse_commas = true) {
    std::vector<std::unique_ptr<Node>> nodes;
    eat(left_delim);

    skip_newlines();

    while (true) {
        auto ttype = lexer.peek().value_or(Token{}).type;
        if (ttype == right_delim ||
            ttype == TokenType::END_OF_FILE) 
        {
            eat(right_delim);
            break;
        }

        skip_newlines();

        nodes.push_back(std::move(parse_expression().value()));

        skip_newlines();

        // parse commas
        if (parse_commas) {
            if (lexer.peek().value_or(Token{}).type == TokenType::COMMA) {
                eat(TokenType::COMMA);
            } else {
                eat(right_delim);
                break;
            };
        }
    }

    return nodes;
}

std::vector<std::unique_ptr<Node>> Parser::parse_tuple() {
    return parse_body(TokenType::LPAREN, TokenType::RPAREN);
}

std::vector<std::unique_ptr<Node>> Parser::parse_lambda_params() {
    getting_params = true;
    auto body = parse_body(TokenType::OR, TokenType::OR);
    getting_params = false;
    return body;
}

std::vector<std::unique_ptr<Node>> Parser::parse_list() {
    return parse_body(TokenType::LBRACKET, TokenType::RBRACKET);
}

std::vector<std::unique_ptr<Node>> Parser::parse_block() {
    return parse_body(TokenType::LBRACE, TokenType::RBRACE, false);
}

}