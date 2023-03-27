// #include "vm.h"

// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>

// CallFrame init_call_frame(CallFrame* parent_call_frame) {
//     return (CallFrame){
//         .parent=parent_call_frame,
//         .env=(Environment){
//             .locals={0}
//         },
//         .stack=(stack_t){
//             .item_size=sizeof(void*),
//             .items=NULL,
//             .size=0
//         }
//         .ip=0
//     };
// }

// CallFrame* new_call_frame(CallFrame* parent_call_frame) {
//     CallFrame* call_frame = malloc(sizeof(CallFrame));
//     *call_frame = init_call_frame(parent_call_frame);
//     return call_frame;
// }

// void free_call_frame(CallFrame* call_frame) {
//     free(call_frame);
// }

// VirtualMachine init_virtual_machine(CompiledModule* compiled_module) {
//     return (VirtualMachine){
//         .compiled_module=compiled_module,
//         .stack={.item_size=sizeof(void*), .items=NULL,.size=0},
//         .call_stack={.item_size=sizeof(void*), .items=NULL, .size=0}
//     };
// }


// VirtualMachine* new_virtual_machine(CompiledModule* compiled_module) {
//     VirtualMachine* virtual_machine = malloc(sizeof(VirtualMachine));
//     *virtual_machine = init_virtual_machine(compiled_module);
//     return virtual_machine;
// }

// void free_virtual_machine(VirtualMachine* virtual_machine) {
//     if (!virtual_machine) return;
//     // TODO: implement free_stack in stack.c and add its declaration in stack.h
//     free_stack(&virtual_machine->stack);
//     free_stack(&virtual_machine->call_stack);
//     free(virtual_machine);
// }

// Object* run_code_object(Code* code, CallFrame* parent_call_frame, ConstPool* constant_pool, NamePool* name_pool) {
//     Object* return_value = NULL;
//     CallFrame* frame = new_call_frame(parent_call_frame);
//     for (size_t* ip = &frame->ip; *ip < code->size;) {
//         switch (code->code[*ip]) {
//             case OP_LOAD_CONST:
//                 // TODO: implement
//                 stack_push(&frame->stack, );
//                 frame->stack
//                 break;
//         }
//     }
//     return return_value;
// }

// void run_virtual_machine(VirtualMachine* virtual_machine) {
//     run_code_object(&virtual_machine->compiled_module->entry, NULL, &virtual_machine->compiled_module->constant_pool, &virtual_machine->compiled_module->name_pool);
// }