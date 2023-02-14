#include <stdio.h>
#include "parser/parser.h"
#include "io.h"

int main() {
    const char* filename = "script.kod"; 
    char* buffer = NULL;

    // Reading into buffer
    uint32_t length = io_read(filename, &buffer);

    // Initializing lexer and parser
    lexer_t lexer = lexer_init(buffer, length);
    parser_t parser = parser_init(&lexer);

    // Parsing AST
    ast_node_t* root = parse(&parser);
    ast_print(root, 0);
    return 0;
}