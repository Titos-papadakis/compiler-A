#ifndef __ALPHA_EXPRESSION_H__
/**
 * @brief Alpha expressions to be 
 * generated from the parser are handled here
 * and NOT in alpha_ExpressionTypes.h/c
 * 
 * @note It is important to note that each
 * expression's emit function has different handling.
 * This means that each rule has to be handled differently
 * and we require one function for handling each and every
 * one of them.
 */
#define __ALPHA_EXPRESSION_H__

#include "Alpha_ExpressionTypes.h"   /* Expression type field */
#include "Alpha_Symbol.h"            /* Symbol field */
#include "Alpha_IOpcode.h"           /* Symbol field */

extern FILE* out; /* Output file for printing */

/* True boolean macro */
#define ALPHA_TRUE 1

/* False boolean macro */
#define ALPHA_FALSE 0

/* Alpha list for the true and false lists */
typedef struct alpha_list{
    int label;                  /* The quad which contains the jump and it is incomplete */
    struct alpha_list* next;    /* Next list node */
}Alpha_List;

/* Alpha expression object */
typedef struct alpha_expression{
    Alpha_ExpressionType type;          /* Type of expression */
    Alpha_Symbol* sym;                  /* Symbol tied to that expression */
    struct alpha_expression* idx;       /* Index epxression of an indexed table */
    double numConst;                    /* Number constant if expression type is a number */
    char* strConst;                     /* String constant if epxression type is a string */
    bool boolConst;                     /* Boolean constant if expresion type is a boolean */
    struct alpha_expression* next;      /* Next element in a table expression */     
    Alpha_List *Alpha_Truelist;         /* True list for partial evaluation */
    Alpha_List *Alpha_Falselist;  
    Alpha_List* nextlist;               /* False list for partial evaluation */
}Alpha_Expression;

/**
 * @brief Creates a new expression
 * 
 * @param type Type of expression to create
 * based on the rule hit by the parser
 * 
 * @returns The newly created expression object
 */
Alpha_Expression *Alpha_CreateExpression(Alpha_ExpressionType type);

/**
 * @brief Recursively prints an expression
 * and all it's sub-expressions (^0^ Peaky thought of it!!)
 * 
 * @param expr Expression to print, along with its sub-expressions
 * (internal expressions)
 */
void Alpha_PrintExpression(Alpha_Expression* expr);

/**
 * @brief Creates a new expression
 * that uses a temporary expression
 * along with a specified type
 * 
 * @param type Type of epxression
 * 
 * @returns The newly created expression
 */
Alpha_Expression *Alpha_CreateTemporaryExpression(Alpha_ExpressionType type);

/**
 * @brief Return temporary expression
 * for when get_return_val is invoked
 * 
 * @param type Type of expression
 * 
 * @returns The new returned expression
 */
Alpha_Expression *Alpha_CreateRetTempExpression(Alpha_ExpressionType type);


/**
 * @brief Creates a new expression
 * that is purely a constant number
 * 
 * @param num Number value of expression
 * 
 * @returns The newly created expression
 */
Alpha_Expression *Alpha_CreateConstNumExpression(double num);

/**
 * @brief Creates a new expression
 * that is purely a string
 * 
 * @param str String value of expression
 * 
 * @returns The newly created expression
 */
Alpha_Expression *Alpha_CreateConstStringExpression(char* str);

/**
 * @brief Creates a new expression
 * that is purely a boolean
 * 
 * @param b Boolean value of exrepssion
 * 
 * @returns The newly created expression
 */
Alpha_Expression *Alpha_CreateConstBoolExpression(bool b);

/**
 * @brief Create a table expression
 * 
 * @param name Name of the table
 * 
 * @returns the newly created expression
 */
Alpha_Expression* Alpha_ExpressAlpha_CreateTableExpressionionTable(char* name);

/**
 * @brief Creates a new expression
 * that is a table element
 *
 * @returns The newly created expression
 */
Alpha_Expression* Alpha_CreateTableElementExpression(void);

/* POOPIE FUNCTIONS */

/**
 * @brief Creates a new expression
 * based on a variable name
 * 
 * @param name Name of the variable
 * 
 * @returns The newly created expression
 */
Alpha_Expression* Alpha_CreateVariableExpression(char *name);

/**
 * @brief Checks if the variable is temporary
 * 
 * @param sym Symbol to check
 * 
 * @returns ALPHA_SUCCESS if the variable is temporary
 * ALPHA_FAIL if not
 */
Uint8 Alpha_IsTheVariableTemporary(Alpha_Symbol *sym);

#endif