#pragma once

#include <parser/parser.hpp>
#include <unordered_map>
#include <runtime/objects/Object.hpp>
#include <runtime/objects/Dict.hpp>

namespace kod {

void test();


std::shared_ptr<Object> constant_to_object(Constant const& constant);

struct CallFrame {
    Code code;
    Dict locals;
    size_t ip = 0;

    CallFrame() = default;
    CallFrame(Code code, Dict locals = {}) : code(code), locals(locals) {}
    ~CallFrame() = default;

};

struct VM {
    CompiledModule& module;
    std::vector<CallFrame> call_stack;
    std::vector<std::shared_ptr<Object>> object_stack;
    std::vector<std::shared_ptr<Object>> constant_objects;
    Dict globals;

    bool repl = false;

    VM(CompiledModule& module, bool repl = false) : module(module), repl(repl) {
        // load globals
        load_globals();
        update_constants();
    }
    ~VM() = default;

    void load_globals();
    void update_constants() {
        constant_objects.clear();
        for (auto& constant : module.constant_pool) {
            constant_objects.push_back(constant_to_object(constant));
        }

        if (repl && !call_stack.empty()) {
            call_stack[0].code = module.entry;
        }
    }
    std::optional<std::shared_ptr<Object>> run();

    std::optional<std::shared_ptr<Object>> search_name(const std::string& name);
    std::optional<std::shared_ptr<Object>> search_global_name(const std::string& name);

    void print_object_stack();
};

}