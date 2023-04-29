#include "parser.h"

static enum STATUS eat(Parser* parser, TokenType type, char** err);
static enum STATUS skip_newlines(Parser* parser, char** err);

static enum STATUS parse_compound(Parser* parser, AstNode** out, char** err);

static enum STATUS parse_body(
    Parser* parser, 
    AstType type, 
    TokenType left_delimiter, 
    TokenType right_delimiter,
    bool parse_comma, 
    AstNode** out, char** err
);

static enum STATUS parse_list    (Parser* parser, AstNode** out, char** err, AstType type); // (...) or |...|
static enum STATUS parse_block   (Parser* parser, AstNode** out, char** err); // {...}

static enum STATUS parse_statement(Parser* parser, AstNode** out, char** err);

static enum STATUS parse_binary         (Parser* parser, AstNode** out, char** err, TokenType* types, size_t types_size, enum STATUS (*next)(Parser* parser, AstNode** out, char** err));

static enum STATUS parse_expression     (Parser* parser, AstNode** out, char** err);
static enum STATUS parse_assignment     (Parser* parser, AstNode** out, char** err);             // = -> += -=, etc...
static enum STATUS parse_bool_or        (Parser* parser, AstNode** out, char** err);             // ||
static enum STATUS parse_bool_and       (Parser* parser, AstNode** out, char** err);             // &&
static enum STATUS parse_bitwise_or     (Parser* parser, AstNode** out, char** err);             // |
static enum STATUS parse_bitwise_xor    (Parser* parser, AstNode** out, char** err);             // ^
static enum STATUS parse_bitwise_and    (Parser* parser, AstNode** out, char** err);             // &
static enum STATUS parse_bool_equals    (Parser* parser, AstNode** out, char** err);             // == !=
static enum STATUS parse_bool_gtlt      (Parser* parser, AstNode** out, char** err);             // < > <= >=
static enum STATUS parse_bitwise_shlr   (Parser* parser, AstNode** out, char** err);             // << >>
static enum STATUS parse_add_sub        (Parser* parser, AstNode** out, char** err);             // + -
static enum STATUS parse_mul_div_mod    (Parser* parser, AstNode** out, char** err);             // * / %
static enum STATUS parse_pow            (Parser* parser, AstNode** out, char** err);             // **       
static enum STATUS parse_before         (Parser* parser, AstNode** out, char** err);             // + - ! ~ @ # sizeof       
static enum STATUS parse_after          (Parser* parser, AstNode* value, AstNode** out, char** err);  // from the right () [] . 
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

    token_free(parser->current_token);
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
    parser->current_token = NULL;
    
    return lexer_get_next_token(parser->lexer, &parser->current_token, err);
}

static enum STATUS skip_newlines(Parser* parser, char** err) {
    while (parser->current_token->type == TOKEN_NL) 
        if (eat(parser, TOKEN_NL, err) == STATUS_FAIL) {
            return STATUS_FAIL;
        }
    return STATUS_OK;
}

static enum STATUS parse_body(
    Parser* parser, 
    AstType type, 
    TokenType left_delimiter, 
    TokenType right_delimiter,
    bool parse_comma, 
    AstNode** out, char** err
) {
    if (ast_node_new(((AstNode){.type=type}), out, err) == STATUS_FAIL) {
        return STATUS_FAIL;
    }

    if (eat(parser, left_delimiter, err) == STATUS_FAIL) return STATUS_FAIL;

    if (skip_newlines(parser, err) == STATUS_FAIL) return STATUS_FAIL;

    while (parser->current_token->type != right_delimiter) {
        AstNode* value = NULL;

        if (skip_newlines(parser, err) == STATUS_FAIL) return STATUS_FAIL;

        if (parser->getting_params) {
            if (parse_factor(parser, &value, err) == STATUS_FAIL) return STATUS_FAIL;
        }
        else if (parse_expression(parser, &value, err) == STATUS_FAIL) return STATUS_FAIL;
        
        if (!value) continue;
        
        if (ast_list_append(&(*out)->_list, value, err) == STATUS_FAIL) return STATUS_FAIL;
        
        if (type == AST_BLOCK && parser->current_token->type == TOKEN_SEMI)
            if (eat(parser, TOKEN_SEMI, err) == STATUS_FAIL) return STATUS_FAIL;
        
        if (skip_newlines(parser, err) == STATUS_FAIL) return STATUS_FAIL;

        if (parse_comma) {
            if (parser->current_token->type == TOKEN_COMMA) {
                if (eat(parser, TOKEN_COMMA, err) == STATUS_FAIL) return STATUS_FAIL;
            }
            else break;
        }
    }

    if (eat(parser, right_delimiter, err) == STATUS_FAIL) return STATUS_FAIL;
    return STATUS_OK;
}

