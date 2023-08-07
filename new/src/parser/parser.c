#include "parser.h"

static Result compound(Parser* parser, AstNode** out);
static Result assignment(Parser* parser, AstNode** out);
static Result factor  (Parser* parser, AstNode** out);

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
    return (Result){0};
}

Result eat(Parser* parser, TokenType_t ttype) {
    if (parser->current_token.type != ttype) {
        ERROR_LOG("Parser", "unexpected token at");

        token_print(parser->current_token);

        printf(", expected (%s)\n", token_type_to_str(ttype));
        return (Result){.what="unexpected token"};
    }

    token_free(&parser->current_token);
    
    return lexer_get_next_token(&parser->lexer, &parser->current_token);
}

void skip_newlines(Parser* parser) {
    while (parser->current_token.type == TOKEN_NL)
        unwrap(eat(parser, TOKEN_NL));
}

Result compound(Parser* parser, AstNode** out) {
    unwrap(ast_new((AstNode){.type=AST_COMPOUND}, out));

    while (parser->current_token.type != TOKEN_EOF) {
        AstNode* value = NULL;

        skip_newlines(parser);

        unwrap(assignment(parser, &value));
        if (!value) continue;

        skip_newlines(parser);

        DA_APPEND((*out)->compound, value);
    }

    return (Result){0};
}

Result assignment(Parser* parser, AstNode** out) {
    AstNode* left = NULL;
    unwrap(factor(parser, &left));

    if (parser->current_token.type != TOKEN_EQUALS) {
        *out = left;
        return (Result){0};
    }

    unwrap(eat(parser, TOKEN_EQUALS));

    switch (left->type) {
        
        case AST_IDENTIFIER:
            unwrap(ast_new((AstNode){.type=AST_ASSIGNMENT,.assignment={.left=left,.right=NULL}}, out));
            return factor(parser, &(*out)->assignment.right);

        default:
            ERROR_ARGS("Parser", "Assignment for %s is not implemented yet", ast_type_to_str(left->type));
            return (Result){.what="unimplemented"};
    }

    assert(0 && "Unreachable");
}

Result factor(Parser* parser, AstNode** out) {
    switch (parser->current_token.type) {
        
        case TOKEN_ID:
        case TOKEN_STRING:
            unwrap(ast_new((AstNode){
                .type=(parser->current_token.type == TOKEN_STRING ? AST_STRING : AST_IDENTIFIER),
                .string=strdup(parser->current_token.value)
            }, out));
            break;

        case TOKEN_INT:
            unwrap(ast_new((AstNode){
                .type=AST_INT,
                .int_=strtoi64(parser->current_token.value, NULL, 10)
            }, out));
            break;

        case TOKEN_FLOAT:
            unwrap(ast_new((AstNode){
                .type=AST_FLOAT,
                .float_=strtod(parser->current_token.value, NULL)
            }, out));
            break;

        default:
            ERROR_LOG("Syntax", "unexpected token");
            token_print(parser->current_token);
            return (Result){.what="unexpected token"};
    }

    return eat(parser, parser->current_token.type);
}