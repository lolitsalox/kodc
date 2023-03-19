#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

static void eat(parser_t* parser, token_type_t type);
static void skip_newlines(parser_t* parser);

static ast_node_t* parse_compound(parser_t* parser);

static ast_node_t* parse_list    (parser_t* parser, bool lambda);
static ast_node_t* parse_block   (parser_t* parser);
static ast_node_t* parse_brackets(parser_t* parser);

static ast_node_t* parse_statement(parser_t* parser);

static ast_node_t* parse_expression     (parser_t* parser);
static ast_node_t* parse_assignment     (parser_t* parser);             // = -> += -=, etc...
static ast_node_t* parse_bool_or        (parser_t* parser);             // ||
static ast_node_t* parse_bool_and       (parser_t* parser);             // &&
static ast_node_t* parse_bitwise_or     (parser_t* parser);             // |
static ast_node_t* parse_bitwise_xor    (parser_t* parser);             // ^
static ast_node_t* parse_bitwise_and    (parser_t* parser);             // &
static ast_node_t* parse_bool_equals    (parser_t* parser);             // == !=
static ast_node_t* parse_bool_gtlt      (parser_t* parser);             // < > <= >=
static ast_node_t* parse_bitwise_shlr   (parser_t* parser);             // << >>
static ast_node_t* parse_add_sub        (parser_t* parser);             // + -
static ast_node_t* parse_mul_div_mod    (parser_t* parser);             // * / %
static ast_node_t* parse_pow            (parser_t* parser);             // **       
static ast_node_t* parse_before         (parser_t* parser);             // + - ! ~ @ # sizeof       
static ast_node_t* parse_after          (parser_t* parser, ast_node_t* value);  // from the right () [] . 
static ast_node_t* parse_factor         (parser_t* parser);             // values 

parser_t parser_init(lexer_t* lexer) {
    if (!lexer) {
        printf("[parser]: Error - lexer is null\n");
        exit(1);
    }

    return (parser_t){.lexer=lexer, .current_token=lexer_get_next_token(lexer), .getting_parameters=false};
}

ast_node_t* parse(parser_t* parser) {
    ast_node_t* root = parse_compound(parser);
    root->ast_type = AST_ROOT;
    return root;
}

static void eat(parser_t* parser, token_type_t type) {
    if (parser->current_token && parser->current_token->token_type != type) {
        printf("[parser]: Error - unexpected token at %d:%d",
            parser->current_token->row, 
            parser->current_token->column
        );

        if (parser->current_token->value)
            printf(" - (%s) `%.*s`", 
                token_type_to_str(parser->current_token->token_type),
                parser->current_token->length, 
                parser->current_token->value 
            );
        
        printf(", expected (%s).\n", 
            token_type_to_str(type)
        );
        exit(1);
    }
    free(parser->current_token);
    parser->current_token = lexer_get_next_token(parser->lexer);
}

static void skip_newlines(parser_t* parser) {
    while (parser->current_token && parser->current_token->token_type == TOKEN_NL) 
        eat(parser, TOKEN_NL);
}

static ast_node_t* parse_compound(parser_t* parser) {
    ast_node_t* compound = ast_node_new(((ast_node_t){
        .ast_type=AST_COMPOUND
    }));
    
    while (parser->current_token && parser->current_token->token_type != TOKEN_EOF) {
        skip_newlines(parser);
        ast_node_t* value = parse_expression(parser);
        if (!value) continue;
        linked_list_append(&compound->ast_compound, value);
    }

    return compound;
}

/**
 * @brief Parses a block of code, list, or parameters.
 * 
 * @param parser the parser object
 * @param type the type of AST node to create
 * @param left_delimiter the left delimiter of the block
 * @param right_delimiter the right delimiter of the block
 * @param parse_comma whether to parse comma-separated items
 * @return the AST node representing the block
 */