static enum STATUS parse_compound(Parser* parser, AstNode** out, char** err) {
    if (ast_node_new(((AstNode){.type=AST_COMPOUND}), out, err) == STATUS_FAIL) {
        return STATUS_FAIL;
    }
    
    while (parser->current_token->type != TOKEN_EOF) {
        AstNode* value = NULL;
        
        if (skip_newlines(parser, err) == STATUS_FAIL) return STATUS_FAIL;
        if (parser->current_token->type == TOKEN_EOF) break;
        
        if (parse_expression(parser, &value, err) == STATUS_FAIL) return STATUS_FAIL;
        if (!value) continue;

        if (ast_list_append(&(*out)->_list, value, err) == STATUS_FAIL) return STATUS_FAIL;
    }

    return STATUS_OK;
}

static bool is_type_in_types(TokenType type, TokenType* types, size_t types_size) {
    for (size_t i = 0; i < types_size; ++i) {
        if (type == types[i]) return true;
    }
    return false;
}

static enum STATUS parse_binary(
    Parser* parser, 
    AstNode** out, 
    char** err, 
    TokenType* types, 
    size_t types_size, 
    enum STATUS (*next)(Parser* parser, AstNode** out, char** err)
) {    
    AstNode* left = NULL;
    
    if (next(parser, &left, err) == STATUS_FAIL) return STATUS_FAIL;

    if (is_type_in_types(parser->current_token->type, types, types_size)) {
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
        if (parse_binary(parser, &left->_binary_op.right, err, types, types_size, next) == STATUS_FAIL) return STATUS_FAIL;
    }

    *out = left;
    return STATUS_OK;
}

static enum STATUS parse_list(Parser* parser, AstNode** out, char** err, AstType type) {
    TokenType left, right;

    switch (type) {
        case AST_TUPLE:     left = TOKEN_LPAREN; right = TOKEN_RPAREN; break; 
        case AST_LAMBDA:    left = TOKEN_OR; right = TOKEN_OR; break; 
        case AST_LIST:      left = TOKEN_LBRACKET; right = TOKEN_RBRACKET; break; 
        default: *err = "Unknown type for parsing a list."; return STATUS_FAIL;
    }

    return parse_body(parser, type == AST_LIST ? type : AST_TUPLE, left, right, true, out, err);
}

static enum STATUS parse_block(Parser* parser, AstNode** out, char** err) {
    return parse_body(parser, AST_BLOCK, TOKEN_LBRACE, TOKEN_RBRACE, false, out, err);
}

static enum STATUS parse_statement(Parser* parser, AstNode** out, char** err) {
    KeywordType ktype = parser->current_token->keyword_type;
    if (eat(parser, TOKEN_KEYWORD, err) == STATUS_FAIL) return STATUS_FAIL;

