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

std::string kod::AccessNode::to_string() const {
    return value->to_string() + "." + field->to_string();
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
    return value;
}

// make one function that the tuple and list can use 
std::string convertToString(const std::vector<std::unique_ptr<Node>>& nodes, std::string const& left, std::string const& right) {
    std::string result = left;
    for (size_t i = 0; i < nodes.size(); ++i) {
        result += nodes[i]->to_string() + (i == nodes.size() - 1 ? "" : ", ");
    }
    result += right;
    return result;
}

std::string TupleNode::to_string() const {
    return convertToString(values, "(", ")");
}

std::string ListNode::to_string() const {
    return convertToString(values, "[", "]");
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

    auto right = parse_assignment();
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
            auto ls = parse_tuple(); // parses a () list
            std::vector<std::unique_ptr<Node>> args;
            if (TupleNode* tuple = dynamic_cast<TupleNode*>(ls.get())) {
                args = std::move(tuple->values);
            }
            else {
                args.push_back(std::move(ls)); // 1 object
            }

            // check if value is from type identifier
            if (dynamic_cast<IdentifierNode*>(value.value().get())) {
                if (lexer.peek().value_or(Token{}).type == TokenType::LBRACE) {
                    // Function definition
                    auto block = parse_block();
                    return std::make_unique<FunctionDefNode>(std::move(value.value()), std::move(args), std::move(block));
                }
            } // call(1)

            // Function call
            return std::make_unique<CallNode>(std::move(value.value()), std::move(args));

        } break;

        case TokenType::DOT: {
            eat(TokenType::DOT);

            auto id = parse_identifier();

            return std::make_unique<AccessNode>(std::move(value.value()), std::move(id.value()));
        } break;

        // case TokenType::LBRACE: {
        //     // subscript
        //     auto subscript = parse_list();

        //     // check if it's empty, if so throw an error
        //     if (subscript.empty()) {
        //         throw std::runtime_error("Subscript is empty");
        //     }

        //     return std::make_unique<SubscriptNode>(std::move(value.value()), std::move(subscript));
        // }

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
        return parse_tuple();
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
                std::optional<std::unique_ptr<Node>> value = {}; 
                if (lexer.peek().value_or(Token{}).type != TokenType::NEW_LINE) {
                    value = parse_expression();
                }
                return std::make_unique<ReturnNode>(std::move(value));
            } break;

            default: throw std::runtime_error("Unexpected keyword: " + keyword.to_string());
        }
    } break;

    case TokenType::UNKNOWN:
    case TokenType::END_OF_FILE:
        return {};
    
    default:
        throw std::runtime_error("Unexpected token: " + lexer.peek().value_or(Token{}).to_string());
        break;
    }

    return {};
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

std::optional<std::unique_ptr<IdentifierNode>> Parser::parse_identifier() {
    auto tok = lexer.next().value();
    if (tok.type != TokenType::ID) {
        throw std::runtime_error("Expected an identifer but got " + std::to_string(static_cast<int>(tok.type)));
    }
    return std::make_unique<IdentifierNode>(tok.value);
}

std::vector<std::unique_ptr<Node>> Parser::parse_body(
    TokenType left_delim, 
    TokenType right_delim, 
    bool parse_commas = true, 
    std::optional<bool*> got_comma = {}
) {
    std::vector<std::unique_ptr<Node>> nodes;
    if (got_comma) *got_comma.value() = false;
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
                if (got_comma) *got_comma.value() = true;
            } else {
                eat(right_delim);
                break;
            };
        }
    }

    return nodes;
}
// (1,2) (,) () (1)
std::unique_ptr<Node> Parser::parse_tuple() {
    bool got_comma = false;
    auto body = parse_body(TokenType::LPAREN, TokenType::RPAREN, true, &got_comma);
    if (!body.empty() && !got_comma) {
        return std::move(body.back());
    }
    return std::make_unique<TupleNode>(std::move(body));
}

std::vector<std::unique_ptr<Node>> Parser::parse_lambda_params() {
    getting_params = true;
    auto body = parse_body(TokenType::OR, TokenType::OR);
    getting_params = false;
    return body;
}

std::unique_ptr<ListNode> Parser::parse_list() {
    return std::make_unique<ListNode>(parse_body(TokenType::LBRACKET, TokenType::RBRACKET));
}

