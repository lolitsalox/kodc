#pragma once

typedef struct kod_object_t kod_object_t;
typedef struct ast_node_t ast_node_t;

kod_object_t* interpret_ast(ast_node_t* root);
