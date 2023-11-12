#include <runtime/runtime.hpp>
#include <runtime/objects/Type.hpp>
#include <runtime/objects/Tuple.hpp>
#include <runtime/objects/Int.hpp>
#include <runtime/objects/String.hpp>
#include <runtime/objects/NativeFunc.hpp>
#include <runtime/objects/NativeMethod.hpp>
#include <runtime/objects/CodeObj.hpp>
#include <runtime/objects/Dict.hpp>

#include <algorithm>
#include <thread>

namespace kod {

void test() {
    // Int i(42);
    // std::cout << i.to_string() << std::endl;
    // std::cout << i.type.to_string() << std::endl;
    // std::cout << i.type.type.to_string() << std::endl;
}

void VM::print_object_stack() {
    std::cout << "Object stack:\n";
    for (auto& obj : object_stack) {
        std::cout << obj->type->type_name << " " << obj->type->__str__(obj) << std::endl;
    }
    std::cout << std::endl;
}

std::shared_ptr<Object> native_globals(VM* vm, std::shared_ptr<Tuple> args) {
    return std::make_shared<Dict>(vm->globals);
}

std::shared_ptr<Object> native_locals(VM* vm, std::shared_ptr<Tuple> args) {
    return std::make_shared<Dict>(vm->call_stack.back().locals);
}

std::shared_ptr<Object> native_print(VM* vm, std::shared_ptr<Tuple> args) {
    for (size_t i = 0; i < args->values.size(); i++) {
        std::cout << args->values[i]->type->__str__(args->values[i]) << " ";
    }
    std::cout << std::endl;
    return vm->globals["null"];
}

std::shared_ptr<Object> native_input(VM* vm, std::shared_ptr<Tuple> args) {
    std::string line;

    // if there is a string in the first argument, print it
    if (!args->values.empty()) {
        if (auto str = std::dynamic_pointer_cast<String>(args->values[0])) {
            std::cout << str->value;
        }
    }
    std::getline(std::cin, line);
    std::cin.clear();
    return std::make_shared<String>(line);
}

std::shared_ptr<Object> native_sleep(VM* vm, std::shared_ptr<Tuple> args) {
    if (args->values.size() > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(args->values[0]->type->__int__(args->values[0])));
    }
    return vm->globals["null"];
}

std::shared_ptr<Object> native_dir(VM* vm, std::shared_ptr<Tuple> args) {
    if (args->values.size() < 1) {
        throw std::runtime_error("Not enough arguments for dir");
    }

    auto& obj = args->values[0];
    if (obj->attributes) {
        return obj->attributes;
    }
    return vm->globals["null"];
}

std::shared_ptr<Object> native_exit(VM* vm, std::shared_ptr<Tuple> args) {
    if (args->values.size() > 0) {
        auto obj = args->values[0];
        if (dynamic_cast<Int*>(obj.get()) || dynamic_cast<Null*>(obj.get())) {
            exit(obj->type->__int__(obj));
        }
        throw std::runtime_error("exit argument must be an integer or null");
    }
    exit(0);
    return vm->globals["null"];
}

void VM::load_globals() {
    kod_type_type->type = kod_type_type;

    globals["type"] = std::make_shared<Type>("type", kod_type_type);
    globals["int"] = std::make_shared<TypeInt>();
    globals["str"] = std::make_shared<TypeString>();
    globals["tuple"] = std::make_shared<TypeTuple>();
    globals["dict"] = std::make_shared<TypeDict>();

    globals["null"] = std::make_shared<Null>();
    
    globals["globals"] = std::make_shared<NativeFunc>(native_globals, "globals");
    globals["locals"] = std::make_shared<NativeFunc>(native_locals, "locals");
    globals["print"] = std::make_shared<NativeFunc>(native_print, "print");
    globals["input"] = std::make_shared<NativeFunc>(native_input, "input");
    globals["sleep"] = std::make_shared<NativeFunc>(native_sleep, "sleep");
    globals["dir"] = std::make_shared<NativeFunc>(native_dir, "dir");
    globals["exit"] = std::make_shared<NativeFunc>(native_exit, "exit");
}

