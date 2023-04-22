#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <defines.h>
#include <kod_io.h>
#include <lexer/lexer.h>
#include <parser/parser.h>
#include <compiler/compiler.h>

void repl();

i32 main(i32 argc, char** argv) {
    char *filename = NULL, *err = NULL, *buffer = NULL;
    
    filename = argv[1];
    size_t buffer_size = 0;
    
    if (argc < 2) {
        repl();
        return 0;
    }
    
    if (io_read(filename, &buffer, &buffer_size, &err) == STATUS_FAIL) {
        ERROR("IO", err);
        return STATUS_FAIL;
    }

    Lexer lex = lexer_init(buffer, buffer_size);
    Parser parser = parser_init(&lex);

    AstNode* root = NULL;
    if (parse(&parser, &root, &err) == STATUS_FAIL) {
        ast_free(root);
        ERROR("Kod", err);
        return STATUS_FAIL;
    }

    // ast_print(root, 0);

        
    CompiledModule* module = new_compiled_module(filename, 0, 1);
    CompilationStatus status = compile_module(root, module, &module->entry);
    if (status.code == STATUS_FAIL) {
        puts(status.what);
        free_module(module);
        return 1;
    }

    print_code(&module->entry, "\n", &module->constant_pool, &module->name_pool);
    print_constant_pool(&module->constant_pool);
    print_name_pool(&module->name_pool);

    free_module(module);
    ast_free(root);

    LOG("Exiting without any errors!");
    return 0;
}

void repl() {
    char* err = NULL;
    char buffer[1<<8] = {0};
    CompiledModule* module = new_compiled_module("<stdin>", 0, 0);
    
    do {
        printf(">>> ");
        memset(buffer, 0, 1<<8);
        fgets(buffer, 1<<8, stdin);

        u32 length = strlen(buffer) + 1;
        buffer[length] = 0;

        Lexer lexer = lexer_init(buffer, length);
        Parser parser = parser_init(&lexer);

        AstNode* root = NULL;
        if (parse(&parser, &root, &err) == STATUS_FAIL) {
            ast_free(root);
            ERROR("Kod", err);
            continue;
        }
        
        CompilationStatus status = compile_module(root, module, &module->entry);
        if (status.code == STATUS_FAIL) {
            ERROR("Compile", status.what);
            goto free_stuff;
        }
        
        print_code(&module->entry, "\n", &module->constant_pool, &module->name_pool);
        print_constant_pool(&module->constant_pool);
        print_name_pool(&module->name_pool);

        free_stuff:
        free_code(module->entry);
        ast_free(root);
        module->entry = (Code) {.code=NULL, .params={0}, .size=0};
    
    } while (strcmp(buffer, "exit\n") != 0);

    free_module(module);
    puts("Exited kod!");
}