static ast_node_t* parse_body(
    parser_t* parser, 
    ast_type_t type, 
    token_type_t left_delimiter, 
    token_type_t right_delimiter,
    bool parse_comma
) {
    // Create a new AST node for the block
    ast_node_t* body = ast_node_new(((ast_node_t){.ast_type=type}));
    linked_list_init(&body->ast_compound);

    // Eat the left delimiter of the block
    eat(parser, left_delimiter);
    
    // Skip any newlines before the first item
    skip_newlines(parser);

    // Parse each item in the block
    while (parser->current_token && parser->current_token->token_type != right_delimiter) {
        skip_newlines(parser);

        // Parse the item and add it to the block
        ast_node_t* value = parse_expression(parser);
        if (!value) continue;
        linked_list_append(&body->ast_compound, value);

        if (type == AST_BLOCK && parser->current_token && parser->current_token->token_type == TOKEN_SEMI)
            eat(parser, TOKEN_SEMI);

        // Skip any newlines after the item
        skip_newlines(parser);
        
        // If comma-separated items are expected, parse the comma if it exists, or stop parsing if it doesn't
        if (parse_comma) {
            if (parser->current_token && parser->current_token->token_type == TOKEN_COMMA)
                eat(parser, TOKEN_COMMA);
            else
                break;
        }
    }

    // Eat the right delimiter of the block
    eat(parser, right_delimiter);

    // Return the AST node representing the block
    return body;
}

// Parses a list of expressions inside parentheses and returns an AST node for it
static ast_node_t* parse_list(parser_t* parser, bool lambda) {
    return lambda ?
        parse_body(parser, AST_LIST, TOKEN_OR, TOKEN_OR, true) : 
        parse_body(parser, AST_LIST, TOKEN_LPAREN, TOKEN_RPAREN, true);
}

// Parses a block of expressions inside braces and returns an AST node for it
static ast_node_t* parse_block(parser_t* parser) {
    return parse_body(parser, AST_BLOCK, TOKEN_LBRACE, TOKEN_RBRACE, false);
}

// Parses a list of expressions inside brackets and returns an AST node for it
static ast_node_t* parse_brackets(parser_t* parser) {
    return parse_body(parser, AST_LIST, TOKEN_LBRACKET, TOKEN_RBRACKET, true);
}

// Parses a statement and returns an AST node for it
static ast_node_t* parse_statement(parser_t* parser) {
    if (!parser->current_token) return NULL;

    token_t tok = *parser->current_token;
    keyword_type_t keyword_type = tok.keyword_type;
    ast_node_t* ast_node = NULL;

    eat(parser, TOKEN_KEYWORD);


    switch (keyword_type) {
        case KEYWORD_IF: 
        case KEYWORD_WHILE: 
            ast_node = ast_node_new((ast_node_t){
                .ast_type=(keyword_type == KEYWORD_IF ? AST_IF_STATEMENT : AST_WHILE_STATEMENT),
                .ast_conditional_statement={
                    .expression=parse_expression(parser),
                    .body=parse_block(parser),
                },
            });
            break;
        
        // case KEYWORD_FOR: 
        //     break;
        
        case KEYWORD_RETURN: 
            ast_node = ast_node_new((ast_node_t){
                .ast_type=AST_RETURN_STATEMENT,
                .ast_return_statement={
                    .value=parse_expression(parser),
                },
            });
            break;
        
        case KEYWORD_UNKNOWN: 
            printf("[parser]: Error - unknown keyword\n");
            exit(1);
    
        default:
            printf("[parser]: TODO - implement keyword ");
            token_print(&tok);
            exit(1);
    }

    return ast_node;
}

static ast_node_t* parse_expression(parser_t* parser) {
    return parse_assignment(parser);
}

/**
 * @brief Parses an assignment expression of the form "<left-hand side> = <right-hand side>".
 * 
 * @param parser the parser object
 * @return ast_node_t* The AST node representing the assignment
 */
static ast_node_t* parse_assignment(parser_t* parser) {
    // Parse the left-hand side of the assignment.
    ast_node_t* left = parse_bool_or(parser);

    // If the current token is not an equals sign, return the left-hand side.
    if (parser->current_token && parser->current_token->token_type != TOKEN_EQUALS) {
        return left;
    }

    // Eat the equals sign.
    eat(parser, TOKEN_EQUALS);

    // Create a new AST node representing the assignment.
    return ast_node_new(((ast_node_t){
        .ast_type=AST_ASSIGNMENT, 
        .ast_assignment={
            .left=left, 
            .right=parse_assignment(parser)
        }
    }));
}

