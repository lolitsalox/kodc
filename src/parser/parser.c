#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

static void eat(parser_t* parser, token_type_t type);
static void skip_newlines(parser_t* parser);

static ast_node_t* parse_compound(parser_t* parser);

static ast_node_t* parse_list(parser_t* parser, bool lambda);
static ast_node_t* parse_block(parser_t* parser);

static ast_node_t* parse_assignment(parser_t* parser);       // = -> += -=, etc...
static ast_node_t* parse_logical_or(parser_t* parser);       // ||
static ast_node_t* parse_logical_and(parser_t* parser);      // &&
static ast_node_t* parse_bitwise_or(parser_t* parser);       // |
static ast_node_t* parse_bitwise_xor(parser_t* parser);      // ^
static ast_node_t* parse_bitwise_and(parser_t* parser);      // &
static ast_node_t* parse_bool_equals(parser_t* parser);      // == !=
static ast_node_t* parse_bool_gtlt(parser_t* parser);        // < > <= >=
static ast_node_t* parse_bitwise_shlr(parser_t* parser);     // << >>
static ast_node_t* parse_add_sub(parser_t* parser);          // + -
static ast_node_t* parse_mul_div_mod(parser_t* parser);      // * / %
static ast_node_t* parse_pow(parser_t* parser);              // **       
static ast_node_t* parse_second(parser_t* parser);           // + - ! ~ @ # sizeof       
static ast_node_t* parse_first(parser_t* parser);            // from the right () [] . 
static ast_node_t* parse_factor(parser_t* parser);           // values

parser_t parser_init(lexer_t* lexer) {
    if (!lexer) {
        printf("[parser]: Warning - lexer is null\n");
        return (parser_t){0};
    }

    return (parser_t){.lexer=lexer, .current_token=lexer_get_next_token(lexer)};
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
        ast_node_t* value = parse_assignment(parser);
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

    // Eat the left delimiter of the block
    eat(parser, left_delimiter);

    // Skip any newlines before the first item
    skip_newlines(parser);

    // Parse each item in the block
    while (parser->current_token && parser->current_token->token_type != right_delimiter) {
        skip_newlines(parser);

        // Parse the item and add it to the block
        ast_node_t* value = parse_assignment(parser);
        if (!value) continue;
        linked_list_append(&body->ast_compound, value);

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

/**
 * @brief Parses an assignment expression of the form "<left-hand side> = <right-hand side>".
 * 
 * @param parser the parser object
 * @return ast_node_t* The AST node representing the assignment
 */
static ast_node_t* parse_assignment(parser_t* parser) {
    // Parse the left-hand side of the assignment.
    ast_node_t* left = parse_add_sub(parser);

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
            .right=parse_add_sub(parser)
        }
    }));
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
    ast_node_t* left = parse_factor(parser);

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
            // Create a new AST node for the identifier.
            ast_node = ast_node_new(((ast_node_t){
                .ast_type = AST_IDENTIFIER,
                .ast_string = {
                    .value = (char*)parser->current_token->value,
                    .length = parser->current_token->length
                }
            })); 
            break;

        // Case of a list
        case TOKEN_LPAREN: {
            // Parse a list of expressions inside the parentheses.
            return parse_list(parser, false);
        }

        // Case of a lambda |x, ...| {...}
        case TOKEN_OR: {
            // Parse a list of expressions inside the parentheses.
            ast_node_t* list = parse_list(parser, true);
            
            skip_newlines(parser);

            // Parse a block of expressions to serve as the function body.
            ast_node_t* block = parse_block(parser);
            
            // Create a new AST node for the function.
            ast_node_t* func = ast_node_new(((ast_node_t){
                .ast_type=AST_FUNCTION,
                .ast_function={
                    .name="<anonymous>", // The function has no name.
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


        // new line
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