std::vector<std::unique_ptr<Node>> Parser::parse_block() {
    return parse_body(TokenType::LBRACE, TokenType::RBRACE, false);
}

void ProgramNode::compile(CompiledModule& module, Code& code) {
    code.code.clear();

    for (auto& statement : statements) {
        statement->compile(module, code);
        if (!statement->pushes()) continue;;
        code.write8((uint8_t)Opcode::OP_POP_TOP);
    }

    if (statements.empty() || (statements.size() > 0 && !statements.back()->returns())) {
        // Find the index of the null constant in the constant pool (if there isn't, create one)
        auto it = std::find(module.constant_pool.begin(), module.constant_pool.end(), Constant(ConstantTag::C_NULL));
        if (it == module.constant_pool.end()) {
            // Create a new entry in the constant pool
            module.constant_pool.push_back(Constant(ConstantTag::C_NULL));
            it = std::prev(module.constant_pool.end());
        }

        // Get the index of the null constant
        size_t index = std::distance(module.constant_pool.begin(), it);

        // Push the null constant
        code.write8((uint8_t)Opcode::OP_LOAD_CONST);
        code.write32(index);
        code.write8((uint8_t)Opcode::OP_RETURN);
    }
}

void IntegerNode::compile(CompiledModule& module, Code& code) {
    auto constant = Constant(value);

    auto it = std::find(module.constant_pool.begin(), module.constant_pool.end(), constant);
    if (it == module.constant_pool.end()) {
        // Create a new entry in the constant pool
        module.constant_pool.push_back(constant);
        it = std::prev(module.constant_pool.end());
    }

    size_t index = std::distance(module.constant_pool.begin(), it);
    code.write8((uint8_t)Opcode::OP_LOAD_CONST);
    code.write32(index);
}

void FloatNode::compile(CompiledModule& module, Code& code) {
    auto constant = Constant(value);

    auto it = std::find(module.constant_pool.begin(), module.constant_pool.end(), constant);
    if (it == module.constant_pool.end()) {
        // Create a new entry in the constant pool
        module.constant_pool.push_back(constant);
        it = std::prev(module.constant_pool.end());
    }

    size_t index = std::distance(module.constant_pool.begin(), it);
    code.write8((uint8_t)Opcode::OP_LOAD_CONST);
    code.write32(index);
}

void StringNode::compile(CompiledModule& module, Code& code) {
    auto constant = Constant(value);

    auto it = std::find(module.constant_pool.begin(), module.constant_pool.end(), constant);
    if (it == module.constant_pool.end()) {
        // Create a new entry in the constant pool
        module.constant_pool.push_back(constant);
        it = std::prev(module.constant_pool.end());
    }

    size_t index = std::distance(module.constant_pool.begin(), it);
    code.write8((uint8_t)Opcode::OP_LOAD_CONST);
    code.write32(index);
}

void IdentifierNode::compile(CompiledModule& module, Code& code) {
    auto it = std::find(module.name_pool.begin(), module.name_pool.end(), value);
    if (it == module.name_pool.end()) {
        // Create a new entry in the name pool
        module.name_pool.push_back(value);
        it = std::prev(module.name_pool.end());
    }

    size_t index = std::distance(module.name_pool.begin(), it);
    code.write8((uint8_t)Opcode::OP_LOAD_NAME);
    code.write32(index);
}

void AccessNode::compile(CompiledModule& module, Code& code) {
    value->compile(module, code);

    auto it = std::find(module.name_pool.begin(), module.name_pool.end(), field->to_string());
    if (it == module.name_pool.end()) {
        // Create a new entry in the name pool
        module.name_pool.push_back(field->to_string());
        it = std::prev(module.name_pool.end());
    }

    size_t index = std::distance(module.name_pool.begin(), it);
    code.write8((uint8_t)Opcode::OP_LOAD_ATTRIBUTE);
    code.write32(index);
}

void ReturnNode::compile(CompiledModule& module, Code& code) {
    if (value) {
        value.value()->compile(module, code);
        if (!value.value()->pushes()) {
            value.value()->push(module, code);
        }
    } else {
        // load null constant
        auto it = std::find(module.constant_pool.begin(), module.constant_pool.end(), Constant(ConstantTag::C_NULL));
        if (it == module.constant_pool.end()) {
            // Create a new entry in the constant pool
            module.constant_pool.push_back(Constant(ConstantTag::C_NULL));
            it = std::prev(module.constant_pool.end());
        }

        // Get the index of the null constant
        size_t index = std::distance(module.constant_pool.begin(), it);

        // Push the null constant
        code.write8((uint8_t)Opcode::OP_LOAD_CONST);
        code.write32(index);
    }
    code.write8((uint8_t)Opcode::OP_RETURN);
}