    switch (ktype) {
        case KEYWORD_IF:
        case KEYWORD_WHILE:
            if (ast_node_new((AstNode){
                .type=(ktype == KEYWORD_IF ? AST_IF_STATEMENT : AST_WHILE_STATEMENT),
                ._conditional={
                    .expr=NULL,
                    .body=NULL
                }
            }, out, err) == STATUS_FAIL) return STATUS_FAIL;

            if (parse_expression(parser, &(*out)->_conditional.expr, err) == STATUS_FAIL) return STATUS_FAIL;
            if (parse_block(parser, &(*out)->_conditional.body, err) == STATUS_FAIL) return STATUS_FAIL;
            break;

        case KEYWORD_RETURN:
            if (ast_node_new((AstNode){
                .type=AST_RETURN_STATEMENT,
                ._return=NULL
            }, out, err) == STATUS_FAIL) return STATUS_FAIL;
            if (parse_expression(parser, &(*out)->_return, err) == STATUS_FAIL) return STATUS_FAIL;
            break;

        case KEYWORD_NULL:
            if (ast_node_new((AstNode) {
                .type = AST_NULL,
            }, out, err) == STATUS_FAIL) return STATUS_FAIL;
            break;

        case KEYWORD_FALSE:
        case KEYWORD_TRUE:
            if (ast_node_new((AstNode) {
                .type = AST_BOOL,
                ._bool = ktype == KEYWORD_TRUE
            }, out, err) == STATUS_FAIL) return STATUS_FAIL;
            break;

        default: 
            ERROR_ARGS("Parser", "'%s' keyword hasn't been implemented yet.\n", keyword_type_to_str(ktype));
            *err = "Unimplemented keyword"; 
            return STATUS_FAIL;
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
    TokenType types[] = {TOKEN_BOOL_OR};
    return parse_binary(parser, out, err, types, ARRAYSIZE(types), parse_bool_and);
} 

static enum STATUS parse_bool_and(Parser* parser, AstNode** out, char** err) {
    TokenType types[] = {TOKEN_BOOL_AND};
    return parse_binary(parser, out, err, types, ARRAYSIZE(types), parse_bitwise_or);
}

static enum STATUS parse_bitwise_or(Parser* parser, AstNode** out, char** err) {
    TokenType types[] = {TOKEN_OR};
    return parse_binary(parser, out, err, types, ARRAYSIZE(types), parse_bitwise_xor);
}

static enum STATUS parse_bitwise_xor(Parser* parser, AstNode** out, char** err) {
    TokenType types[] = {TOKEN_HAT};
    return parse_binary(parser, out, err, types, ARRAYSIZE(types), parse_bitwise_and);
}

static enum STATUS parse_bitwise_and(Parser* parser, AstNode** out, char** err) {
    TokenType types[] = {TOKEN_AND};
    return parse_binary(parser, out, err, types, ARRAYSIZE(types), parse_bool_equals);
}

static enum STATUS parse_bool_equals(Parser* parser, AstNode** out, char** err) {
    TokenType types[] = {TOKEN_BOOL_EQ, TOKEN_BOOL_NE};
    return parse_binary(parser, out, err, types, ARRAYSIZE(types), parse_bool_gtlt);
}

static enum STATUS parse_bool_gtlt      (Parser* parser, AstNode** out, char** err) {
    TokenType types[] = {TOKEN_BOOL_GT, TOKEN_BOOL_GTE, TOKEN_BOOL_LT, TOKEN_BOOL_LTE};
    return parse_binary(parser, out, err, types, ARRAYSIZE(types), parse_bitwise_shlr);
}

static enum STATUS parse_bitwise_shlr(Parser* parser, AstNode** out, char** err) {
    TokenType types[] = {TOKEN_SHL, TOKEN_SHR};
    return parse_binary(parser, out, err, types, ARRAYSIZE(types), parse_add_sub);
}

static enum STATUS parse_add_sub(Parser* parser, AstNode** out, char** err) {
    TokenType types[] = {TOKEN_ADD, TOKEN_SUB};
    return parse_binary(parser, out, err, types, ARRAYSIZE(types), parse_mul_div_mod);
}

static enum STATUS parse_mul_div_mod(Parser* parser, AstNode** out, char** err) {
    TokenType types[] = {TOKEN_MUL, TOKEN_DIV, TOKEN_MOD};
    return parse_binary(parser, out, err, types, ARRAYSIZE(types), parse_pow);
}

static enum STATUS parse_pow(Parser* parser, AstNode** out, char** err) {
    TokenType types[] = {TOKEN_POW};
    return parse_binary(parser, out, err, types, ARRAYSIZE(types), parse_before);
}

static enum STATUS parse_before(Parser* parser, AstNode** out, char** err) {
    if (parser->current_token->type == TOKEN_ADD ||
        parser->current_token->type == TOKEN_SUB ||
        parser->current_token->type == TOKEN_NOT ||
        parser->current_token->type == TOKEN_BOOL_NOT
    ) {
        TokenType type = parser->current_token->type;
        if (eat(parser, type, err) == STATUS_FAIL) return STATUS_FAIL;

        AstNode* value = NULL;
        if (parse_before(parser, &value, err) == STATUS_FAIL) return STATUS_FAIL;

        if (value->type == AST_INT) {
            *out = value;
            switch (type) {
                case TOKEN_ADD: return STATUS_OK;
                case TOKEN_SUB: value->_int *= -1; return STATUS_OK;
                case TOKEN_NOT: value->_int = ~((int64_t)value->_int); return STATUS_OK;
                case TOKEN_BOOL_NOT: value->_int = !value->_int; return STATUS_OK;
                default: break;
            }
        } else if (value->type == AST_FLOAT) {
            *out = value;
            switch (type) {
                case TOKEN_ADD: return STATUS_OK;
                case TOKEN_SUB: value->_float *= -1; return STATUS_OK;
                case TOKEN_BOOL_NOT: value->_float = !value->_float; return STATUS_OK;
                case TOKEN_NOT: 
                    printf("[parser] - Error: can't use ~ on a float.\n"); 
                    *err = "can't use ~ on a float."; 
                    return STATUS_FAIL;
                default: break;
            }
        }

        AstNode* node = NULL;
        
        if (ast_node_new((AstNode){
            .type=AST_UNARY_OP, 
            ._unary_op={
                .value=value,
                .op=type
            }}, 
            &node, 
            err
        ) == STATUS_FAIL) return STATUS_FAIL;

        return parse_after(parser, node, out, err);
    }
        
