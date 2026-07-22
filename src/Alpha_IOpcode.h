#ifndef __ALPHA_IOPCODE_H__
/**
 * @brief This file handles instruction
 * opcodes for genereating intermediate
 * code from the alpha parser
 */
#define __ALPHA_IOPCODE_H__

/* Alpha lang instruction opcodes for handling generated instructions */
typedef enum alpha_iopcode{
    Alpha_InstructionOpcode_Assign,              /* = */
    Alpha_InstructionOpcode_Add,                 /* + */
    Alpha_InstructionOpcode_Sub,                 /* - */
    Alpha_InstructionOpcode_Div,                 /* / */
    Alpha_InstructionOpcode_Mult,                /* * */
    Alpha_InstructionOpcode_Mod,                 /* % */
    Alpha_InstructionOpcode_If_eq,               /* == */
    Alpha_InstructionOpcode_If_not_eq,           /* != */
    Alpha_InstructionOpcode_If_less_eq,          /* <= */
    Alpha_InstructionOpcode_If_greater_eq,       /* >= */
    Alpha_InstructionOpcode_If_less,             /* < */
    Alpha_InstructionOpcode_If_greater,          /* > */
    Alpha_InstructionOpcode_Call,                /* foo() */
    Alpha_InstructionOpcode_Param,               /* foo(a, b) // a and b are parameters */
    Alpha_InstructionOpcode_Func_start,          /* function boo(){ .....  */
    Alpha_InstructionOpcode_Func_end,            /* ..... } end of function boo? */
    Alpha_InstructionOpcode_Table_create,        /* local t[50]; */
    Alpha_InstructionOpcode_Table_get_element,   /* x = t[0]; */
    Alpha_InstructionOpcode_Table_set_element,   /* t[0] = 2; */
    Alpha_InstructionOpcode_Jump,                /* j xxx */
    Alpha_InstructionOpcode_Noop,                /* */
    Alpha_InstructionOpcode_And,                 /* and */
    Alpha_InstructionOpcode_Or,                  /* or */
    Alpha_InstructionOpcode_Not,                 /* not */
    Alpha_InstructionOpcode_Get_ret_val,         /* return xp; // value returned is xp */
    Alpha_InstructionOpcode_Ret,                 /* return */
    Alpha_InstructionOpcode_Uminus               /* -- */
}Alpha_IOpcode;

#define Alpha_InstructionOpcode Alpha_IOpcode /* Alpha lang instruction opcodes */

/**
 * @brief Returns the opcode string
 * of an opcode for printing
 * 
 * @param opcode Opcode to return the string of
 * 
 * @returns the opcode string
 */
const char* Alpha_IOpcodeToString(Alpha_IOpcode opcode);

#endif
