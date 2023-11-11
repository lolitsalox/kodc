#include <iostream>
#include <fstream>
#include <parser/parser.hpp>
#include <runtime/runtime.hpp>
#include <runtime/objects/Type.hpp>
#include <runtime/objects/Tuple.hpp>

namespace kod {

void repl() {
    std::string line;
    kod::CompiledModule module("out.bkod");
    kod::VM vm(module, true);

    std::cout << ">>> ";
    while (std::getline(std::cin, line)) {
        try {
            Lexer lexer(line);
            Parser parser(lexer);
            auto root = parser.parse_program();

            root->compile(module, module.entry);
            #if 1
            std::cout << root->to_string() << std::endl;
            std::cout << module.entry.to_string() << std::endl;

            for (auto const& constant : module.constant_pool) {
                if (constant.tag != kod::ConstantTag::C_CODE) continue;
                std::cout << constant._code.to_string() + "\n";
            }

            std::cout << "\nConstant pool:" << std::endl;
            // print the constant pool with the index
            for (size_t i = 0; i < module.constant_pool.size(); ++i) {
                std::cout << i << ": " << module.constant_pool[i].to_string() << std::endl;
            }

            std::cout << "\nName pool:" << std::endl;
            // print the name pool with the index
            for (size_t i = 0; i < module.name_pool.size(); ++i) {
                std::cout << i << ": " << module.name_pool[i] << std::endl;
            }
            #endif
            vm.update_constants();
            auto opt_obj = vm.run();
            if (opt_obj) {
                auto& obj = opt_obj.value();
                std::cout << "Program returned: " + obj->type->__str__(obj) << std::endl;
            }
        } 
        catch (std::exception const& e) {
            std::cerr << "!!! Error encountred: " << e.what() << std::endl;
            // clear all call stacks except from the first one
            if (vm.call_stack.size() > 1 ) vm.call_stack.erase(vm.call_stack.begin() + 1, vm.call_stack.end());
        }
        if (!vm.call_stack.empty()) vm.call_stack[0].ip = 0;
        std::cout << ">>> ";
        std::cin.clear();
    }
}

}

int main(int argc, char* argv[]) {

    // kod::test();
    // return 0;

    bool repl = argc < 2;

    if (repl) {
        kod::repl();
        return 0;
    }

    try {
        std::ifstream file(argv[1]);
        std::string s = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        kod::Lexer lexer(s);
        kod::Parser parser(lexer);
        auto root = parser.parse_program();

        kod::CompiledModule module("out.bkod");
        root->compile(module, module.entry);
        
        #if 1
        std::cout << root->to_string() << std::endl;
        std::cout << module.entry.to_string() << std::endl;

        for (auto const& constant : module.constant_pool) {
            if (constant.tag != kod::ConstantTag::C_CODE) continue;
            std::cout << constant._code.to_string() + "\n";
        }

        std::cout << "\nConstant pool:" << std::endl;
        // print the constant pool with the index
        for (size_t i = 0; i < module.constant_pool.size(); ++i) {
            std::cout << i << ": " << module.constant_pool[i].to_string() << std::endl;
        }

        std::cout << "\nName pool:" << std::endl;
        // print the name pool with the index
        for (size_t i = 0; i < module.name_pool.size(); ++i) {
            std::cout << i << ": " << module.name_pool[i] << std::endl;
        }
        #endif

        kod::VM vm(module);
        auto obj = vm.run();
        if (obj) {
            // std::cout << "Program returned: " << obj.value()->to_string() << std::endl;
        }
        
    } catch (std::exception const& e) {
        std::cerr << "!!! Error encountred: " << e.what() << std::endl;
    }

    return 0;
}
