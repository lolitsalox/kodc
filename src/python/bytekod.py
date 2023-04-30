from enum import IntEnum, auto
from dataclasses import dataclass, field
from struct import unpack, calcsize
from io import BufferedIOBase
from typing import Self, Callable, Any
from time import time


class Operation(IntEnum):
    LOAD_CONST = 0
    LOAD_NAME = auto()      
    LOAD_ATTRIBUTE = auto() 
    STORE_NAME = auto()     
    STORE_ATTRIBUTE = auto()
    POP_TOP   = auto()
    UNARY_ADD = auto()
    UNARY_SUB = auto()
    UNARY_NOT = auto()
    UNARY_BOOL_NOT = auto()
    BINARY_ADD = auto()
    BINARY_SUB = auto()
    BINARY_MUL = auto()
    BINARY_DIV = auto()
    BINARY_MOD = auto()
    BINARY_POW = auto()
    BINARY_AND = auto()
    BINARY_OR = auto()
    BINARY_XOR = auto()
    BINARY_LEFT_SHIFT = auto()
    BINARY_RIGHT_SHIFT = auto()
    BINARY_BOOLEAN_AND = auto()
    BINARY_BOOLEAN_OR = auto()
    BINARY_BOOLEAN_EQUAL = auto()
    BINARY_BOOLEAN_NOT_EQUAL = auto()
    BINARY_BOOLEAN_GREATER_THAN = auto()
    BINARY_BOOLEAN_GREATER_THAN_OR_EQUAL_TO = auto()
    BINARY_BOOLEAN_LESS_THAN = auto()
    BINARY_BOOLEAN_LESS_THAN_OR_EQUAL_TO = auto()
    CALL = auto()                
    CALL_METHOD = auto()         
    RETURN = auto()              
    JUMP = auto()                
    POP_JUMP_IF_FALSE = auto()   

class Tag(IntEnum):
    NULL    = 0
    BOOL    = auto()
    INTEGER = auto()
    FLOAT   = auto()
    ASCII   = auto()
    CODE    = auto()

@dataclass
class Code:
    name: str
    params: tuple[str, ...]
    code: bytes

@dataclass
class ConstantInformation:
    tag: Tag
    _int: int | None = None
    _float: float | None = None
    _bool: bool | None = None
    _string: str | None = None
    _code: Code | None = None

def read_until_null(fp) -> bytes:
    data = b""
    while (b := fp.read(1)) != b"\0":
        data += b
    return data

def read_constant(fp: BufferedIOBase) -> ConstantInformation:
    tag = Tag(unpack("i", fp.read(calcsize("i")))[0])
    match tag:
        case Tag.NULL:
            return ConstantInformation(Tag.NULL)
        case Tag.INTEGER:
            return ConstantInformation(Tag.INTEGER, _int=unpack("q", (fp.read(calcsize("q"))))[0])
        case Tag.FLOAT:
            return ConstantInformation(Tag.FLOAT, _float=unpack("d", fp.read(calcsize("d")))[0])
        case Tag.ASCII:
            return ConstantInformation(Tag.ASCII, _string=read_until_null(fp).decode())
        case Tag.CODE:
            name = read_until_null(fp).decode()
            param_size = unpack("Q", fp.read(calcsize("Q")))[0]
            params = [read_until_null(fp).decode() for _ in range(param_size)]
            code_size = unpack("Q", fp.read(calcsize("Q")))[0]
            code = fp.read(code_size)
            return ConstantInformation(Tag.CODE, _code=Code(name, params, code))

class KodModule:
    def __init__(self, file_name: str) -> None:
        with open(file_name, "rb") as f:
            file_name = read_until_null(f)
            major_version = unpack("H", f.read(calcsize("H")))[0]
            minor_version = unpack("H", f.read(calcsize("H")))[0]
            name_pool_size = unpack("Q", f.read(calcsize("Q")))[0]
            name_pool = [read_until_null(f).decode() for _ in range(name_pool_size)]
            constant_pool_size = unpack("Q", f.read(calcsize("Q")))[0]
            constant_pool = [read_constant(f) for _ in range(constant_pool_size)]
            entry_size = unpack("Q", f.read(calcsize("Q")))[0]
            entry_code = f.read(entry_size)
        
        self.file_name = file_name
        self.major_version = major_version
        self.minor_version = minor_version
        self.name_pool = name_pool
        self.constant_pool = constant_pool
        self.entry = Code("__main__", [], entry_code)