static ast_node_t* parse_bool_or(parser_t* parser) {
    ast_node_t* left = parse_bool_and(parser);

    if (parser->current_token && parser->current_token->token_type == TOKEN_BOOL_OR) {
        left = ast_node_new(((ast_node_t){
            .ast_type=AST_BIN_OP, 
            .ast_bin_op={
                .left=left, 
                .right=NULL,
                .op=parser->current_token->token_type,
            }
        }));
        // Eating the token
        eat(parser, parser->current_token->token_type);
        left->ast_bin_op.right = parse_bool_or(parser);
    }

    return left;
} 

static ast_node_t* parse_bool_and(parser_t* parser) {
    ast_node_t* left = parse_bitwise_or(parser);

    if (parser->current_token && parser->current_token->token_type == TOKEN_BOOL_AND) {
        left = ast_node_new(((ast_node_t){
            .ast_type=AST_BIN_OP, 
            .ast_bin_op={
                .left=left, 
                .right=NULL,
                .op=parser->current_token->token_type,
            }
        }));
        // Eating the token
        eat(parser, parser->current_token->token_type);
        left->ast_bin_op.right = parse_bool_and(parser);
    }

    return left;
}

static ast_node_t* parse_bitwise_or(parser_t* parser) {
    ast_node_t* left = parse_bitwise_xor(parser);

    // * Special case because of lambdas...
    if (!parser->getting_parameters && parser->current_token && parser->current_token->token_type == TOKEN_OR) {
        left = ast_node_new(((ast_node_t){
            .ast_type=AST_BIN_OP, 
            .ast_bin_op={
                .left=left, 
                .right=NULL,
                .op=parser->current_token->token_type,
            }
        }));
        // Eating the token
        eat(parser, parser->current_token->token_type);
        left->ast_bin_op.right = parse_bitwise_or(parser);
    }

    return left;
} 

static ast_node_t* parse_bitwise_xor(parser_t* parser) {
    ast_node_t* left = parse_bitwise_and(parser);

    if (parser->current_token && parser->current_token->token_type == TOKEN_HAT) {
        left = ast_node_new(((ast_node_t){
            .ast_type=AST_BIN_OP, 
            .ast_bin_op={
                .left=left, 
                .right=NULL,
                .op=parser->current_token->token_type,
            }
        }));
        // Eating the token
        eat(parser, parser->current_token->token_type);
        left->ast_bin_op.right = parse_bitwise_xor(parser);
    }

    return left;
}

static ast_node_t* parse_bitwise_and(parser_t* parser) {
    ast_node_t* left = parse_bool_equals(parser);

    if (parser->current_token && parser->current_token->token_type == TOKEN_AND) {
        left = ast_node_new(((ast_node_t){
            .ast_type=AST_BIN_OP, 
            .ast_bin_op={
                .left=left, 
                .right=NULL,
                .op=parser->current_token->token_type,
            }
        }));
        // Eating the token
        eat(parser, parser->current_token->token_type);
        left->ast_bin_op.right = parse_bitwise_and(parser);
    }

    return left;
}

static ast_node_t* parse_bool_equals(parser_t* parser) {
    ast_node_t* left = parse_bool_gtlt(parser);

    if (parser->current_token && (
        parser->current_token->token_type == TOKEN_BOOL_EQ ||
        parser->current_token->token_type == TOKEN_BOOL_NOTE
    )) {
        left = ast_node_new(((ast_node_t){
            .ast_type=AST_BIN_OP, 
            .ast_bin_op={
                .left=left, 
                .right=NULL,
                .op=parser->current_token->token_type,
            }
        }));
        // Eating the token
        eat(parser, parser->current_token->token_type);
        left->ast_bin_op.right = parse_bool_equals(parser);
    }

    return left;
}

