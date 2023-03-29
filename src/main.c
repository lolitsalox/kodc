#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser/parser.h"
#include "io/io.h"
#include "compiler/compiler.h"
#include "vm/vm.h"

void repl(void) {
    size_t filename_size = sizeof("<stdin>");
    char* filename = malloc(filename_size);
    strncpy(filename, "<stdin>", filename_size);

    CompiledModule* module = new_compiled_module(filename, 0, 1);

    VirtualMachine vm = init_vm(module, true);
    CallFrame main_frame = init_call_frame(NULL, get_native_functions());

    char buffer[1<<8] = {0};
    do {
        printf(">>> ");
        memset(buffer, 0, 1<<8);
        fgets(buffer, 1<<8, stdin);

        uint32_t length = strlen(buffer) + 1;
        buffer[length] = 0;

        lexer_t lexer = lexer_init(buffer, length);
        parser_t parser = parser_init(&lexer);
        ast_node_t* root = parse(&parser);
        // ast_print(root, 0);

        
        CompilationStatus status = compile_module(root, module, &module->entry);
        // print_code(&module->entry, "\n", &module->constant_pool, &module->name_pool);
        vm = init_vm(module, true);

        if (status.code == STATUS_FAIL) {
            puts(status.what);
            continue;
        }

        // print_code(&module->entry, "\n");
        // print_constant_pool(&module->constant_pool);
        // save_module_to_file(module, "out.bkod");
        // free_module(module);



        // module = load_module_from_file("out.bkod");
        // if (!module) {
        //     puts("no module");
        //     return 1;
        // }

        Kod_Object* result = run_code_object(&vm, &vm.module->entry, NULL, NULL, &main_frame);
        deref_object(result);

        main_frame.ip = 0;

        free_code(module->entry);
        module->entry = (Code) {.code=NULL, .params=init_string_array(), .size=0};
    } while (strcmp(buffer, "exit\n") != 0);

    free_call_frame(&main_frame, &vm.cop);
    destroy_vm(&vm);
    puts("\n\x1b[36m>>> \x1b[32mNO ERRORS\x1b[36m!\x1b[0m");

}

int main() {
    repl();
    return 0;

    // size_t filename_size = sizeof("script.kod");
    // char* filename = malloc(filename_size);
    // strncpy(filename, "script.kod", filename_size);
    // char* buffer = NULL;

    // uint32_t length = io_read(filename, &buffer);
    // lexer_t lexer = lexer_init(buffer, length);

    // parser_t parser = parser_init(&lexer);
    // ast_node_t* root = parse(&parser);
    // // ast_print(root, 0);

    
    // CompiledModule* module = new_compiled_module(filename, 0, 1);
    // CompilationStatus status = compile_module(root, module, &module->entry);
    // puts(status.what);
    // if (status.code == STATUS_FAIL) {
    //     free_module(module);
    //     return 1;
    // }

    // // print_code(&module->entry, "\n");
    // // print_constant_pool(&module->constant_pool);
    // save_module_to_file(module, "out.bkod");
    // free_module(module);



    // module = load_module_from_file("out.bkod");
    // if (!module) {
    //     puts("no module");
    //     return 1;
    // }

    // VirtualMachine vm = init_vm(module);
    // Kod_Object* res = vm_run_entry(&vm);
    // deref_object(res);

    // destroy_vm(&vm);
    // puts("\n\x1b[36m>>> \x1b[32mNO ERRORS\x1b[36m!\x1b[0m");

    return 0;
}


/*
>>> a = 0
>>> 
*/