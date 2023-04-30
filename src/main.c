#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser/parser.h"
#include "io/io.h"
#include "compiler/compiler.h"
#include "vm_new/vm.h"

void repl(void) {
    size_t filename_size = sizeof("<stdin>");
    char* filename = malloc(filename_size);
    strncpy(filename, "<stdin>", filename_size);

    CompiledModule* module = new_compiled_module(filename, 0, 1);

    VirtualMachine vm = init_vm(module, true);
    // CallFrame main_frame = init_call_frame(NULL, get_native_functions());
    CallFrame main_frame = init_call_frame(NULL, NULL);

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

        KodObject* result = run_code_object(&vm, &vm.module->entry, NULL, NULL, &main_frame);
        deref_object(result);

        main_frame.ip = 0;

        free_code(module->entry);
        module->entry = (Code) {.code=NULL, .params=init_string_array(), .size=0};
    } while (vm.running);

    free_call_frame(&main_frame, vm.constant_objects);
    destroy_vm(&vm);
    puts("\x1b[36m>>> \x1b[32mNO ERRORS\x1b[36m!\x1b[0m");

}

int main(int argc, char** argv) {
    // if (argc < 2) {
    //     repl();
    //     return 0;
    // }

    size_t filename_size = strlen(argv[1]) + 1;
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

    uint32_t length = io_read(filename, &buffer);
    lexer_t lexer = lexer_init(buffer, length);

    parser_t parser = parser_init(&lexer);
    ast_node_t* root = parse(&parser);
    // ast_print(root, 0);

    
    CompiledModule* module = new_compiled_module(filename, 0, 1);
<<<<<<< HEAD
    if (compile_module(root, module, &module->entry) == STATUS_OK) {
        // save_module_to_file(module, "out.bkod");

        // print_constant_pool(&module->constant_pool);
        // print_name_pool(&module->name_pool); // it wouldn't have shown different byte code if it were the same memory
        print_code(&module->entry, "\n");

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
=======
    CompilationStatus status = compile_module(root, module, &module->entry);
    if (status.code == STATUS_FAIL) {
        puts(status.what);
        free_module(module);
        return 1;
    }

    // print_code(&module->entry, "\n");
    // print_constant_pool(&module->constant_pool);
    save_module_to_file(module, "out.bkod");
    free_module(module);



    module = load_module_from_file("out.bkod");
    if (!module) {
        puts("no module");
        return 1;
    }
>>>>>>> develop

    VirtualMachine vm = init_vm(module, false);
    KodObject* res = vm_run_entry(&vm);
    deref_object(res);
<<<<<<< HEAD
    // print_constant_pool(&module->constant_pool);
    // print_name_pool(&module->name_pool); // it wouldn't have shown different byte code if it were the same memory
    // print_code(&module->entry, "\n");
    destroy_vm(&vm);
    puts("\n\x1b[36m>>> \x1b[32mNO ERRORS\x1b[36m!\x1b[0m");

    // eval(root);
    // kod_object_t* res = interpret_ast(root);
    // object_print(res, 0);
    // object_free(res);
=======

    destroy_vm(&vm);
    puts("\n\x1b[36m>>> \x1b[32mNO ERRORS\x1b[36m!\x1b[0m");
>>>>>>> develop

    return 0;
}


/*
>>> a = 0
>>> 
*/