static ast_node_t* parse_bool_gtlt(parser_t* parser) {
    ast_node_t* left = parse_bitwise_shlr(parser);

    if (parser->current_token && (
        parser->current_token->token_type == TOKEN_BOOL_GT ||
        parser->current_token->token_type == TOKEN_BOOL_GTE || 
        parser->current_token->token_type == TOKEN_BOOL_LT || 
        parser->current_token->token_type == TOKEN_BOOL_LTE 
    )) {
        left = ast_node_new(((ast_node_t){
            .ast_type=AST_BIN_OP, 
            .ast_bin_op={
                .left=left, 
                .right=NULL,
                .op=parser->current_token->token_type,
            }
        }));
        // Eating the token
        eat(parser, parser->current_token->token_type);
        left->ast_bin_op.right = parse_bool_gtlt(parser);
    }

    return left;
}  

static ast_node_t* parse_bitwise_shlr(parser_t* parser) {
    ast_node_t* left = parse_add_sub(parser);

    if (parser->current_token && (
        parser->current_token->token_type == TOKEN_SHL ||
        parser->current_token->token_type == TOKEN_SHR
    )) {
        left = ast_node_new(((ast_node_t){
            .ast_type=AST_BIN_OP, 
            .ast_bin_op={
                .left=left, 
                .right=NULL,
                .op=parser->current_token->token_type,
            }
        }));
        // Eating the token
        eat(parser, parser->current_token->token_type);
        left->ast_bin_op.right = parse_bitwise_shlr(parser);
    }

    return left;
}

static ast_node_t* parse_add_sub(parser_t* parser) {
    ast_node_t* left = parse_mul_div_mod(parser);

    if (parser->current_token && (
        parser->current_token->token_type == TOKEN_ADD ||
        parser->current_token->token_type == TOKEN_SUB
    )) {
        left = ast_node_new(((ast_node_t){
            .ast_type=AST_BIN_OP, 
            .ast_bin_op={
                .left=left, 
                .right=NULL,
                .op=parser->current_token->token_type,
            }
        }));
        // Eating the token
        eat(parser, parser->current_token->token_type);
        left->ast_bin_op.right = parse_add_sub(parser);
    }

    return left;
}

static ast_node_t* parse_mul_div_mod(parser_t* parser) {
    ast_node_t* left = parse_pow(parser);

    if (parser->current_token && (
        parser->current_token->token_type == TOKEN_MUL ||
        parser->current_token->token_type == TOKEN_DIV ||
        parser->current_token->token_type == TOKEN_MOD
    )) {
        left = ast_node_new(((ast_node_t){
            .ast_type=AST_BIN_OP, 
            .ast_bin_op={
                .left=left, 
                .right=NULL,
                .op=parser->current_token->token_type,
            }
        }));
        // Eating the token
        eat(parser, parser->current_token->token_type);
        left->ast_bin_op.right = parse_mul_div_mod(parser);
    }

    return left;
}

static ast_node_t* parse_pow(parser_t* parser) {
    ast_node_t* left = parse_before(parser);

    if (parser->current_token && parser->current_token->token_type == TOKEN_POW) {
        left = ast_node_new(((ast_node_t){
            .ast_type=AST_BIN_OP, 
            .ast_bin_op={
                .left=left, 
                .right=NULL,
                .op=parser->current_token->token_type,
            }
        }));
        // Eating the token
        eat(parser, parser->current_token->token_type);
        left->ast_bin_op.right = parse_pow(parser);
    }

    return left;
}   

/**
 * @brief This function checks if the current token is a unary operator and creates a new AST node for it, 
 * then recursively parses the expression after the operator. If the current token is not a unary operator, 
 * it calls parse_after to parse the expression after it.
 * 
 * @param parser parser object
 * @return ast_node_t* 
 */
