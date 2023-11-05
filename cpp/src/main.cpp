#include <iostream>
#include <parser/parser.hpp>

int main(int argc, char* argv[]) {

    try {
        kod::Lexer lexer("script.kod");
        kod::Parser parser(lexer);
        auto root = parser.parse_program();
        std::cout << root->to_string() << std::endl;

        kod::CompiledModule module("out.bkod");
        root->compile(module, module.entry);

        std::cout << module.entry.to_string() << std::endl;

        std::cout << "\nConstant pool:" << std::endl;
        // print the constant pool with the index
        for (size_t i = 0; i < module.constant_pool.size(); ++i) {
            std::cout << i << ": " << module.constant_pool[i].to_string() << std::endl;
        }

        // kod::Compiler compiler(parser.root, "out.bkod");
        // kod::VM vm("out.bkod");
        // vm.run();
    } catch (std::exception const& e) {
        std::cerr << "!!! Error encountred: " << e.what() << std::endl;
    }

    return 0;
}
