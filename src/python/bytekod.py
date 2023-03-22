from enum import IntEnum, auto
from dataclasses import dataclass
from struct import unpack, calcsize
from io import BufferedIOBase
from typing import Self, Callable


class Operation(IntEnum):
    LOAD_CONST = 0
    LOAD_NAME = auto()      
    LOAD_ATTRIBUTE = auto() 
    STORE_NAME = auto()     
    STORE_ATTRIBUTE = auto()
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
    params: list[str]
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
            file_name_size = unpack("Q", f.read(calcsize("Q")))[0]
            file_name = f.read(file_name_size)[:-1].decode()
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

class NullObject(Object):
    def __init__(self) -> None:
        attributes = {

        }
        super().__init__(attributes)

class BoolObject(Object):
    def __init__(self, value: bool) -> None:
        attributes = {

        }
        super().__init__(attributes)
        self.value = value

class IntObject(Object):
    def __init__(self, value: int) -> None:
        attributes = {
            "__add__": native_int_method_add, # is a native method object
        }
        super().__init__(attributes)
        self.value = value

class FloatObject(Object):
    def __init__(self, value: float) -> None:
        attributes = {

        }
        super().__init__(attributes)
        self.value = value

class StringObject(Object):
    def __init__(self, value: str) -> None:
        attributes = {

        }
        super().__init__(attributes)
        self.value = value

class CodeObject(Object):
    def __init__(self, value: Code) -> None:
        attributes = {

        }
        super().__init__(attributes)
        self.value = value

@dataclass
class NativeFunction:
    name: str
    function: Callable[[list[Object]], Object]

class NativeFunctionObject(Object):
    def __init__(self, value: NativeFunction) -> None:
        attributes = {

        }
        super().__init__(attributes)
        self.value = value



@dataclass
class CallFrame:
    parent: Self | None
    environment: dict[str, Object]
    stack: list[Object]
    ip: int = 0

class VirtualMachine:
    def __init__(self, module: KodModule) -> None:
        self.module = module
    
    def run(self) -> None:
        self.run_code_object(self.module.entry, None)
    
    def run_code_object(self, code: Code, parent_call_frame: CallFrame | None) -> Object | None:
        return_value: Object | None = None
        frame = CallFrame(None, {}, [])
        code_size = len(code.code)
        while frame.ip < code_size:
            match code.code[frame.ip]:
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
                    frame.environment[self.module.name_pool[index]] = obj
                case op:
                    op = Operation(op)
                    print(f"Operation {op.name} has not been implemented yet.")
                    break
        print(frame.environment)
        return return_value

def main():
    module = KodModule("out.bkod")
    vm = VirtualMachine(module)
    vm.run()

if __name__ == "__main__":
    main()