std::shared_ptr<Object> constant_to_object(Constant const& constant) {
    switch (constant.tag) {
        case ConstantTag::C_NULL: return std::make_shared<Null>();
        case ConstantTag::C_INTEGER: return std::make_shared<Int>(constant._int);
        case ConstantTag::C_TUPLE: return std::make_shared<Tuple>(constant._tuple);
        case ConstantTag::C_ASCII: return std::make_shared<String>(constant._string);
        case ConstantTag::C_CODE: return std::make_shared<CodeObj>(constant._code);
        // case ConstantTag::C_BOOL: return std::make_shared<ObjectBool>(constant._bool);
        // case ConstantTag::C_FLOAT: return std::make_shared<ObjectFloat>(constant._float);
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
        if (call_stack.size() >= 100) {
            throw std::runtime_error("Stack overflow");
        }
        auto opcode = static_cast<Opcode>(frame->code.read8(frame->ip));
        switch (opcode) {
            case Opcode::OP_LOAD_CONST: {
                uint32_t index = frame->code.read32(frame->ip);

                auto obj = constant_objects[index];
                object_stack.push_back(obj);
            } break;

            case Opcode::OP_POP_TOP: {
                auto obj = object_stack.back();
                if (repl && !dynamic_cast<Null*>(obj.get())) {
                    std::cout << obj->type->__str__(obj) << std::endl;
                }
                object_stack.pop_back();
            } break;

            case Opcode::OP_JUMP: {
                uint32_t ip = frame->code.read32(frame->ip);
                frame->ip = ip;
            } break;

            case Opcode::OP_POP_JUMP_IF_FALSE: {
                auto obj = object_stack.back();
                object_stack.pop_back();

                auto ip = frame->code.read32(frame->ip);

                if (!obj->type->__bool__(obj)) {
                    frame->ip = ip;
                }
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
                if (globals.dict.count(std::make_shared<String>(name)) > 0) {
                    auto& obj = globals[name];
                    object_stack.push_back(obj);
                    break;
                }

                throw std::runtime_error("Name not found: " + name);
            } break;

            case Opcode::OP_BUILD_TUPLE: {
                uint32_t count = frame->code.read32(frame->ip);
                std::vector<std::shared_ptr<Object>> values;
                for (uint32_t i = 0; i < count; i++) {
                    auto obj = object_stack.back();
                    object_stack.pop_back();
                    values.push_back(obj);
                }

                std::reverse(values.begin(), values.end());
                auto obj = std::make_shared<Tuple>(values);
                object_stack.push_back(obj);
            } break;

            case Opcode::OP_BINARY_ADD: {
                auto right = object_stack.back();
                object_stack.pop_back();
                auto left = object_stack.back();
                object_stack.pop_back();

                object_stack.push_back(left->type->__add__(left, right));
            } break;

            case Opcode::OP_BINARY_SUB: {
                auto right = object_stack.back();
                object_stack.pop_back();
                auto left = object_stack.back();
                object_stack.pop_back();

                object_stack.push_back(left->type->__sub__(left, right));
            } break;

            case Opcode::OP_BINARY_MUL: {
                auto right = object_stack.back();
                object_stack.pop_back();
                auto left = object_stack.back();
                object_stack.pop_back();

                object_stack.push_back(left->type->__mul__(left, right));
            } break;

            case Opcode::OP_BINARY_BOOLEAN_LESS_THAN: {
                auto right = object_stack.back();
                object_stack.pop_back();
                auto left = object_stack.back();
                object_stack.pop_back();

                object_stack.push_back(left->type->__lt__(left, right));
            } break;

            case Opcode::OP_BINARY_BOOLEAN_GREATER_THAN: {
                auto right = object_stack.back();
                object_stack.pop_back();
                auto left = object_stack.back();
                object_stack.pop_back();

                object_stack.push_back(left->type->__gt__(left, right));
            } break;

            case Opcode::OP_BINARY_BOOLEAN_EQUAL: {
                auto right = object_stack.back();
                object_stack.pop_back();
                auto left = object_stack.back();
                object_stack.pop_back();

                object_stack.push_back(left->type->__eq__(left, right));
            } break;

            case Opcode::OP_LOAD_ATTRIBUTE: {
                auto obj = object_stack.back();
                object_stack.pop_back();

                uint32_t index = frame->code.read32(frame->ip);
                auto& name = module.name_pool[index];
                
                if (!obj->attributes) throw std::runtime_error("Object has no attributes");
                auto attr = obj->attributes->operator[](name);

                if (!attr) {
                    throw std::runtime_error("Attribute not found: " + name);
                }

                object_stack.push_back(attr);
            } break;

            case Opcode::OP_LOAD_ATTRIBUTE_SELF: {
                auto obj = object_stack.back();
                object_stack.pop_back();

                uint32_t index = frame->code.read32(frame->ip);
                auto& name = module.name_pool[index];

                if (!obj->attributes) throw std::runtime_error("Object has no attributes");
                auto attr = obj->attributes->operator[](name);

                if (!attr) {
                    throw std::runtime_error("Attribute not found: " + name);
                }

                object_stack.push_back(attr);
                object_stack.push_back(obj);
            } break;

            case Opcode::OP_CALL: {
                auto arg_count = frame->code.read32(frame->ip);

                std::vector<std::shared_ptr<Object>> args;
                // pop all args and push to front of args
                for (uint32_t i = 0; i < arg_count; i++) {
                    auto obj = object_stack.back();
                    object_stack.pop_back();
                    args.push_back(obj);
                }

                // pop the func
                auto obj = object_stack.back();
                object_stack.pop_back();

                std::reverse(args.begin(), args.end());

                auto tup = std::make_shared<Tuple>(args);
                auto res = obj->type->__call__(this, obj, tup);

                if (dynamic_cast<NativeFunc*>(obj.get()) || dynamic_cast<Type*>(obj.get()) || dynamic_cast<NativeMethod*>(obj.get())) {
                    object_stack.push_back(res);
                    
                } else if (CodeObj* func = dynamic_cast<CodeObj*>(obj.get())) {
                    frame = &call_stack.emplace_back(func->code, func->locals);

                } else {
                    throw std::runtime_error("Function object is not a native/code object");
                }

            } break;

            default: throw std::runtime_error("Unknown opcode: " + Opcode_to_string(opcode));
        }
    }

    return result;
}

std::optional<std::shared_ptr<Object>> VM::search_name(const std::string& name) {
    auto s_obj = std::make_shared<String>(name);
    for (int i = call_stack.size() - 1; i >= 0; i--) {
        if (call_stack[i].locals.dict.count(s_obj) > 0) {
            return call_stack[i].locals[name];
        }
    }
    return {};
}

std::optional<std::shared_ptr<Object>> VM::search_global_name(const std::string& name) {
    // Search the global environment
    auto s_obj = std::make_shared<String>(name);
    if (globals.dict.count(s_obj) > 0) {
        return globals[name];
    }
    return {};
}

}
