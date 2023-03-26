#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser/parser.h"
#include "io/io.h"
// #include "runtime/runtime.h"
// #include "runtime/object.h"
#include "compiler/compiler.h"
#include "vm/vm.h"

int main() {
    char* filename = malloc(sizeof("script.kod"));
    strncpy(filename, "script.kod", sizeof("script.kod"));

#if 1
    char arr[] = "\
fib(n) {\n\
    if n < 2 {\n\
        return 1\n\
    }\n\
    return fib(n - 1) + fib(n - 2)\n\
}\n\
\n\
i = 0\n\
while i < 25 {\n\
    print(fib(i))\n\
    i = i + 1\n\
}\n\
";
    lexer_t lexer = lexer_init(arr, sizeof(arr));
#else
    // const char* filename = "script.kod"; 
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
    if (compile_module(root, module, &module->entry) == STATUS_OK) {
        // save_module_to_file(module, "out.bkod");

        // print_constant_pool(&module->constant_pool);
        // print_name_pool(&module->name_pool); // it wouldn't have shown different byte code if it were the same memory
        // print_code(&module->entry, "\n");

        puts("\x1b[32m!!!Compilation success!!!\x1b[0m");
    }
    else {
        fputs("\x1b[31m!!!Error while compiling!!!\x1b[0m", stderr);
    }
    // free_module(module);

    // module = load_module_from_file("out.bkod");
    // if (!module) {
    //     puts("no module");
    //     return 1;
    // }

    VirtualMachine vm = init_vm(module);
    vm_run_entry(&vm);
    // print_constant_pool(&module->constant_pool);
    // print_name_pool(&module->name_pool); // it wouldn't have shown different byte code if it were the same memory
    // print_code(&module->entry, "\n");
    free_module(module);
    puts("After final free!");

    // eval(root);
    // kod_object_t* res = interpret_ast(root);
    // object_print(res, 0);
    // object_free(res);

    return 0;
}