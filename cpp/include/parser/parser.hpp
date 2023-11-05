#pragma once

#include <memory>
#include <vector>

#include <parser/lexer.hpp>

namespace kod {

#include <type_traits>

template <typename Derived, typename Base>
struct is_instance_of {
    static constexpr bool value = std::is_base_of<Base, Derived>::value;
};

struct Node {
    Node() = default;
    virtual ~Node() = default;
    virtual std::string to_string() const = 0;
};

struct ProgramNode : public Node {
    std::vector<std::unique_ptr<Node>> statements;
    std::string to_string() const override;
};

struct BinaryOpNode : public Node {
    TokenType op;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    std::string to_string() const override;

    BinaryOpNode(TokenType op, std::unique_ptr<Node> left, std::unique_ptr<Node> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}
};

struct UnaryOpNode : public Node {
    TokenType op;
    std::unique_ptr<Node> value;
    std::string to_string() const override;

    UnaryOpNode(TokenType op, std::unique_ptr<Node> value)
        : op(op), value(std::move(value)) {}
};

struct AssignmentNode : public Node {
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    std::string to_string() const override;

    AssignmentNode(std::unique_ptr<Node> left, std::unique_ptr<Node> right)
        : left(std::move(left)), right(std::move(right)) {}
};

struct CallNode : public Node {
    std::unique_ptr<Node> callee;
    std::vector<std::unique_ptr<Node>> args;
    std::string to_string() const override;

    CallNode(std::unique_ptr<Node> callee, std::vector<std::unique_ptr<Node>> args)
        : callee(std::move(callee)), args(std::move(args)) {}  
};

struct FunctionDefNode : public Node {
    std::unique_ptr<Node> callee;
    std::vector<std::unique_ptr<Node>> args;
    std::vector<std::unique_ptr<Node>> body;
    std::string to_string() const override;

    FunctionDefNode(std::unique_ptr<Node> callee, std::vector<std::unique_ptr<Node>> args, std::vector<std::unique_ptr<Node>> body)
        : callee(std::move(callee)), args(std::move(args)), body(std::move(body)) {}
};

struct LambdaNode : public Node {
    std::vector<std::unique_ptr<Node>> args;
    std::vector<std::unique_ptr<Node>> body;
    std::string to_string() const override;

    LambdaNode(std::vector<std::unique_ptr<Node>> args, std::vector<std::unique_ptr<Node>> body)
        : args(std::move(args)), body(std::move(body)) {}
};

struct ReturnNode : public Node {
    std::optional<std::unique_ptr<Node>> value;
    std::string to_string() const override;

    ReturnNode(std::optional<std::unique_ptr<Node>> value)
        : value(std::move(value)) {}
};

struct IntegerNode : public Node {
    int64_t value;
    std::string to_string() const override;

    IntegerNode(int64_t value) : value(value) {}
};

struct FloatNode : public Node {
    double value;
    std::string to_string() const override;

    FloatNode(double value) : value(value) {}
};

struct StringNode : public Node {
    std::string value;
    std::string to_string() const override;

    StringNode(std::string value) : value(std::move(value)) {}
};

struct BooleanNode : public Node {
    bool value;
    std::string to_string() const override;

    BooleanNode(bool value) : value(value) {}
};

struct IdentifierNode : public Node {
    std::string value;
    std::string to_string() const override;

    IdentifierNode(std::string value) : value(std::move(value)) {}
};

class Parser {
public:
    Lexer& lexer;
    std::unique_ptr<Node> root;

    Parser(Lexer& lexer);

    std::unique_ptr<ProgramNode> parse_program();
private:
    bool getting_params = false;

    void skip_newlines();
    void eat(TokenType type);

    std::optional<std::unique_ptr<Node>> parse_statement();
    std::optional<std::unique_ptr<Node>> parse_expression();
    std::optional<std::unique_ptr<Node>> parse_assignment();
    std::optional<std::unique_ptr<Node>> parse_integer();
    std::optional<std::unique_ptr<Node>> parse_float();
    std::optional<std::unique_ptr<Node>> parse_string();
    std::optional<std::unique_ptr<Node>> parse_boolean();
    std::optional<std::unique_ptr<Node>> parse_identifier();

    std::vector<std::unique_ptr<Node>> parse_body(TokenType left_delim, TokenType right_delim, bool parse_commas);
    std::vector<std::unique_ptr<Node>> parse_tuple(); // (..., )
    std::vector<std::unique_ptr<Node>> parse_lambda_params(); // |..., |
    std::vector<std::unique_ptr<Node>> parse_list();  // [..., ]
    std::vector<std::unique_ptr<Node>> parse_block();  // {...}

    std::optional<std::unique_ptr<Node>> parse_bool_or();
    std::optional<std::unique_ptr<Node>> parse_bool_and();
    std::optional<std::unique_ptr<Node>> parse_bitwise_or();
    std::optional<std::unique_ptr<Node>> parse_bitwise_xor();
    std::optional<std::unique_ptr<Node>> parse_bitwise_and();
    std::optional<std::unique_ptr<Node>> parse_bool_equals();
    std::optional<std::unique_ptr<Node>> parse_bool_gtlt();
    std::optional<std::unique_ptr<Node>> parse_bitwise_shlr();
    std::optional<std::unique_ptr<Node>> parse_add_sub();
    std::optional<std::unique_ptr<Node>> parse_mul_div_mod();
    std::optional<std::unique_ptr<Node>> parse_pow();
    std::optional<std::unique_ptr<Node>> parse_before(); // unary like: ! - + ++ -- & ~ *
    std::optional<std::unique_ptr<Node>> parse_after(std::optional<std::unique_ptr<Node>> value);
    std::optional<std::unique_ptr<Node>> parse_factor();

    std::optional<std::unique_ptr<Node>> parseBinaryOperators(
        const std::vector<TokenType>& operators, 
        std::optional<std::unique_ptr<Node>> (Parser::*parseFunc)()
    );

};


}