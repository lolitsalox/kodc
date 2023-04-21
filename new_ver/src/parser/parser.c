#include "parser.h"

static enum STATUS eat(Parser* parser, TokenType type, char** err);
static enum STATUS skip_newlines(Parser* parser, char** err);

static enum STATUS parse_compound(Parser* parser, AstNode** out, char** err);

// static enum STATUS parse_body(
//     Parser* parser, 
//     AstType type, 
//     TokenType left_delimiter, 
//     TokenType right_delimiter,
//     bool parse_comma, AstNode** out, char** err
// );
// static enum STATUS parse_list    (Parser* parser, bool lambda, AstNode** out, char** err); // (...) or |...|
// static enum STATUS parse_block   (Parser* parser, AstNode** out, char** err); // {...}
// static enum STATUS parse_brackets(Parser* parser, AstNode** out, char** err); // [...]

// static enum STATUS parse_statement(Parser* parser, AstNode** out, char** err);

static enum STATUS parse_expression     (Parser* parser, AstNode** out, char** err);
static enum STATUS parse_assignment     (Parser* parser, AstNode** out, char** err);             // = -> += -=, etc...
static enum STATUS parse_bool_or        (Parser* parser, AstNode** out, char** err);             // ||
static enum STATUS parse_bool_and       (Parser* parser, AstNode** out, char** err);             // &&
static enum STATUS parse_bitwise_or     (Parser* parser, AstNode** out, char** err);             // |
// static enum STATUS parse_bitwise_xor    (Parser* parser, AstNode** out, char** err);             // ^
// static enum STATUS parse_bitwise_and    (Parser* parser, AstNode** out, char** err);             // &
// static enum STATUS parse_bool_equals    (Parser* parser, AstNode** out, char** err);             // == !=
// static enum STATUS parse_bool_gtlt      (Parser* parser, AstNode** out, char** err);             // < > <= >=
// static enum STATUS parse_bitwise_shlr   (Parser* parser, AstNode** out, char** err);             // << >>
// static enum STATUS parse_add_sub        (Parser* parser, AstNode** out, char** err);             // + -
// static enum STATUS parse_mul_div_mod    (Parser* parser, AstNode** out, char** err);             // * / %
// static enum STATUS parse_pow            (Parser* parser, AstNode** out, char** err);             // **       
// static enum STATUS parse_before         (Parser* parser, AstNode** out, char** err);             // + - ! ~ @ # sizeof       
// static enum STATUS parse_after          (Parser* parser, AstNode* value, AstNode** out, char** err);  // from the right () [] . 
static enum STATUS parse_factor         (Parser* parser, AstNode** out, char** err);             // values 

Parser parser_init(Lexer* lexer) {
    return (Parser) {
        .lexer=lexer,
        .current_token=NULL,
        .getting_params=false
    };
}

enum STATUS parse(Parser* parser, AstNode** out, char** err) {
    if (!parser || !parser->lexer) {
        *err = "The Parser and/or the Lexer is null.";
        return STATUS_FAIL;
    }

    if (lexer_get_next_token(parser->lexer, &parser->current_token, err) == STATUS_FAIL) {
        return STATUS_FAIL;
    }

    enum STATUS status = parse_compound(parser, out, err);
    if (*out)
        (*out)->type = AST_ROOT;
    return status;
}

static enum STATUS eat(Parser* parser, TokenType type, char** err) {
    if (parser->current_token->type != type) {
        ERROR_ARGS("Parser", "Unexpected token at %d:%d",
            parser->current_token->row, 
            parser->current_token->column
        );

        if (parser->current_token->value)
            printf(" - (%s) `%s`", 
                token_type_to_str(parser->current_token->type),
                parser->current_token->value 
            );
        
        printf(", expected (%s).\n", 
            token_type_to_str(type)
        );
        *err = "Unexpected token";
        return STATUS_FAIL;
    }

    token_free(parser->current_token);
    return lexer_get_next_token(parser->lexer, &parser->current_token, err);
}

static enum STATUS skip_newlines(Parser* parser, char** err) {
    while (parser->current_token->type == TOKEN_NL) 
        if (eat(parser, TOKEN_NL, err) == STATUS_FAIL) {
            return STATUS_FAIL;
        }
    return STATUS_OK;
}

static enum STATUS parse_compound(Parser* parser, AstNode** out, char** err) {
    if (ast_node_new(((AstNode){.type=AST_COMPOUND}), out, err) == STATUS_FAIL) {
        return STATUS_FAIL;
    }
    
    while (parser->current_token->type != TOKEN_EOF) {
        AstNode* value = NULL;
        
        if (skip_newlines(parser, err) == STATUS_FAIL) return STATUS_FAIL;
        if (parse_expression(parser, &value, err) == STATUS_FAIL) return STATUS_FAIL;

        if (!value) continue;

        if (ast_list_append(&(*out)->_list, value, err) == STATUS_FAIL) return STATUS_FAIL;
    }

    return STATUS_OK;
}

static enum STATUS parse_expression(Parser* parser, AstNode** out, char** err) {
    return parse_assignment(parser, out, err);
}

