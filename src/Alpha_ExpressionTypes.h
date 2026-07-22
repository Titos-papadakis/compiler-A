#ifndef __ALPHA_EXPRESSION_TYPES_H__
/**
 * @brief Different types of expressions
 * that can be generated in alpha lang
 * 
 * @note Here we handle only
 * the TYPES of expressions and not
 * the expressions themselves (s: alpha_Expressions.h/c)
 */
#define __ALPHA_EXPRESSION_TYPES_H__

#include "Alpha_Symbol.h"

 /* Types of alpha expressions */
typedef enum alpha_expressionType{
    Alpha_Expression_Variable,          /* Variable (local x;) */
    Alpha_Expression_Table_item,        /* Table item (x[i])*/
    Alpha_Expression_Program_function,  /* Program function (foo();) */
    Alpha_Expression_Library_function,  /* Lib function (print();) */
    Alpha_Expression_Arithmetic,        /* Arithmetic expression (x + 5;) */
    Alpha_Expression_Boolean,           /* Boolean expression (x == 5;) */
    Alpha_Expression_Assignment,        /* Assignment (x = y;) */
    Alpha_Expression_New_table,         /* Table creation (local t[20];) */
    Alpha_Expression_Const_number,      /* Constant number (p = 5;) */
    Alpha_Expression_Const_bool,        /* Constant boolean (p = TRUE;) */
    Alpha_Expression_Const_string,      /* Constant string (p = "Hello_Peaky\n";) */
    Alpha_Expression_Nil                /* Null expression (p = ALPHA_NIL;) */
}Alpha_ExpressionType;

/**
 * @brief Evaluates what string matches
 * to each expression type
 * 
 * @param type Type of expression
 * 
 * @returns The string of the expression type
 */
const char* Alpha_ExpressionTypeString(Alpha_ExpressionType type);

#endif