    return parse_after(parser, NULL, out, err);
}

static enum STATUS parse_after(Parser* parser, AstNode* value, AstNode** out, char** err) {
    // If 'value' is null, set it to the result of parsing a factor.
    if (!value) {
        if (parse_factor(parser, &value, err) == STATUS_FAIL) return STATUS_FAIL;
    }
    *out = value;
    // Check the type of the current token and perform an action based on the type.
    switch (parser->current_token->type) {
        // Function definition/call
        case TOKEN_LPAREN: {
            // Parse a list of arguments/parameters.
            AstNode* list = NULL;
            if (parse_list(parser, &list, err, AST_TUPLE) == STATUS_FAIL) return STATUS_FAIL;

            // If the next token is a left brace, this is a function definition.
            if (parser->current_token->type == TOKEN_LBRACE) {
                
                // Ensure that the value being defined is an identifier.
                if (value->type != AST_IDENTIFIER) {
                    printf("[parser]: Error - invalid syntax (can't define a function like that bro)\n");
                    *err = "invalid syntax (can't define a function like that bro)";
                    return STATUS_FAIL;
                }

                // Create a new function AST node and return it.
                AstNode* func = NULL;
                if (ast_node_new((AstNode){
                    .type=AST_FUNCTION,
                    ._function={
                        .name=value->_string,
                        .params=list,
                        .body=NULL,
                    }
                }, &func, err) == STATUS_FAIL) return STATUS_FAIL;

                if (parse_block(parser, &func->_function.body, err) == STATUS_FAIL) return STATUS_FAIL;
                func->_function.body->parent = func;
                // Freeing the value node (not the string) affects the system
                // free(value);
                size_t s_size = strlen(value->_string) + 1;
                func->_function.name = malloc(s_size);
                memcpy(func->_function.name, value->_string, s_size);
                
                // printf("freeing identifier\n");
                ast_free(value);
                // printf("freed identifier\n");
                *out = func;
                return STATUS_OK;
            }

            // if (value->type == AST_ACCESS) {
            //     // This is a method call.

            //     ast_node_t* method_call = ast_node_new((ast_node_t){
            //         .ast_type=AST_METHOD_CALL,
            //         .ast_method_call={
            //             .callable=value->ast_access.field,
            //             .arguments=list,
            //             .this=value->ast_access.value
            //         }
            //     });

            //     return parse_after(parser, method_call);
            // }

            // This is a function call.
            AstNode* function_call = NULL;
            
            if (ast_node_new((AstNode){
                .type=AST_CALL,
                ._call={
                    .callable=value,
                    .args=list
                }
            }, &function_call, err) == STATUS_FAIL) return STATUS_FAIL;

            return parse_after(parser, function_call, out, err);
        }

    //     // Array subscript
    //     case TOKEN_LBRACKET: {
    //         // Parse the subscript expression inside the brackets.
    //         ast_node_t* subscript = parse_brackets(parser);
    //         // Create a new AST node for the array subscript and return it.
    //         ast_node_t* array_subscript = ast_node_new((ast_node_t){
    //             .ast_type=AST_SUBSCRIPT,
    //             .ast_subscript={
    //                 .value=value,
    //                 .subscript=subscript
    //             }
    //         });

    //         return parse_after(parser, array_subscript);
    //     }

    //     // Class access expression (e.g. expr.id)
    //     case TOKEN_DOT: {
    //         // Eat the dot token and parse the field expression.
    //         eat(parser, TOKEN_DOT);
    //         ast_node_t* field = parse_factor(parser);
    //         // Ensure that the field is an identifier.
    //         if (field->ast_type != AST_IDENTIFIER) {
    //             printf("[parser]: Error - invalid syntax (can't get a field that is an expression)\n");
    //             exit(1);
    //         }

    //         // Create a new AST node for the class access expression and return it.
    //         ast_node_t* class_access = ast_node_new((ast_node_t){
    //             .ast_type=AST_ACCESS,
    //             .ast_access={
    //                 .value=value,
    //                 .field=field
    //             }
    //         });

    //         return parse_after(parser, class_access);
    //     }

    //     // Default case: do nothing.
        default: break;
    }

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
                ._string = NULL
            }), out, err) == STATUS_FAIL) return STATUS_FAIL;
            
            (*out)->_string = malloc(sizeof(char) * (parser->current_token->length + 1));
            memcpy((*out)->_string, parser->current_token->value, parser->current_token->length + 1);
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
                ._int = strtoi64(parser->current_token->value, NULL, 10)
            }), out, err) == STATUS_FAIL) return STATUS_FAIL;
            break;

        // If the token is an identifier.
        case TOKEN_ID:
            // Create a new AST node for the identifier.
            if (ast_node_new(((AstNode){
                .type = AST_IDENTIFIER,
                ._string = NULL
            }), out, err) == STATUS_FAIL) return STATUS_FAIL; 

            (*out)->_string = malloc(sizeof(char) * (parser->current_token->length + 1));
            memcpy((*out)->_string, parser->current_token->value, parser->current_token->length + 1);
            break;
        
        // If the token is a keyword.
        case TOKEN_KEYWORD:
            return parse_statement(parser, out, err);

        // Case of a list
        case TOKEN_LPAREN: { 
            // Parse a list of expressions inside the parentheses.
            AstNode* list = NULL;
            if (parse_list(parser, &list, err, AST_TUPLE) == STATUS_FAIL) return STATUS_FAIL;

            if (list->_list.size == 1) {
                *out = list->_list.head->node;
                free(list->_list.head);
                free(list);
            }
            else *out = list;
            return STATUS_OK;
        }

        // Case of a lambda |x, ...| {...}
        case TOKEN_OR: {

            // Parse a list of expressions inside the parentheses.
            AstNode* list = NULL;
            parser->getting_params = true;
            if (parse_list(parser, &list, err, AST_LAMBDA) == STATUS_FAIL) return STATUS_FAIL;
            parser->getting_params = false;

            // Parse a block of expressions to serve as the function body.
            AstNode* block = NULL;
            if (parse_block(parser, &block, err) == STATUS_FAIL) return STATUS_FAIL;
            
            // Create a new AST node for the function.
            if (ast_node_new(((AstNode){
                    .type=AST_LAMBDA,
                    ._lambda={
                        .params=list, // The function parameters come from the list we just parsed.
                        .body=block, // The function body is the block of expressions we just parsed.
                    },
                }),
                out, err
            ) == STATUS_FAIL) return STATUS_FAIL;

            block->parent = *out;
            return STATUS_OK;
        }
        
        // Case of a block
        case TOKEN_LBRACE: {
            // Parse a list of expressions inside the parentheses.
            return parse_block(parser, out, err);
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
            *err = "Unexpected token";
            return STATUS_FAIL;
    }

    if (eat(parser, parser->current_token->type, err) == STATUS_FAIL) return STATUS_FAIL;
    return STATUS_OK;
}