class Object:
    def __init__(self, attributes: dict[str, Self]) -> None:
        self.attributes = attributes
        self.ref_count = 1
    
    def ref(self):
        self.ref_count += 1
    
    def deref(self) -> bool:
        self.ref_count -= 1
        return self.ref_count <= 0

    def __bool__(self) -> bool:
        return True

class NullObject(Object):
    def __init__(self) -> None:
        attributes = {

        }
        super().__init__(attributes)

    def __bool__(self) -> bool:
        return False

    def __str__(self) -> str:
        return "null"

class BoolObject(Object):
    def __init__(self, value: bool) -> None:
        attributes = {

        }
        super().__init__(attributes)
        self.value = value
    
    def __bool__(self) -> bool:
        return self.value

class IntObject(Object):
    def __init__(self, value: int) -> None:
        attributes = native_int_methods  # not just methods though
        super().__init__(attributes)
        self.value = value
    
    def __bool__(self) -> bool:
        return not not self.value
    
    def __str__(self) -> str:
        return str(self.value)
    
class FloatObject(Object):
    def __init__(self, value: float) -> None:
        attributes = {

        }
        super().__init__(attributes)
        self.value = value
        
    def __bool__(self) -> bool:
        return not not self.value
    
    def __str__(self) -> str:
        return str(self.value)

class StringObject(Object):
    def __init__(self, value: str) -> None:
        attributes = {
            # "__str__": NativeMethodObject
        }
        super().__init__(attributes)
        self.value = value

    def __bool__(self) -> bool:
        return not not self.value
    
    def __str__(self) -> str:
        return self.value
        

class CodeObject(Object):
    def __init__(self, value: Code) -> None:
        attributes = {

        }
        super().__init__(attributes)
        self.value = value

class NativeFunctionObject(Object):
    def __init__(self, name: str, function: Callable[[Any], Object]) -> None:
        attributes = {
            "__name__": StringObject(name)
        }
        super().__init__(attributes)
        self.name = name
        self.function = function

def kod_native_print(args: list[Object]) -> Object:
    print(*map(str, args))
    return NullObject()

def kod_native_time(args: list[Object]) -> IntObject:
    return IntObject(round(time()))

native_functions_dict = {
    "print": NativeFunctionObject("print", kod_native_print),
    "time": NativeFunctionObject("time", kod_native_time),
}

def kod_native_int_method_unary_add(self: IntObject) -> IntObject:
    return IntObject(self.value)

def kod_native_int_method_unary_sub(self: IntObject) -> IntObject:
    return IntObject(-self.value)

def kod_native_int_method_unary_not(self: IntObject) -> IntObject:
    return IntObject(~self.value)

def kod_native_int_method_unary_bool_not(self: IntObject) -> IntObject:
    return IntObject(+(not self.value))

def kod_native_int_method_add(self: IntObject, other: Object) -> IntObject | FloatObject:
    if not isinstance(other, (IntObject, FloatObject)):
        raise RuntimeError()
    return IntObject(self.value + other.value) if isinstance(other, IntObject) else FloatObject(self.value + other.value)

def kod_native_int_method_sub(self: IntObject, other: Object) -> IntObject | FloatObject:
    if not isinstance(other, (IntObject, FloatObject)):
        raise RuntimeError()
    return IntObject(self.value - other.value) if isinstance(other, IntObject) else FloatObject(self.value - other.value)

def kod_native_int_method_mul(self: IntObject, other: Object) -> IntObject | FloatObject:
    if not isinstance(other, (IntObject, FloatObject)):
        raise RuntimeError()
    return IntObject(self.value * other.value) if isinstance(other, IntObject) else FloatObject(self.value * other.value)

