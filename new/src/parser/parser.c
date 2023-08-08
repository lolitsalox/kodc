#include "parser.h"

typedef struct BinaryOperators_t {
    Result (*next)(Parser*, AstNode** out);
    TokenType_t* ops;
} BinaryOperators_t;

#define BIN_OP_STRUCT(next, ...) {next, (TokenType_t[]){__VA_ARGS__, TOKEN_UNKNOWN}}
#define PARSE(ops, precedence) \
    Result ops(Parser* parser, AstNode** out) \
        { return binary(parser, out, operators[precedence]); }

enum {
    e_bool_or,
    e_bool_and,
};

#define LEFT_DELIM 0
#define RIGHT_DELIM 1
typedef TokenType_t delims_t[2];

static delims_t tuple_delims = {TOKEN_LPAREN, TOKEN_RPAREN};
static delims_t list_delims = {TOKEN_LBRACKET, TOKEN_RBRACKET};
static delims_t block_delims = {TOKEN_LBRACE, TOKEN_RBRACE};

typedef Result (parse_function) (Parser* parser, AstNode** out);
static parse_function compound;
static parse_function expression;
static parse_function assignment;
static parse_function bool_or;
static parse_function bool_and;
static parse_function bitwise_or;
static parse_function bitwise_xor;
static parse_function bitwise_and;
static parse_function bool_equals;
static parse_function bool_gtlt;
static parse_function bitwise_shlr;
static parse_function add_sub;
static parse_function mul_div_mod;
static parse_function pow_;
static parse_function before;
static parse_function after;
static parse_function factor;
static parse_function string;
static parse_function int_;
static parse_function float_;
static parse_function tuple;
static parse_function list;
static parse_function block;

static Result body(Parser* parser, AstNode** out, delims_t delims, bool commas);

BinaryOperators_t operators[] = {
    BIN_OP_STRUCT(bool_and, TOKEN_BOOL_OR),
    BIN_OP_STRUCT(bitwise_or, TOKEN_BOOL_AND),
    BIN_OP_STRUCT(bitwise_xor, TOKEN_OR),
    BIN_OP_STRUCT(bitwise_and, TOKEN_HAT),
    BIN_OP_STRUCT(bool_equals, TOKEN_AND),
    BIN_OP_STRUCT(bool_gtlt, TOKEN_BOOL_EQ, TOKEN_BOOL_NE),
    BIN_OP_STRUCT(bitwise_shlr, TOKEN_BOOL_GT, TOKEN_BOOL_GTE, TOKEN_BOOL_LT, TOKEN_BOOL_LTE),
    BIN_OP_STRUCT(add_sub, TOKEN_SHL, TOKEN_SHR),
    BIN_OP_STRUCT(mul_div_mod, TOKEN_ADD, TOKEN_SUB),
    BIN_OP_STRUCT(pow_, TOKEN_MUL, TOKEN_DIV, TOKEN_MOD),
    BIN_OP_STRUCT(before, TOKEN_POW),
};

static Result binary(Parser* parser, AstNode** out, BinaryOperators_t binop);
PARSE(bool_or, 0)
PARSE(bool_and, 1)
PARSE(bitwise_or, 2)
PARSE(bitwise_xor, 3)
PARSE(bitwise_and, 4)
PARSE(bool_equals, 5)
PARSE(bool_gtlt, 6)
PARSE(bitwise_shlr, 7)
PARSE(add_sub, 8)
PARSE(mul_div_mod, 9)
PARSE(pow_, 10)

inline Parser parser_init(Lexer lexer) {
    return (Parser) {.lexer=lexer};
}

Result parser_parse(Parser* parser, AstNode** out) {
    assert(parser && "Parser is NULL");

    // Getting first token
    unwrap(lexer_get_next_token(&parser->lexer, &parser->current_token));

    unwrap(compound(parser, out));

    assert(*out && "parse returned NULL");
    (*out)->type = AST_ROOT;

    token_free(&parser->current_token);
    return result_ok();
}

Result eat(Parser* parser, TokenType_t ttype) {
    if (parser->current_token.type != ttype) {
        ERROR_LOG("Parser", "unexpected token at ");

        token_print(parser->current_token);

        printf(", expected (%s)\n", token_type_to_str(ttype));
        return result_error("unexpected token");
    }

    token_free(&parser->current_token);
    
    return lexer_get_next_token(&parser->lexer, &parser->current_token);
}

static inline void skip_newlines(Parser* parser) {
    while (parser->current_token.type == TOKEN_NL)
        unwrap(eat(parser, TOKEN_NL));
}

inline bool is_type_in_types(TokenType_t type, TokenType_t* types) {
    for (size_t i = 0; types[i] != TOKEN_UNKNOWN; ++i) {
        if (type == types[i]) return true;
    }
    return false;
}