static ast_node_t* parse_before(parser_t* parser) {
    if (
        parser->current_token && (
        parser->current_token->token_type == TOKEN_ADD ||
        parser->current_token->token_type == TOKEN_SUB ||
        parser->current_token->token_type == TOKEN_NOT ||
        parser->current_token->token_type == TOKEN_BOOL_NOT
    )) {
        token_type_t type = parser->current_token->token_type;
        eat(parser, type);

        ast_node_t* value = parse_before(parser);
        if (value->ast_type == AST_NUMBER) {
            switch (type) {
                case TOKEN_ADD: return value;
                case TOKEN_SUB: value->ast_number.value *= -1; return value;
                case TOKEN_NOT: value->ast_number.value = ~((int64_t)value->ast_number.value); return value;
                case TOKEN_BOOL_NOT: value->ast_number.value = !value->ast_number.value; return value;
                default: break;
            }
        }

        ast_node_t* node = ast_node_new((ast_node_t){
            .ast_type=AST_UNARY_OP, 
            .ast_unary_op={
                .value=value,
                .op=type
            }});

        // TODO: make optimizations
        return node;
    }
        
    return parse_after(parser, NULL);
} 

/**
 * @brief This function is called after parsing a factor to handle any operators or other operations
 * that might appear after the factor. It recursively parses the rest of the expression and returns
 * the corresponding AST node.
 * 
 * @param parser parser object
 * @param value NULL by default
 * @return ast_node_t* 
 */
static ast_node_t* parse_after(parser_t* parser, ast_node_t* value) {
    // If 'value' is null, set it to the result of parsing a factor.
    value = value ? value : parse_factor(parser);
    // If 'value' or 'current_token' is null, return 'value'.
    if (!value || !parser->current_token) return value;

    // Check the type of the current token and perform an action based on the type.
    switch (parser->current_token->token_type) {
        // Function definition/call
        case TOKEN_LPAREN: {
            // Parse a list of arguments/parameters.
            ast_node_t* list = parse_list(parser, false);

            // If the next token is a left brace, this is a function definition.
            if (parser->current_token && parser->current_token->token_type == TOKEN_LBRACE) {
                // Ensure that the value being defined is an identifier.
                if (value->ast_type != AST_IDENTIFIER) {
                    printf("[parser]: Error - invalid syntax (can't define a function like that bro)\n");
                    exit(1);
                }

                // Create a new function AST node and return it.
                ast_node_t* func = ast_node_new((ast_node_t){
                    .ast_type=AST_FUNCTION,
                    .ast_function={
                        .name={
                            .value=value->ast_string.value,
                            .length=value->ast_string.length
                        },
                        .parameters=list,
                        .body=parse_block(parser),
                    }
                });
                free(value);
                return func;
            }

            if (value->ast_type == AST_ACCESS) {
                // This is a method call.
                linked_list_node_t* old_head = list->ast_compound.head;

                linked_list_node_t* new_head = malloc(sizeof(linked_list_node_t));  // the first param will be the `this`
                new_head->item = value->ast_access.value;
                new_head->next = old_head;
                list->ast_compound.size++;
                list->ast_compound.head = new_head;

                ast_node_t* method_call = ast_node_new((ast_node_t){
                    .ast_type=AST_METHOD_CALL,
                    .ast_method_call={
                        .callable=value->ast_access.field,
                        .arguments=list,
                        .this=value->ast_access.value // dont really need this...
                    }
                });

                return parse_after(parser, method_call);
            }

            // This is a function call.
            ast_node_t* function_call = ast_node_new((ast_node_t){
                .ast_type=AST_CALL,
                .ast_call={
                    .callable=value,
                    .arguments=list
                }
            });

            return parse_after(parser, function_call);
        }

        // Array subscript
        case TOKEN_LBRACKET: {
            // Parse the subscript expression inside the brackets.
            ast_node_t* subscript = parse_brackets(parser);
            // Create a new AST node for the array subscript and return it.
            ast_node_t* array_subscript = ast_node_new((ast_node_t){
                .ast_type=AST_SUBSCRIPT,
                .ast_subscript={
                    .value=value,
                    .subscript=subscript
                }
            });

            return parse_after(parser, array_subscript);
        }

        // Class access expression (e.g. expr.id)
        case TOKEN_DOT: {
            // Eat the dot token and parse the field expression.
            eat(parser, TOKEN_DOT);
            ast_node_t* field = parse_factor(parser);
            // Ensure that the field is an identifier.
            if (field->ast_type != AST_IDENTIFIER) {
                printf("[parser]: Error - invalid syntax (can't get a field that is an expression)\n");
                exit(1);
            }

            // Create a new AST node for the class access expression and return it.
            ast_node_t* class_access = ast_node_new((ast_node_t){
                .ast_type=AST_ACCESS,
                .ast_access={
                    .value=value,
                    .field=field
                }
            });

            return parse_after(parser, class_access);
        }

        // Default case: do nothing.
        default: break;
    }

    return value;
}

