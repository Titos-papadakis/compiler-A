#include "Alpha_IOpcode.h"

const char* Alpha_IOpcodeToString(Alpha_IOpcode opcode) {
    switch (opcode) {
        case Alpha_InstructionOpcode_Assign:                return "assign";
        case Alpha_InstructionOpcode_Add:                   return "add";
        case Alpha_InstructionOpcode_Sub:                   return "sub";
        case Alpha_InstructionOpcode_Mult:                  return "mult";
        case Alpha_InstructionOpcode_Div:                   return "div";
        case Alpha_InstructionOpcode_Mod:                   return "mod";
        case Alpha_InstructionOpcode_Uminus:                return "uminus";
        case Alpha_InstructionOpcode_And:                   return "and";
        case Alpha_InstructionOpcode_Or:                    return "or";
        case Alpha_InstructionOpcode_Not:                   return "not";
        case Alpha_InstructionOpcode_If_eq:                 return "if_eq";
        case Alpha_InstructionOpcode_If_not_eq:             return "if_neq";
        case Alpha_InstructionOpcode_If_less_eq:            return "if_le";
        case Alpha_InstructionOpcode_If_greater_eq:         return "if_ge";
        case Alpha_InstructionOpcode_If_less:               return "if_lt";
        case Alpha_InstructionOpcode_If_greater:            return "if_gt";
        case Alpha_InstructionOpcode_Call:                  return "call";
        case Alpha_InstructionOpcode_Param:                 return "param";
        case Alpha_InstructionOpcode_Ret:                   return "ret";
        case Alpha_InstructionOpcode_Get_ret_val:           return "getretval";
        case Alpha_InstructionOpcode_Func_start:            return "funcstart";
        case Alpha_InstructionOpcode_Func_end:              return "funcend";
        case Alpha_InstructionOpcode_Table_create:          return "tablecreate";
        case Alpha_InstructionOpcode_Table_get_element:     return "tablegetelem";
        case Alpha_InstructionOpcode_Table_set_element:     return "tablesetelem";
        case Alpha_InstructionOpcode_Jump:                  return "jump";
        default:                                            return "unknown";
    }
}