def kod_native_int_method_lt(self: IntObject, other: Object) -> IntObject | FloatObject:
    if not isinstance(other, (IntObject, FloatObject)):
        raise RuntimeError()
    return IntObject(self.value < other.value) if isinstance(other, IntObject) else FloatObject(self.value < other.value)


native_int_methods = {
    "__unary_add__": NativeFunctionObject("__unary_add__", kod_native_int_method_unary_add),
    "__unary_sub__": NativeFunctionObject("__unary_sub__", kod_native_int_method_unary_sub),
    "__unary_not__": NativeFunctionObject("__unary_not__", kod_native_int_method_unary_not),
    "__unary_bool_not__": NativeFunctionObject("__unary_bool_not__", kod_native_int_method_unary_bool_not),
    "__add__": NativeFunctionObject("__add__", kod_native_int_method_add),
    "__sub__": NativeFunctionObject("__sub__", kod_native_int_method_sub),
    "__mul__": NativeFunctionObject("__mul__", kod_native_int_method_mul),
    "__lt__": NativeFunctionObject("__lt__", kod_native_int_method_lt),
}

@dataclass
class CallFrame:
    parent: Self | None = None
    environment: dict[str, Object] = field(default_factory=dict)
    stack: list[Object] = field(default_factory=list)
    ip: int = 0