static enum STATUS parse_assignment(Parser* parser, AstNode** out, char** err) {
    // Parse the left-hand side of the assignment.
    AstNode* left = NULL;
    if (parse_bool_or(parser, &left, err) == STATUS_FAIL) return STATUS_FAIL;

    // If the current token is not an equals sign, return the left-hand side.
    if (parser->current_token->type != TOKEN_EQUALS) {
        *out = left;
        return STATUS_OK;
    }

    // Eat the equals sign.
    if (eat(parser, TOKEN_EQUALS, err) == STATUS_FAIL) return STATUS_FAIL;

    switch (left->type) {
        case AST_IDENTIFIER: {
            if (ast_node_new(
                ((AstNode){
                .type=AST_ASSIGNMENT, 
                ._assignment={
                    .left=left, 
                    .right=NULL
                }
                }), 
                out, 
                err
            ) == STATUS_FAIL) {
                return STATUS_FAIL;
            };
            if (parse_assignment(parser, &(*out)->_assignment.right, err) == STATUS_FAIL)
                return STATUS_FAIL;
            
            return STATUS_OK;
        }

        case AST_ACCESS: {
            if (ast_node_new(
                ((AstNode){
                    .type=AST_STORE_ATTR, 
                    ._store_attr={
                        .left=left, 
                        .right=NULL
                    },
                }), 
                out, 
                err
            ) == STATUS_FAIL) {
                return STATUS_FAIL;
            };

            if (parse_assignment(parser, &(*out)->_store_attr.right, err) == STATUS_FAIL)
                return STATUS_FAIL;
            *out = left;
            return STATUS_OK;
        }

        default: ERROR_ARGS("Parser", "Assignment for %s is not implemented yet\n", 
            ast_type_to_str(left->type));
            UNIMPLEMENTED
            *err = "Unimplemented assignment"; 
            return STATUS_FAIL;
    }

    // unreachable
    *err = "Unreachable got reached...";
    return STATUS_FAIL;
}

static enum STATUS parse_bool_or(Parser* parser, AstNode** out, char** err) {
    AstNode* left = NULL;
    if (parse_bool_and(parser, &left, err) == STATUS_FAIL) return STATUS_FAIL;

    if (parser->current_token->type == TOKEN_BOOL_OR) {
        if (ast_node_new(((AstNode){
            .type=AST_BIN_OP, 
            ._binary_op={
                .left=left, 
                .right=NULL,
                .op=parser->current_token->type,
            }
        }), &left, err) == STATUS_FAIL) return STATUS_FAIL;

        // Eating the token
        if (eat(parser, parser->current_token->type, err) == STATUS_FAIL) return STATUS_FAIL; 
        if (parse_bool_or(parser, &left->_binary_op.right, err) == STATUS_FAIL) return STATUS_FAIL;
    }

    *out = left;
    return STATUS_OK;
} 

static enum STATUS parse_bool_and(Parser* parser, AstNode** out, char** err) {
    AstNode* left = NULL;
    if (parse_bitwise_or(parser, &left, err) == STATUS_FAIL) return STATUS_FAIL;

    if (parser->current_token->type == TOKEN_BOOL_OR) {
        if (ast_node_new(((AstNode){
            .type=AST_BIN_OP, 
            ._binary_op={
                .left=left, 
                .right=NULL,
                .op=parser->current_token->type,
            }
        }), &left, err) == STATUS_FAIL) return STATUS_FAIL;

        // Eating the token
        if (eat(parser, parser->current_token->type, err) == STATUS_FAIL) return STATUS_FAIL; 
        if (parse_bool_and(parser, &left->_binary_op.right, err) == STATUS_FAIL) return STATUS_FAIL;
    }

    *out = left;
    return STATUS_OK;
}

static enum STATUS parse_bitwise_or(Parser* parser, AstNode** out, char** err) {
    AstNode* left = NULL;
    if (parse_factor(parser, &left, err) == STATUS_FAIL) return STATUS_FAIL;

    if (parser->current_token->type == TOKEN_BOOL_OR) {
        if (ast_node_new(((AstNode){
            .type=AST_BIN_OP, 
            ._binary_op={
                .left=left, 
                .right=NULL,
                .op=parser->current_token->type,
            }
        }), &left, err) == STATUS_FAIL) return STATUS_FAIL;

        // Eating the token
        if (eat(parser, parser->current_token->type, err) == STATUS_FAIL) return STATUS_FAIL; 
        if (parse_bool_and(parser, &left->_binary_op.right, err) == STATUS_FAIL) return STATUS_FAIL;
    }

    *out = left;
    return STATUS_OK;
}

static enum STATUS parse_factor(Parser* parser, AstNode** out, char** err) {
    parse_factor_start:
    if (!parser->current_token) {
        *err = "Current token is null";
        return STATUS_FAIL;
    }

