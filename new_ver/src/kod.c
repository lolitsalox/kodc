#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "defines.h"
#include "kod_io.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "compiler/compiler.h"
#include "runtime/vm.h"

#pragma warning(disable : 4703)
#pragma warning(disable : 6001)

void repl();

i32 run_module(CompiledModule* module) {
    VirtualMachine* vm = calloc(1, sizeof(VirtualMachine));
    if (!vm) return 1;
    vm->initialized = false;
    Status s = vm_init(module, true, vm);
    if (s.type == ST_FAIL) {
        puts(s.what);
        free(s.what);
        return 1;
    }

    KodObject* result = NULL;
    s = vm_run_code_object(vm, &module->entry, NULL, &result);

    if (s.type == ST_FAIL) {
        ERROR("KodRuntime", s.what);
        free(s.what);
        return 1;
    }

    if ((s = object_stack_clear(&vm->stack, false)).type == ST_FAIL) {
        ERROR("KodAfterRuntime", s.what);
        free(s.what);
    }

    if (result && result->ref_count == 0 && (s = kod_object_deref(result)).type == ST_FAIL) {
        ERROR("KodRuntime", s.what);
        free(s.what);
        return 1;
    }

    if ((s = vm_destroy(vm)).type == ST_FAIL) {
        ERROR("KodRuntime", s.what);
        return 1;
    };
    return 0;
}

i32 main(i32 argc, char** argv) {
    char *filename = "script.kod", *err = NULL, *buffer = NULL;
    system("cd");
    //filename = argv[1];
    size_t buffer_size = 0;
    /*
    if (argc < 2) {
        repl();
        return 0;
    }
    */
    
    if (io_read(filename, &buffer, &buffer_size, &err) == STATUS_FAIL) {
        ERROR("IO", err);
        return STATUS_FAIL;
    }

    Lexer lex = lexer_init(buffer, (u32)buffer_size);
    Parser parser = parser_init(&lex);

    AstNode* root = NULL;
    if (parse(&parser, &root, &err) == STATUS_FAIL) {
        ast_free(root);
        ERROR("Kod", err);
        return STATUS_FAIL;
    }

#ifdef DEBUG_PARSER
    ast_print(root, 0);
#endif
        
    CompiledModule* module = new_compiled_module(filename, 0, 1);
    CompilationStatus status = compile_module(root, module, &module->entry);
    if (status.code == STATUS_FAIL) {
        puts(status.what);
        free_module(module);
        return 1;
    }

#ifdef DEBUG_COMPILER
    print_code(&module->entry, "\n", &module->constant_pool, &module->name_pool);
    print_constant_pool(&module->constant_pool);
    print_name_pool(&module->name_pool);
#endif

    if (run_module(module) == 1) return 1;

    free_module(module);
    ast_free(root);

    LOG("Exiting without any errors!");
    return 0;
}

void repl() {
    char* err = NULL;
    char buffer[1<<8] = {0};
    CompiledModule* module = new_compiled_module("<stdin>", 0, 0);
    VirtualMachine* vm = calloc(1, sizeof(VirtualMachine));
    if (!vm) return;
    vm->initialized = false;
    Status s = vm_init(module, true, vm);
    if (s.type == ST_FAIL) {
        puts(s.what);
        free(s.what);
        return;
    }
    
    do {
        printf(">>> ");
        memset(buffer, 0, 1<<8);
        fgets(buffer, 1<<8, stdin);

        size_t length = strlen(buffer) + 1;
        buffer[length] = 0;

        Lexer lexer = lexer_init(buffer, (u32)length);
        Parser parser = parser_init(&lexer);

        AstNode* root = NULL;
        if (parse(&parser, &root, &err) == STATUS_FAIL) {
            ast_free(root);
            ERROR("Kod", err);
            continue;
        }
#ifdef DEBUG_PARSER
        ast_print(root, 0);
#endif

        CompilationStatus status = compile_module(root, module, &module->entry);
        if (status.code == STATUS_FAIL) {
            ERROR("Compile", status.what);
            goto free_stuff;
        }
        
#ifdef DEBUG_COMPILER
        print_code(&module->entry, "\n", &module->constant_pool, &module->name_pool);
        print_constant_pool(&module->constant_pool);
        print_name_pool(&module->name_pool);
#endif

        Status s = vm_init(module, true, vm);
        if (s.type == ST_FAIL) {
            puts(s.what);
            free(s.what);
            return;
        }

        KodObject* result = NULL;
        s = vm_run_code_object(vm, &module->entry, NULL, &result);

        if (s.type == ST_FAIL) {
            ERROR("KodRuntime", s.what);
            free(s.what);
        }

        if ((s = object_stack_clear(&vm->stack, false)).type == ST_FAIL) {
            ERROR("KodAfterRuntime", s.what);
            free(s.what);
        }

        free_stuff:
        if (result && result->ref_count == 0 && (s = kod_object_deref(result)).type == ST_FAIL) {
            ERROR("KodRuntime", s.what);
            free(s.what);
        }
        free_code(module->entry);
        ast_free(root);
        module->entry = (Code) {.code=NULL, .params={0}, .size=0};
        vm->frame_stack.frames[0].ip = 0; // resetting the ip
    
    } while (strcmp(buffer, "exit\n") != 0);

    if ((s = vm_destroy(vm)).type == ST_FAIL) {
        ERROR("KodRuntime", s.what);
    };
    free_module(module);
    free(vm);
    puts("Exited kod!");
}