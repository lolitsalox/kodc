#include <runtime/runtime.hpp>
#include <runtime/objects/Type.hpp>

namespace kod {

void test() {
    Int i(42);
    std::cout << i.to_string() << std::endl;
    std::cout << i.type.to_string() << std::endl;
    std::cout << i.type.type.to_string() << std::endl;
}

std::shared_ptr<Object> constant_to_object(Constant const& constant) {
    switch (constant.tag) {
        // case ConstantTag::C_NULL: return std::make_shared<ObjectNull>();
        // case ConstantTag::C_BOOL: return std::make_shared<ObjectBool>(constant._bool);
        // case ConstantTag::C_INTEGER: return std::make_shared<ObjectInt>(constant._int);
        // case ConstantTag::C_FLOAT: return std::make_shared<ObjectFloat>(constant._float);
        // case ConstantTag::C_ASCII: return std::make_shared<ObjectString>(constant._string);
        // case ConstantTag::C_CODE: return std::make_shared<ObjectCode>(constant._code);
        // case ConstantTag::C_TUPLE: return std::make_shared<ObjectTuple>(constant._tuple);
        default: throw std::runtime_error("Unknown constant tag: " + std::to_string(static_cast<uint32_t>(constant.tag)));
    }
}

std::optional<std::shared_ptr<Object>> VM::run() {
    std::optional<std::shared_ptr<Object>> result;

    if (call_stack.empty()) {
        call_stack.emplace_back(module.entry);
    }
    CallFrame* frame = &call_stack.back();

    while (frame->ip < frame->code.code.size()) {
        auto opcode = static_cast<Opcode>(frame->code.read8(frame->ip));
        switch (opcode) {
            case Opcode::OP_LOAD_CONST: {
                uint32_t index = frame->code.read32(frame->ip);

                auto obj = constant_objects[index];
                object_stack.push_back(obj);
            } break;

            case Opcode::OP_POP_TOP: {
                auto obj = object_stack.back();
                // if (repl) std::cout << obj->to_string() << std::endl;
                object_stack.pop_back();
            } break;

            case Opcode::OP_RETURN: {
                result = object_stack.back();
                object_stack.pop_back();
                
                // if repl and its the last frame dont pop the call stack
                if (repl && call_stack.size() == 1) {
                    return result;
                }
                
                call_stack.pop_back();
                if (call_stack.empty()) {
                    return result;
                }

                object_stack.push_back(result.value());
                frame = &call_stack.back();
            } break;

            case Opcode::OP_STORE_NAME: {
                uint32_t index = frame->code.read32(frame->ip);
                auto obj = object_stack.back();
                object_stack.pop_back();
                frame->locals[module.name_pool[index]] = obj;
            } break;

            case Opcode::OP_LOAD_NAME: {
                uint32_t index = frame->code.read32(frame->ip);
                std::string& name = module.name_pool[index];
                // search through the call stack and then global
                std::optional<std::shared_ptr<Object>> result = search_name(name);
                if (result.has_value()) {
                    object_stack.push_back(result.value());
                    break;
                }

                // Search the global environment
                std::optional<std::shared_ptr<Object>> global_result = search_global_name(name);
                if (global_result.has_value()) {
                    object_stack.push_back(global_result.value());
                    break;
                }

                throw std::runtime_error("Name not found: " + name);
            } break;

            // case Opcode::OP_BINARY_ADD: {
            //     auto right = object_stack.back();
            //     object_stack.pop_back();
            //     auto left = object_stack.back();
            //     object_stack.pop_back();

            //     auto obj = std::make_shared<ObjectInt>(left->to_int() + right->to_int());
            //     object_stack.push_back(obj);
            // } break;

            // case Opcode::OP_CALL: {
            //     auto arg_count = frame->code.read32(frame->ip);

            //     // pop the func
            //     auto obj = object_stack.back();
            //     object_stack.pop_back();

            //     std::shared_ptr<ObjectCode> func;

            //     // check if the func is a code object
            //     if ((func = std::dynamic_pointer_cast<ObjectCode>(obj))) {
            //         // nothing
            //     } else {
            //         throw std::runtime_error("Function is not a code object");
            //     }

            //     std::vector<std::shared_ptr<Object>> args;
            //     // pop all args
            //     for (uint32_t i = 0; i < arg_count; i++) {
            //         auto arg = object_stack.back();
            //         object_stack.pop_back();
            //         args.push_back(arg);
            //     }

            //     // map the args to their names in the func def
            //     ObjectMap arg_map;
            //     for (size_t i = 0; i < args.size(); i++) {
            //         const std::string& arg_name = func->value.params[i];
            //         arg_map[arg_name] = args[i];
            //     }

            //     // create a call frame->with the args
            //     frame = &call_stack.emplace_back(func->value, arg_map);
            // } break;

            default: throw std::runtime_error("Unknown opcode: " + Opcode_to_string(opcode));
        }
    }

    return result;
}

std::optional<std::shared_ptr<Object>> VM::search_name(const std::string& name) {
    // Search through the call stack
    for (int i = call_stack.size() - 1; i >= 0; i--) {
        if (call_stack[i].locals.count(name) > 0) {
            return call_stack[i].locals[name];
        }
    }
    return {};
}

std::optional<std::shared_ptr<Object>> VM::search_global_name(const std::string& name) {
    // Search the global environment
    if (globals.count(name) > 0) {
        return globals[name];
    }
    return {};
}

}