static ast_node_t* parse_factor(parser_t* parser) {
    // If there are no more tokens to parse, return NULL.
    if (!parser->current_token) {
        return NULL;
    }

    ast_node_t* ast_node = NULL;

    switch (parser->current_token->token_type) {
        // If the token is a string literal.
        case TOKEN_STRING:
            // Create a new AST node for the string literal.
            ast_node = ast_node_new(((ast_node_t){
                .ast_type = AST_STRING,
                .ast_string = {
                    .value = (char*)parser->current_token->value,
                    .length = parser->current_token->length
                }
            })); 
            break;

        // If the token is a numeric literal.
        case TOKEN_FLOAT:
        case TOKEN_INT:
            // Create a new AST node for the numeric literal.
            ast_node = ast_node_new(((ast_node_t){
                .ast_type = AST_NUMBER,
                .ast_number = {
                    .value = strtod(parser->current_token->value, NULL)
                }
            }));
            break;

        // If the token is an identifier.
        case TOKEN_ID:
            // TODO: add check for statements (e.g. if, while, for)

            // Create a new AST node for the identifier.
            ast_node = ast_node_new(((ast_node_t){
                .ast_type = AST_IDENTIFIER,
                .ast_string = {
                    .value = (char*)parser->current_token->value,
                    .length = parser->current_token->length
                }
            })); 
            break;
        
        // If the token is a keyword.
        case TOKEN_KEYWORD:
            return parse_statement(parser);

        // Case of a list
        case TOKEN_LPAREN: {
            // Parse a list of expressions inside the parentheses.
            return parse_list(parser, false);
        }

        // Case of a lambda |x, ...| {...}
        case TOKEN_OR: {

            parser->getting_parameters = true;
            // Parse a list of expressions inside the parentheses.
            ast_node_t* list = parse_list(parser, true);

            parser->getting_parameters = false;

            // Parse a block of expressions to serve as the function body.
            ast_node_t* block = parse_block(parser);
            
            static char name[] = "<anonymous>";  
            // Create a new AST node for the function.
            ast_node_t* func = ast_node_new(((ast_node_t){
                .ast_type=AST_FUNCTION,
                .ast_function={
                    .name={.value=name, .length=sizeof(name)}, // The function has no name.
                    .parameters=list, // The function parameters come from the list we just parsed.
                    .body=block, // The function body is the block of expressions we just parsed.
                },
            }));
            return func; // Return the function AST node.
        }
        
        // Case of a block
        case TOKEN_LBRACE: {
            // Parse a list of expressions inside the parentheses.
            return parse_block(parser);
        }

        case TOKEN_LINE_COMMENT:
            while (parser->current_token && parser->current_token->token_type != TOKEN_NL &&
                                            parser->current_token->token_type != TOKEN_EOF)
                parser->current_token = lexer_get_next_token(parser->lexer);
            
            skip_newlines(parser);
            return NULL;
            
        case TOKEN_MULTILINE_COMMENT_START:
            while (parser->current_token && parser->current_token->token_type != TOKEN_MULTILINE_COMMENT_END) {
                if (parser->current_token->token_type == TOKEN_EOF) {
                    printf("[parser]: Error - unexpected end of multi line comment (you forgot to close it with \"*/\")\n");
                    exit(1);
                }
                parser->current_token = lexer_get_next_token(parser->lexer);
            }
            eat(parser, TOKEN_MULTILINE_COMMENT_END);
            skip_newlines(parser);
            return NULL;

        // end of expression or whatever
        case TOKEN_NL: 
        case TOKEN_EOF: break;
        
        // anything else
        default:
            printf("[parser]: Error - unexpected token ");
            token_print(parser->current_token);
            exit(1);
    }

    eat(parser, parser->current_token->token_type);
    return ast_node;
}