void AssignmentNode::compile(CompiledModule& module, Code& code) {
    right->compile(module, code);
    if (!right->pushes()) {
        right->push(module, code);
    }

    // Check if the left side is an identifier
    if (auto identifier = dynamic_cast<IdentifierNode*>(left.get())) {
        // Get the index of the identifier
        auto it = std::find(module.name_pool.begin(), module.name_pool.end(), identifier->value);

        if (it == module.name_pool.end()) {
            // Create a new entry in the name pool
            module.name_pool.push_back(identifier->value);
            it = std::prev(module.name_pool.end());
        }

        size_t index = std::distance(module.name_pool.begin(), it);
        code.write8((uint8_t)Opcode::OP_STORE_NAME);
        code.write32(index);

    } else {
        throw std::runtime_error("Left side of assignment must be an identifier (unimplemented)");
    }
}

void AssignmentNode::push(CompiledModule& module, Code& code) const {
    // Push the value by loading it from the name pool (LOAD_NAME)
    if (auto identifier = dynamic_cast<IdentifierNode*>(left.get())) {
        // Get the index of the identifier
        auto it = std::find(module.name_pool.begin(), module.name_pool.end(), identifier->value);

        if (it == module.name_pool.end()) {
            // Create a new entry in the name pool
            module.name_pool.push_back(identifier->value);
            it = std::prev(module.name_pool.end());
        }

        size_t index = std::distance(module.name_pool.begin(), it);
        code.write8((uint8_t)Opcode::OP_LOAD_NAME);
        code.write32(index);

    } else {
        throw std::runtime_error("Left side of assignment must be an identifier (unimplemented)");
    }
}

void compile_code_constant(CompiledModule& module, Code& code, 
    std::string name,
    std::vector<std::unique_ptr<kod::Node>> const& func_args,
    std::vector<std::unique_ptr<kod::Node>> const& body
) {
    Code func;
    func.name = name;

    // Convert the arguments to a vector of strings
    std::vector<std::string> args;
    for (auto& arg : func_args) {
        args.push_back(arg->to_string());
    }

    func.params = std::move(args);

    for (auto& statement : body) {
        statement->compile(module, func);
        if (!statement->pushes()) continue;;
        func.write8((uint8_t)Opcode::OP_POP_TOP);
    }

    if (body.empty() || (body.size() > 0 && !body.back()->returns())) {
        // Find the index of the null constant in the constant pool (if there isn't, create one)
        auto it = std::find(module.constant_pool.begin(), module.constant_pool.end(), Constant(ConstantTag::C_NULL));
        if (it == module.constant_pool.end()) {
            // Create a new entry in the constant pool
            module.constant_pool.push_back(Constant(ConstantTag::C_NULL));
            it = std::prev(module.constant_pool.end());
        }

        // Get the index of the null constant
        size_t index = std::distance(module.constant_pool.begin(), it);

        // Push the null constant
        func.write8((uint8_t)Opcode::OP_LOAD_CONST);
        func.write32(index);
        func.write8((uint8_t)Opcode::OP_RETURN);
    }

    // Add the code object to the constant pool
    Constant func_constant(func);
    auto code_it = std::find(module.constant_pool.begin(), module.constant_pool.end(), func_constant);
    if (code_it == module.constant_pool.end()) {
        // Create a new entry in the constant pool
        module.constant_pool.push_back(func_constant);
        code_it = std::prev(module.constant_pool.end());
    }

    // Load the constant and store it by name index
    size_t index = std::distance(module.constant_pool.begin(), code_it);
    code.write8((uint8_t)Opcode::OP_LOAD_CONST);
    code.write32(index);
}

void FunctionDefNode::compile(CompiledModule& module, Code& code) {
    auto name = callee->to_string();
    compile_code_constant(module, code, name, args, body);

    // find the index of the callee name
    auto name_it = std::find(module.name_pool.begin(), module.name_pool.end(), name);
    if (name_it == module.name_pool.end()) {
        // Create a new entry in the name pool
        module.name_pool.push_back(name);
        name_it = std::prev(module.name_pool.end());
    }

    auto index = std::distance(module.name_pool.begin(), name_it);
    code.write8((uint8_t)Opcode::OP_STORE_NAME);
    code.write32(index);

}