    switch (parser->current_token->type) {
        // If the token is a string literal.
        case TOKEN_STRING:
            // Create a new AST node for the string literal.
            if (ast_node_new(((AstNode){
                .type = AST_STRING,
                ._string = parser->current_token->value
            }), out, err) == STATUS_FAIL) return STATUS_FAIL; 
            break;

        // If the token is a numeric literal.
        case TOKEN_FLOAT:
            // Create a new AST node for the float literal.
            if (ast_node_new(((AstNode){
                .type = AST_FLOAT,
                ._float = strtod(parser->current_token->value, NULL)
            }), out, err) == STATUS_FAIL) return STATUS_FAIL;
            break;

        case TOKEN_INT:
            // Create a new AST node for the numeric literal.
            if (ast_node_new(((AstNode){
                .type = AST_INT,
                ._int = _strtoi64(parser->current_token->value, NULL, 10)
            }), out, err) == STATUS_FAIL) return STATUS_FAIL;
            break;

        // If the token is an identifier.
        case TOKEN_ID:
            // Create a new AST node for the identifier.
            if (ast_node_new(((AstNode){
                .type = AST_IDENTIFIER,
                ._string = parser->current_token->value
            }), out, err) == STATUS_FAIL) return STATUS_FAIL; 
            break;
        
        // If the token is a keyword.
        case TOKEN_KEYWORD:
            UNIMPLEMENTED;
            *err = "Unimplemented";
            return STATUS_FAIL;
            // return parse_statement(parser, out, err);

        // Case of a list
        case TOKEN_LPAREN: {
            // Parse a list of expressions inside the parentheses.
            AstNode* list = NULL;
            UNIMPLEMENTED;
            *err = "Unimplemented";
            return STATUS_FAIL;
            // if (parse_list(parser, false, &list, err) == STATUS_FAIL) return STATUS_FAIL;
            // if (list->_list.size == 1) {
            //     *out = list->_list.head->node;
            //     free(list->_list.head); // freeing the node
            //     free(list);
            //     return STATUS_OK;
            // }
            *out = list;
            return STATUS_OK;
        }

        // Case of a lambda |x, ...| {...}
        case TOKEN_OR: {

            // Parse a list of expressions inside the parentheses.
            // AstNode* list = NULL;
            
            UNIMPLEMENTED;
            *err = "Unimplemented";
            return STATUS_FAIL;
            parser->getting_params = true;
            // if (parse_list(parser, true, &list, err) == STATUS_FAIL) return STATUS_FAIL;
            parser->getting_params = false;

            // Parse a block of expressions to serve as the function body.
            // AstNode* block = NULL;
            UNIMPLEMENTED;
            *err = "Unimplemented";
            return STATUS_FAIL;
            // if (parse_block(parser, &block, err) == STATUS_FAIL) return STATUS_FAIL;
            
            char* name = malloc(sizeof("<anonymous>"));
            strcpy(name, "<anonymous>");

            // Create a new AST node for the function.
            // if (ast_node_new(((AstNode){
            //         .type=AST_FUNCTION,
            //         .ast_function={
            //             .name=name, // The function has no name.
            //             .parameters=list, // The function parameters come from the list we just parsed.
            //             .body=block, // The function body is the block of expressions we just parsed.
            //         },
            //     }),
            //     out, err
            // ) == STATUS_FAIL) return STATUS_FAIL;
            // return STATUS_OK;
        }
        
        // Case of a block
        case TOKEN_LBRACE: {
            // Parse a list of expressions inside the parentheses.
            UNIMPLEMENTED;
            *err = "Unimplemented";
            return STATUS_FAIL;
            // return parse_block(parser, out, err);
        }

        case TOKEN_LINE_COMMENT:
            while (parser->current_token->type != TOKEN_NL &&
                   parser->current_token->type != TOKEN_EOF) {
                if (lexer_get_next_token(parser->lexer, &parser->current_token, err) == STATUS_FAIL) return STATUS_FAIL;
            }
            if (skip_newlines(parser, err) == STATUS_FAIL) return STATUS_FAIL;
            goto parse_factor_start;
            
        case TOKEN_MULTILINE_COMMENT_START:
            while (parser->current_token && parser->current_token->type != TOKEN_MULTILINE_COMMENT_END) {
                if (parser->current_token->type == TOKEN_EOF) {
                    ERROR("Parser", "Unexpected end of multi line comment (you forgot to close it with \"*/\")\n");
                    return STATUS_FAIL;
                }
                if (lexer_get_next_token(parser->lexer, &parser->current_token, err) == STATUS_FAIL) return STATUS_FAIL;
            }
            if (eat(parser, TOKEN_MULTILINE_COMMENT_END, err) == STATUS_FAIL) return STATUS_FAIL;
            if (skip_newlines(parser, err) == STATUS_FAIL) return STATUS_FAIL;
            goto parse_factor_start;

        // end of expression or whatever
        case TOKEN_NL: 
        case TOKEN_SEMI: 
        case TOKEN_EOF: 
            *out = NULL;
            break;
        
        // anything else
        default:
            printf("[parser]: Error - unexpected token ");
            token_print(parser->current_token);
            exit(1);
    }

    if (eat(parser, parser->current_token->type, err) == STATUS_FAIL) return STATUS_FAIL;
    return STATUS_OK;
}