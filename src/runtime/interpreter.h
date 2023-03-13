#pragma once

#include "object.h"
#include "../parser/ast.h"

kod_object_t* eval(ast_node_t* root);