void LambdaNode::compile(CompiledModule& module, Code& code) {
    compile_code_constant(module, code, "<lambda>", args, body);
}

void CallNode::compile(CompiledModule& module, Code& code) {
    for (auto& arg : args) {
        arg->compile(module, code); // todo, build tuple before calling
    }

    callee->compile(module, code);
    code.write8((uint8_t)Opcode::OP_CALL);
    code.write32((uint8_t)args.size());
}

void BinaryOpNode::compile(CompiledModule& module, Code& code) {
    left->compile(module, code);
    right->compile(module, code);

    Opcode bin_op = Opcode::OP_UNKNOWN;
    switch (this->op) {
        case TokenType::ADD:         bin_op = Opcode::OP_BINARY_ADD; break;
        case TokenType::SUB:         bin_op = Opcode::OP_BINARY_SUB; break;
        case TokenType::MUL:         bin_op = Opcode::OP_BINARY_MUL; break;
        case TokenType::DIV:         bin_op = Opcode::OP_BINARY_DIV; break;
        case TokenType::MOD:         bin_op = Opcode::OP_BINARY_MOD; break;
        case TokenType::POW:         bin_op = Opcode::OP_BINARY_POW; break;
        case TokenType::AND:         bin_op = Opcode::OP_BINARY_AND; break;
        case TokenType::OR:          bin_op = Opcode::OP_BINARY_OR; break;
        case TokenType::HAT:         bin_op = Opcode::OP_BINARY_XOR; break;
        case TokenType::SHL:         bin_op = Opcode::OP_BINARY_LEFT_SHIFT; break;
        case TokenType::SHR:         bin_op = Opcode::OP_BINARY_RIGHT_SHIFT; break;
        case TokenType::BOOL_AND:    bin_op = Opcode::OP_BINARY_BOOLEAN_AND; break;
        case TokenType::BOOL_OR:     bin_op = Opcode::OP_BINARY_BOOLEAN_OR; break;
        case TokenType::BOOL_EQ:     bin_op = Opcode::OP_BINARY_BOOLEAN_EQUAL; break;
        case TokenType::BOOL_NE:     bin_op = Opcode::OP_BINARY_BOOLEAN_NOT_EQUAL; break;
        case TokenType::BOOL_GT:     bin_op = Opcode::OP_BINARY_BOOLEAN_GREATER_THAN; break;
        case TokenType::BOOL_GTE:    bin_op = Opcode::OP_BINARY_BOOLEAN_GREATER_THAN_OR_EQUAL_TO; break;
        case TokenType::BOOL_LT:     bin_op = Opcode::OP_BINARY_BOOLEAN_LESS_THAN; break;
        case TokenType::BOOL_LTE:    bin_op = Opcode::OP_BINARY_BOOLEAN_LESS_THAN_OR_EQUAL_TO; break;
        default: throw std::runtime_error("Dont know how to compile this op");
    }

    code.write8((uint8_t)bin_op);
}

void TupleNode::compile(CompiledModule& module, Code& code) {
    bool found = false;
    // find at least one value that is not a constant
    for (auto& value : values) {
        if (!value->is_constant()) {
            found = true;
            break;
        }
    }

    if (found) {
        for (auto& value : values) {
            value->compile(module, code);
        }

        code.write8((uint8_t)Opcode::OP_BUILD_TUPLE);
        code.write32((uint8_t)values.size());
        return;
    }

    // Create a vector of constants
    std::vector<Constant> constants;
    for (auto& value : values) {
        constants.push_back(value->to_constant());
    }

    auto constant = Constant(constants);
    auto it = std::find(module.constant_pool.begin(), module.constant_pool.end(), constant);
    if (it == module.constant_pool.end()) {
        // Create a new entry in the constant pool
        module.constant_pool.push_back(constant);
        it = std::prev(module.constant_pool.end());
    }

    size_t index = std::distance(module.constant_pool.begin(), it);
    code.write8((uint8_t)Opcode::OP_LOAD_CONST);
    code.write32(index);
}

void ListNode::compile(CompiledModule& module, Code& code) {
    for (auto& value : values) {
        value->compile(module, code);
    }

    code.write8((uint8_t)Opcode::OP_BUILD_LIST);
    code.write32((uint8_t)values.size());
}

}