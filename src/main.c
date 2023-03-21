#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser/parser.h"
#include "io/io.h"
// #include "runtime/runtime.h"
// #include "runtime/object.h"
#include "compiler/compiler.h"

int main() {

#if 0
    char arr[] = "\
print(int.__add__(5, 10))\
";
    lexer_t lexer = lexer_init(arr, sizeof(arr));
#else
    // const char* filename = "script.kod"; 
    char* filename = malloc(sizeof("script.kod"));
    strncpy(filename, "script.kod", sizeof("script.kod"));
    char* buffer = NULL;

    // Reading into buffer
    uint32_t length = io_read(filename, &buffer);

    // Initializing lexer and parser
    lexer_t lexer = lexer_init(buffer, length);
#endif
    parser_t parser = parser_init(&lexer);

    // Parsing AST
    ast_node_t* root = parse(&parser);
    // ast_print(root, 0);

    
    CompiledModule* module = new_compiled_module(filename, 0, 1);
    compile_module(root, module, &module->entry);
    print_constant_pool(&module->constant_pool);
    print_name_pool(&module->name_pool); // it wouldn't have shown different byte code if it were the same memory

    print_code(&module->entry, "\n");
    free_module(module);

    // eval(root);
    // kod_object_t* res = interpret_ast(root);
    // object_print(res, 0);
    // object_free(res);

    return 0;
}