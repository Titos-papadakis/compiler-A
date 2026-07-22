#include "Alpha_ExpressionTypes.h"

const char* Alpha_ExpressionTypeString(Alpha_ExpressionType type){
    switch (type)
    {
    case Alpha_Expression_Arithmetic:               return "ARITHMETIC";        
    case Alpha_Expression_Assignment:               return "ASSIGNMENT";
    case Alpha_Expression_Boolean:                  return "BOOLEAN";
    case Alpha_Expression_Const_bool:               return "CONST_BOOLEAN";
    case Alpha_Expression_Const_number:             return "CONST_NUMBER";
    case Alpha_Expression_Const_string:             return "CONST_STRING"; /* woohoo */
    case Alpha_Expression_Library_function:         return "LIBRARY_FUNCTION";
    case Alpha_Expression_New_table:                return "NEW_TABLE";
    case Alpha_Expression_Nil:                      return "NIL";
    case Alpha_Expression_Program_function:         return "FUNCTION";
    case Alpha_Expression_Table_item:               return "TABLE_ELEMENT";
    case Alpha_Expression_Variable:                 return "VARIABLE";
    default:
        AERR("Cannot evaluate expression type %d, token %s at line %d\n", type, yytext, yylineno);
        return "UNKOWN_EXPRESSION";
    }
}