class VirtualMachine:
    def __init__(self, module: KodModule) -> None:
        self.module = module
    
    def run(self) -> None:
        global_frame = CallFrame(environment=native_functions_dict)
        self.run_code_object(self.module.entry, global_frame)
    
    def run_code_object(self, code: Code, parent_call_frame: CallFrame | None, initial_env: dict[str, Object] | None = None) -> Object | None:
        return_value: Object = NullObject()
        frame = CallFrame(parent_call_frame)
        if initial_env is not None:
            frame.environment.update(initial_env)
        code_size = len(code.code)
        while frame.ip < code_size:
            match code.code[frame.ip]:
                case Operation.POP_TOP:
                    frame.ip += 1
                    frame.stack.pop()
                case Operation.LOAD_CONST:
                    frame.ip += 1
                    index = unpack("Q", code.code[frame.ip:frame.ip + calcsize("Q")])[0]
                    frame.ip += calcsize("Q")
                    constant: ConstantInformation = self.module.constant_pool[index]
                    match constant.tag:
                        case Tag.NULL:
                            obj = NullObject()
                        case Tag.BOOL:
                            obj = BoolObject(constant._bool)
                        case Tag.INTEGER:
                            obj = IntObject(constant._int)
                        case Tag.FLOAT:
                            obj = FloatObject(constant._float)
                        case Tag.ASCII:
                            obj = StringObject(constant._string)
                        case Tag.CODE:
                            obj = CodeObject(constant._code)
                        case _:
                            print(f"Uknown tag {constant.tag}")
                    frame.stack.append(obj)
                case Operation.STORE_NAME:
                    frame.ip += 1
                    index = unpack("Q", code.code[frame.ip:frame.ip + calcsize("Q")])[0]
                    frame.ip += calcsize("Q")
                    obj = frame.stack.pop()
                    name = self.module.name_pool[index]
                    if name in frame.environment:
                        if frame.environment[name].deref():
                            del frame.environment[name]
                    obj.ref()
                    frame.environment[name] = obj
                case Operation.LOAD_NAME:
                    frame.ip += 1
                    index = unpack("Q", code.code[frame.ip:frame.ip + calcsize("Q")])[0]
                    frame.ip += calcsize("Q")
                    name = self.module.name_pool[index]
                    curr_frame = frame
                    while name not in curr_frame.environment:
                        curr_frame = curr_frame.parent
                        if curr_frame is None:
                            raise NameError(f"'{name}' is not defined")
                    obj = curr_frame.environment[name]
                    frame.stack.append(obj)
                case Operation.CALL:
                    frame.ip += 1
                    arg_count = unpack("Q", code.code[frame.ip:frame.ip + calcsize("Q")])[0]
                    frame.ip += calcsize("Q")
                    obj = frame.stack.pop() # popping function object
                    obj.ref()
                    args = [frame.stack.pop() for _ in range(arg_count)]
                    for arg in args:
                        arg.ref()
                    if isinstance(obj, NativeFunctionObject):
                        obj: NativeFunctionObject
                        ret_val = obj.function(args)
                        frame.stack.append(ret_val)
                    elif isinstance(obj, CodeObject):
                        obj: CodeObject
                        ret_val = self.run_code_object(obj.value, frame, dict(zip(obj.value.params, args)))
                        frame.stack.append(ret_val)
                    else:
                        raise RuntimeError
                    obj.deref()
                case Operation.RETURN:
                    frame.ip += 1
                    return_value = frame.stack.pop() # popping result object
                    frame.ip = len(code.code)
                case Operation.JUMP:
                    frame.ip += 1
                    addr = unpack("Q", code.code[frame.ip:frame.ip + calcsize("Q")])[0]
                    frame.ip = addr
                case Operation.POP_JUMP_IF_FALSE:
                    frame.ip += 1
                    addr = unpack("Q", code.code[frame.ip:frame.ip + calcsize("Q")])[0]
                    obj = frame.stack.pop()
                    if obj:
                        frame.ip += calcsize("Q")
                    else:
                        frame.ip = addr
                case Operation.UNARY_ADD:
                    frame.ip += 1
                    obj = frame.stack.pop()
                    if "__unary_add__" in obj.attributes:
                        call_obj = obj.attributes["__unary_add__"]
                        if isinstance(call_obj, CodeObject):
                            call_obj: CodeObject
                            value = self.run_code_object(call_obj.value, frame, {"self": obj})
                        elif isinstance(call_obj, NativeFunctionObject):
                            call_obj: NativeFunctionObject
                            value = call_obj.function(obj)
                        else:
                            raise RuntimeError()
                        frame.stack.append(value)
                    else:
                        raise RuntimeError()
                case Operation.UNARY_SUB:
                    frame.ip += 1
                    obj = frame.stack.pop()
                    if "__unary_sub__" in obj.attributes:
                        call_obj = obj.attributes["__unary_sub__"]
                        if isinstance(call_obj, CodeObject):
                            call_obj: CodeObject
                            value = self.run_code_object(call_obj.value, frame, {"self": obj})
                        elif isinstance(call_obj, NativeFunctionObject):
                            call_obj: NativeFunctionObject
                            value = call_obj.function(obj)
                        else:
                            raise RuntimeError()
                        frame.stack.append(value)
                    else:
                        raise RuntimeError()
                case Operation.UNARY_NOT:
                    frame.ip += 1
                    obj = frame.stack.pop()
                    if "__unary_not__" in obj.attributes:
                        call_obj = obj.attributes["__unary_not__"]
                        if isinstance(call_obj, CodeObject):
                            call_obj: CodeObject
                            value = self.run_code_object(call_obj.value, frame, {"self": obj})
                        elif isinstance(call_obj, NativeFunctionObject):
                            call_obj: NativeFunctionObject
                            value = call_obj.function(obj)
                        else:
                            raise RuntimeError()
                        frame.stack.append(value)
                    else:
                        raise RuntimeError()
                case Operation.UNARY_BOOL_NOT:
                    frame.ip += 1
                    obj = frame.stack.pop()
                    if "__unary_bool_not__" in obj.attributes:
                        call_obj = obj.attributes["__unary_bool_not__"]
                        if isinstance(call_obj, CodeObject):
                            call_obj: CodeObject
                            value = self.run_code_object(call_obj.value, frame, {"self": obj})
                        elif isinstance(call_obj, NativeFunctionObject):
                            call_obj: NativeFunctionObject
                            value = call_obj.function(obj)
                        else:
                            raise RuntimeError()
                        frame.stack.append(value)
                    else:
                        raise RuntimeError()
                case Operation.BINARY_ADD:
                    frame.ip += 1
                    right = frame.stack.pop()
                    left = frame.stack.pop()
                    if "__add__" in left.attributes:
                        call_obj = left.attributes["__add__"]
                        if isinstance(call_obj, CodeObject):
                            call_obj: CodeObject
                            value = self.run_code_object(call_obj.value, frame, {"self": left, "other": right})
                        elif isinstance(call_obj, NativeFunctionObject):
                            call_obj: NativeFunctionObject
                            value = call_obj.function(left, right)
                        else:
                            raise RuntimeError()
                        frame.stack.append(value)
                    else:
                        raise RuntimeError()
                case Operation.BINARY_SUB:
                    frame.ip += 1
                    right = frame.stack.pop()
                    left = frame.stack.pop()
                    if "__sub__" in left.attributes:
                        call_obj = left.attributes["__sub__"]
                        if isinstance(call_obj, CodeObject):
                            call_obj: CodeObject
                            value = self.run_code_object(call_obj.value, frame, {"self": left, "other": right})
                        elif isinstance(call_obj, NativeFunctionObject):
                            call_obj: NativeFunctionObject
                            value = call_obj.function(left, right)
                        else:
                            raise RuntimeError()
                        frame.stack.append(value)
                    else:
                        raise RuntimeError()
                case Operation.BINARY_MUL:
                    frame.ip += 1
                    right = frame.stack.pop()
                    left = frame.stack.pop()
                    if "__mul__" in left.attributes:
                        call_obj = left.attributes["__mul__"]
                        if isinstance(call_obj, CodeObject):
                            call_obj: CodeObject
                            value = self.run_code_object(call_obj.value, frame, {"self": left, "other": right})
                        elif isinstance(call_obj, NativeFunctionObject):
                            call_obj: NativeFunctionObject
                            value = call_obj.function(left, right)
                        else:
                            raise RuntimeError()
                        frame.stack.append(value)
                    else:
                        raise RuntimeError()
                # case Operation.BINARY_DIV:
                # case Operation.BINARY_MOD:
                # case Operation.BINARY_POW:
                # case Operation.BINARY_AND:
                # case Operation.BINARY_OR:
                # case Operation.BINARY_XOR:
                # case Operation.BINARY_LEFT_SHIFT:
                # case Operation.BINARY_RIGHT_SHIFT:
                # case Operation.BINARY_BOOLEAN_AND:
                # case Operation.BINARY_BOOLEAN_OR:
                # case Operation.BINARY_BOOLEAN_EQUAL:
                # case Operation.BINARY_BOOLEAN_NOT_EQUAL:
                # case Operation.BINARY_BOOLEAN_GREATER_THAN:
                # case Operation.BINARY_BOOLEAN_GREATER_THAN_OR_EQUAL_TO:
                case Operation.BINARY_BOOLEAN_LESS_THAN:
                    frame.ip += 1
                    right = frame.stack.pop()
                    left = frame.stack.pop()
                    if "__lt__" in left.attributes:
                        call_obj = left.attributes["__lt__"]
                        if isinstance(call_obj, CodeObject):
                            call_obj: CodeObject
                            value = self.run_code_object(call_obj.value, frame, {"self": left, "other": right})
                        elif isinstance(call_obj, NativeFunctionObject):
                            call_obj: NativeFunctionObject
                            value = call_obj.function(left, right)
                        else:
                            raise RuntimeError()
                        frame.stack.append(value)
                    else:
                        raise RuntimeError()
                # case Operation.BINARY_BOOLEAN_LESS_THAN_OR_EQUAL_TO:
                case op:
                    op = Operation(op)
                    print(f"Operation {op.name} has not been implemented yet.")
                    break
        # print(frame.stack)
        # for obj in frame.stack:  # for every leftover object in the stack
        #     if obj.deref():  # dereference
        #         if obj != return_value:  # unless it's the return value, free
        #             del obj  # free_object(obj);
        # frame.stack.clear()
        return return_value

def main():
    module = KodModule("out.bkod")
    vm = VirtualMachine(module)
    vm.run()

if __name__ == "__main__":
    main()