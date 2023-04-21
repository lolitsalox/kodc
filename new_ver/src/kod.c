#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <defines.h>
#include <kod_io.h>
#include "lexer/lexer.h"
#include "parser/parser.h"

void repl();

s32 main(s32 argc, char** argv) {
    if (argc < 2) {
        repl();
        return 0;
    }

    char *filename = NULL, *err = NULL, *buffer = NULL;
    size_t buffer_size = 0;

    filename = argv[1];
    
    if (io_read(filename, &buffer, &buffer_size, &err) == STATUS_FAIL) {
        ERROR("IO", err);
        return STATUS_FAIL;
    }

    Lexer lex = lexer_init(buffer, buffer_size);
    Parser parser = parser_init(&lex);

    AstNode* root = NULL;
    if (parse(&parser, &root, &err) == STATUS_FAIL) {
        ast_free(root);
        token_free(parser.current_token);
        ERROR("Kod", err);
        return STATUS_FAIL;
    }

    ast_print(root, 0);
    ast_free(root);
    token_free(parser.current_token);
    // Token* tok = NULL;
    // bool exit = false;

    // while (!exit) {
    //     if (lexer_get_next_token(&lex, &tok, &err) == STATUS_FAIL) {
    //         LOG_ARGS("Exiting with error '%s'", err);
    //         return STATUS_FAIL;
    //     }
        
    //     if (tok->type == TOKEN_EOF) exit = true;
    //     token_print(tok);
    //     token_free(tok);
    // }

    LOG("Exiting without any errors!");
    return 0;
}

void repl() {
    UNIMPLEMENTED
}