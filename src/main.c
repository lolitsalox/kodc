#include <stdio.h>
#include "parser/parser.h"
#include "io/io.h"
#include "runtime/runtime.h"
#include "runtime/object.h"

int main() {

#if 0
    char arr[] = "\
print(int.__add__(5, 10))\
";
    lexer_t lexer = lexer_init(arr, sizeof(arr));
#else
    const char* filename = "script.kod"; 
    char* buffer = NULL;

    // Reading into buffer
    uint32_t length = io_read(filename, &buffer);

    // Initializing lexer and parser
    lexer_t lexer = lexer_init(buffer, length);
#endif
    parser_t parser = parser_init(&lexer);

    // Parsing AST
    ast_node_t* root = parse(&parser);
    ast_print(root, 0);

    

    // eval(root);
    // kod_object_t* res = interpret_ast(root);
    // object_print(res, 0);
    // object_free(res);

    return 0;
}