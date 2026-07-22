#ifndef __ALPHA_QUAD_H__
/**
 * @brief Alpha quads which contain most generated
 * code data are handled here
 */
#define __ALPHA_QUAD_H__

#include "Alpha_IOpcode.h"          /* Opcode field of the quad */
#include "Alpha_Expression.h"       /* Expression field of the quad */

/* Quad struct, main object that handles intermediate code generation */
typedef struct alpha_quad{
    Alpha_IOpcode op;           /* Opcode */
    Alpha_Expression* result;   /* rd */
    Alpha_Expression* arg1;     /* rs1 */
    Alpha_Expression* arg2;     /* rs2 */
    Sint32 label;               /* Label */
    Uint32 line;                /* Line */
    Uint32 taddress;         
}Alpha_Quad;

extern Alpha_Quad** Alpha_Quads;        /* Quads dynmaic array (stupid) */
extern Uint32 Alpha_TotalQuads;         /* Total number of quads */ 
extern Uint32 Alpha_CurrentQuad;        /* Currently parsed quad */

/* Initial maximum number of quads to allocate, before reallocating (ugh...) */
#define ALPHA_INITIAL_QUADS_LIMIT 64

/**
 * @brief Initial quads array
 * allocation based on the 
 * ALPHA_INITIAL_QUADS_LIMIT macro
 */
void Alpha_AllocateQuadsArray(void);

/**
 * @brief Reallocates memory for the
 * quads array if the initial limit
 * has been reached.
 * 
 * @param quadCount Total count of quads (If > ALPHA_INITIAL_QUADS_LIMIT => realloc(Alpha_Quad))
 * 
 * @returns The newly allocated quads dynamic array
 */
Alpha_Quad** Alpha_ReallocateQuadsArray(unsigned quadCount);

/**
 * @brief Creates a new alpha quad
 * with a specified opcode
 * argument1,2 and result
 * 
 * @param op Instruction opcode
 * @param arg1 Argument 1 in expression
 * @param arg2 Argument 2 in expression
 * @param res Result of Instruction
 * 
 * @returns The newly created quad
 */
Alpha_Quad* Alpha_CreateQuad(Alpha_IOpcode op, Alpha_Expression* arg1, Alpha_Expression* arg2, Alpha_Expression* res);

/* Name macro (fuck this name) */
#define Alpha_Emit Alpha_CreateQuad

/**
 * @brief Inserts a quad onto the quads
 * dynamic list
 * 
 * @param quad Quad to insert
 */
void Alpha_InsertQuad(Alpha_Quad* quad);

/**
 * @brief Prints all quads
 * saved in the Alpha_Quads
 * dynamic array
 */
void Alpha_PrintAllQuads(void);

/**
 * @brief List backpatching
 * 
 * @param head Head of the list 
 * @param quadLabel Quad label
 */
void Alpha_Backpatch(Alpha_List *head, int quadLabel);

/**
 * @brief Merges 2 lists together
 * 
 * @param list1 List1 to merge
 * @param list2 List2 to merge
 * 
 * @returns The merged list
 */
Alpha_List* Alpha_Merge(Alpha_List *list1, Alpha_List * list2);

/**
 * @brief Creates a list, with a specific quad label
 * as a node
 * 
 * @param nextQuadLabel Next label
 * 
 * @returns The newly allocated list
 */
Alpha_List* Alpha_MakeList(int nextQuadLabel);

/**
 * @brief  Creates a new Alpha_Expression to represent a boolean constant.
 * 
 * 
 * @param value The boolean value to store (e.g., 0 for false, 1 for true).
 * @return A pointer to the newly created Alpha_Expression structure.
 * The caller is responsible for freeing the allocated memory
 * when it is no longer needed.
 * 
 */
Alpha_Expression* Alpha_CreateBoolExpression(int value);

#endif
