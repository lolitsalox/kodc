#include <stdio.h>
#include "parser/parser.h"
#include "io/io.h"
#include "runtime/runtime.h"
#include "runtime/object.h"

int main() {

//     char arr[] = "a = \"hello world\"\n\
// d = a\n\
// print(d)\n\
// a = \"Byeee\"\n\
// \n\
// main() {\n\
//     z = \"ZZZ\"\n\
//     return z\n\
// }\n\
// print(main(), a)\n\
// main()\n\
// ";

    const char* filename = "script.kod"; 
    char* buffer = NULL;

    // Reading into buffer
    uint32_t length = io_read(filename, &buffer);

    // lexer_t lexer = lexer_init(arr, sizeof(arr));
    // Initializing lexer and parser
    lexer_t lexer = lexer_init(buffer, length);
    parser_t parser = parser_init(&lexer);

    // Parsing AST
    ast_node_t* root = parse(&parser);
    // ast_print(root, 0);

    // eval(root);
    kod_object_t* res = interpret_ast(root);
    object_print(res, 0);
    object_free(res);

    return 0;
}