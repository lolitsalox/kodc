#include <stdio.h>

#include "kod_io.h"
#include "parser/parser.h"

// #include "bdd-for-c.h"
// spec("kod") {
//     it("Should work") {
//         char* buffer = NULL;
//         size_t buffer_size = 0;

//         unwrap(io_read("script.kod", &buffer, &buffer_size));
        
//         Lexer lexer = lexer_init(buffer, buffer_size);

//         Token tok = {0};
//         do {
//             unwrap(lexer_get_next_token(&lexer, &tok));
//             token_print(&tok);
//             token_free(&tok);
//         } while (tok.type != TOKEN_EOF);

//         INFO("Done!");

//         check(true);
//     }
// }

int main(void) {

    char* buffer = NULL;
    size_t buffer_size = 0;

    unwrap(io_read("script.kod", &buffer, &buffer_size));
    
    Lexer lexer = lexer_init(buffer, buffer_size);
    Parser parser = parser_init(lexer);
    
    AstNode* root = NULL;
    unwrap(parser_parse(&parser, &root));

    ast_print(root, 0);

    ast_free(root);

    // Token tok = {0};
    // do {
    //     unwrap(lexer_get_next_token(&lexer, &tok));
    //     token_print(&tok);
    //     token_free(&tok);
    // } while (tok.type != TOKEN_EOF);

    INFO("Done!");
    return 0;
}