Result binary(Parser* parser, AstNode** out, BinaryOperators_t binop) {
    AstNode* left = NULL;
    TokenType_t op = TOKEN_UNKNOWN;

    unwrap(binop.next(parser, &left));
    

    op = parser->current_token.type;
    while (is_type_in_types(op, binop.ops)) {

        unwrap(eat(parser, op)); // eating the operator

        AstNode* right = NULL;
        unwrap(binop.next(parser, &right));

        AstNode* new_node = NULL;
        unwrap(ast_new((AstNode){.type=AST_BIN_OP,.binary_op={left,right,op}}, &new_node));

        left = new_node;
        op = parser->current_token.type;
    }

    *out = left;
    return result_ok();
}

Result compound(Parser* parser, AstNode** out) {
    unwrap(ast_new((AstNode){.type=AST_COMPOUND}, out));

    while (parser->current_token.type != TOKEN_EOF) {
        AstNode* value = NULL;

        skip_newlines(parser);

        unwrap(expression(parser, &value));
        if (!value) continue;

        skip_newlines(parser);

        DA_APPEND((*out)->compound, value);
    }

    return result_ok();
}

Result expression(Parser* parser, AstNode** out) {
    return assignment(parser, out);
}

Result assignment(Parser* parser, AstNode** out) {
    AstNode* left = NULL;
    unwrap(bool_or(parser, &left));

    if (parser->current_token.type != TOKEN_EQUALS) {
        *out = left;
        return result_ok();
    }

    unwrap(eat(parser, TOKEN_EQUALS));

    switch (left->type) {
        
        case AST_IDENTIFIER:
            unwrap(ast_new((AstNode){.type=AST_ASSIGNMENT,.assignment={.left=left,.right=NULL}}, out));
            return factor(parser, &(*out)->assignment.right);

        default:
            ERROR_ARGS("Parser", "Assignment for %s is not implemented yet\n", ast_type_to_str(left->type));
            return result_error("assignment unimplemented");
    }

    assert(0 && "Unreachable");
}

Result before(Parser* parser, AstNode** out) {
    return after(parser, out);
}

Result after(Parser* parser, AstNode** out) {
    return factor(parser, out);
}

Result factor(Parser* parser, AstNode** out) {
    switch (parser->current_token.type) {
        
        case TOKEN_ID:
        case TOKEN_STRING:
            unwrap(string(parser, out));
            break;

        case TOKEN_INT:
            unwrap(int_(parser, out));
            break;

        case TOKEN_FLOAT:
            unwrap(float_(parser, out));
            break;

        case TOKEN_LPAREN:
            unwrap(tuple(parser, out));
            return result_ok(); // bc we have already eaten the ) inside tuple
            break;

        default:
            ERROR_LOG("Syntax", "unexpected token");
            token_print(parser->current_token);
            return result_error("unexpected token");
    }

    return eat(parser, parser->current_token.type);
}

Result string(Parser* parser, AstNode** out) {
    return ast_new((AstNode){
        .type=(parser->current_token.type == TOKEN_STRING ? AST_STRING : AST_IDENTIFIER),
        .string=strdup(parser->current_token.value)
    }, out);
}

Result int_(Parser* parser, AstNode** out) {
    return ast_new((AstNode){
        .type=AST_INT,
        .int_=strtoi64(parser->current_token.value, NULL, 10)
    }, out);
}

Result float_(Parser* parser, AstNode** out) {
    return ast_new((AstNode){
        .type=AST_FLOAT,
        .float_=strtod(parser->current_token.value, NULL)
    }, out);
}

Result tuple(Parser* parser, AstNode** out) {
    AstNode* tup = NULL;
    
    unwrap(body(parser, &tup, tuple_delims, true));
    
    if (tup->type == AST_UNKNOWN) {
        tup->type = AST_TUPLE;
        *out = tup;
    } 
    else {
        // If not unknown then it's not a tuple
        *out = tup->tuple.items[0];
        DA_FREE(tup->tuple);
        kod_free(tup);
    }
    
    return result_ok();
}

Result list(Parser* parser, AstNode** out) {
    return body(parser, out, list_delims, true);
}

Result block(Parser* parser, AstNode** out) {
    return body(parser, out, block_delims, false);
}

Result body(Parser* parser, AstNode** out, delims_t delims, bool commas) {
    unwrap(ast_new((AstNode){.type=AST_UNKNOWN}, out));

    unwrap(eat(parser, delims[LEFT_DELIM]));

    skip_newlines(parser);

    while (parser->current_token.type != delims[RIGHT_DELIM]) {
        AstNode* value = NULL;

        unwrap(expression(parser, &value));
        skip_newlines(parser);

        if (!value) continue;

        DA_APPEND((*out)->list, value);

        if (commas) {
            if (parser->current_token.type != TOKEN_COMMA) {
                // special case for tuple (x)
                if ((*out)->list.count == 1) {
                    (*out)->type = (*out)->list.items[0]->type;
                }
                break;
            }
            unwrap(eat(parser, TOKEN_COMMA));
        }
    }

    return eat(parser, delims[RIGHT_DELIM]);
}