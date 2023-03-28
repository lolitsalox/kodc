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
    size_t filename_size = sizeof("script.kod");
    char* filename = malloc(filename_size);
    strncpy(filename, "script.kod", filename_size);

#if 0
    char arr[] = "\
f(x) {\n\
    if x < 1 {\n\
        return 0\n\
    }\n\
    return x + f(x - 1)\n\
}\n\
\n\
i = 0\n\
print(f(2))\n\
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
    CompilationStatus status = compile_module(root, module, &module->entry);
    puts(status.what);
    if (status.code == STATUS_FAIL) {
        fputs("\x1b[31m!!!Error while compiling!!!\x1b[0m", stderr);
        return 1;
    }

    // save_module_to_file(module, "out.bkod");

    puts("\x1b[32m!!!Compilation success!!!\x1b[0m");

    // module = load_module_from_file("out.bkod");
    // if (!module) {
    //     puts("no module");
    //     return 1;
    // }

    VirtualMachine vm = init_vm(module);
    Kod_Object* res = vm_run_entry(&vm);
    deref_object(res);

    destroy_vm(&vm);
    puts("\n\x1b[36m>>> \x1b[32mNO ERRORS\x1b[36m!\x1b[0m");

    return 0;
}