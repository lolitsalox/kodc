#include <stdio.h>

#include "kod_io.h"
#include "parser/ast.h"
#include "parser/lexer.h"

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

    AstNode* root = NULL;
    AstNode* node = NULL;

    unwrap(ast_new((AstNode){.type=AST_ROOT}, &root));
    unwrap(ast_new((AstNode){.type=AST_STRING,.string=strdup("Hello world!")}, &node));

    DA_APPEND(root->compound, node);

    ast_print(root, 0);
    ast_free(root);

    INFO("Done!");
    return 0;
}