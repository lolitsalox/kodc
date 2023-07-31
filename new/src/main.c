#include <stdio.h>

#include "kod_io.h"
#include "parser/lexer.h"

int main(int argc, char** argv) {

    char* buffer = NULL;
    size_t buffer_size = 0;

    unwrap(io_read("script.kod", &buffer, &buffer_size));
    
    Lexer lexer = lexer_init(buffer, buffer_size);

    Token tok = {0};
    do {
        unwrap(lexer_get_next_token(&lexer, &tok));
        token_print(&tok);
        token_free(&tok);
    } while (tok.type != TOKEN_EOF);

    INFO("Done!");
    return 0;
}