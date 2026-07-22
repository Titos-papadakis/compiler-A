#ifndef __ALPHA_SYMBOL_H__
/**
 * @brief Structure for producing symbols
 * via reading the tokens parsed, this file works
 * in connection with the Alpha_Parser.y yakk-bison file
 */
#define __ALPHA_SYMBOL_H__

#include "Alpha_Utilities.h"

/* Allows us to evaluate the scope where a symbol was created, 
it increments and decrements */
extern Uint32 Alpha_CurrentScope;

/* The global block is a global counter, that depending on the blocks 
created, it will only increment */
extern Uint32 Alpha_GlobalBlock;

/* Counter for unnamed functions (_f0, _f1, _f2, ect) */
extern Uint32 Alpha_UnnamedFunctionCounter;

/* When the keyword 'function' is seen, the proxy saves it's line */
extern Uint32 Alpha_FunctionLineProxy;

/* Check for when calling continue/break */
extern Uint32 Alpha_InLoop;

/* Check for when calling return */
extern Uint32 Alpha_InFunc;

/* Check if we are accessing a member from a a function call, where assignment is legal */
extern bool Alpha_MemberAccess;

/* Temporary symbol counter */
extern Uint32 Alpha_TemporaryCounter;

/* Symbol types for indentifiers */
typedef enum Alpha_SymbolType{
    ALPHA_SYMTYPE_LOCAL,                /* Local variable, sees in one specific scope, meaning we cane have the same name in different scopes */
    ALPHA_SYMTYPE_GLOBAL,               /* Global variable, can be seen from any other scope */
    ALPHA_SYMTYPE_PARAM,                /* Function parameter type */
    ALPHA_SYMTYPE_FUNCTION,             /* User function */
    ALPHA_SYMTYPE_TABLE,                /* Table type, temporary to surpress errors */
    ALPHA_SYMTYPE_TEMP                  /* Temporary symbol, use only for expressions */
}Alpha_SymbolType;

/* Symbol object to moreover handle input, and check
if our syntax is correct */
typedef struct Alpha_Symbol{
    char* symbolName;                   /* Name of the symbol */
    bool active;                        /* Check if the symbol is active, meaning we are on the correct scope */
    Uint32 line;                        /* Line created at */
    Uint32 scope;                       /* Scope created at */
    Uint32 block;                       /* Block created at */
    Alpha_SymbolType type;              /* SYmbol type, based on where and how it was created */
    struct Alpha_Symbol* nextInBucket;  /* Pointer to the next symbol, inside the same bucket */
    struct Alpha_Symbol* nextInScope;   /* Pointer to the next symbol, of the same scope */
}Alpha_Symbol;

/* The amount of buckets (array cells) of our symbol table */
#define ALPHA_SYMBOL_TABLE_BUCKETS 64

extern Alpha_Symbol *Alpha_Symbols[ALPHA_SYMBOL_TABLE_BUCKETS]; /* Symbol table */

extern char* Alpha_TypeNames[4]; /* Type names */

/**
 * @brief Hash function for symbol table entry
 * 
 * @param symbolName Name of symbol to hash with
 * 
 * @returns The index of the bucket we have to visit
 */
Uint8 Alpha_HashFunction(char* symbolName);

/**
 * @brief Creates a new symbol
 * and allocates memory to it
 *
 * @param name Name of the symbol
 * @param type Type of symbol (function or variable)
 * 
 * @returns The created symbol
 */
Alpha_Symbol* Alpha_CreateSymbol(char *name, Alpha_SymbolType type);

/**
 * @brief Hash function for symbol table entry
 * 
 * @param name Name of symbol to hash with
 * 
 * @returns The index of the bucket we have to visit
 */
#define h(name) Alpha_HashFunction(name)

/**
 * @brief Allocate memory for bucket heads
 * inside the symbol table
 *
 * @returns 0 on success, -1 on failure
 */
Sint8 Alpha_AllocateSymbolTableBuckets(void);

/**
 * @brief Inserts a symbol inside
 * the symbol table
 * 
 * @param sym Symbol to insert
 */
void Alpha_InsertSymbol(Alpha_Symbol* sym);

/**
 * @brief Links a symbol to the scope list
 * 
 * @param sym Symbol to link
 * @param bucket Bucket index of the current symbol
 */
void Alpha_LinkSymbolToScopeList(Alpha_Symbol* symbol, Uint8 bucket);

/**
 * @brief Print a list tied to a scope
 * 
 * @param scope Scope to print the list of
 */
void Alpha_PrintScopeList(Uint32 scope);

/**
 * @brief Print the bucket list 
 * 
 * @param bucket Bucket to print
 */
void Alpha_PrintBucket(Uint8 bucket);

/**
 * @brief Create the library functions
 */
void Alpha_CreateLibraryFunctions(void);

/**
 * @brief Finds the head of a scope list
 * 
 * @param scope Scope to search the head of
 * 
 * @returns The head node of the scope list
 */
Alpha_Symbol* Alpha_FindScopeListHead(Uint32 scope);

/**
 * @brief Special function for creating temporary-like
 * unnamed functions
 * 
 * @returns The created symbol
 */
Alpha_Symbol* Alpha_CreateUnNamedFunctionSymbol(void);

/**
 * @brief Look up if a symbol exists by name
 * 
 * @param symName Name of symbol to search for
 * 
 * @returns The symbol if found, lese nil
 */
Alpha_Symbol* Alpha_LookUp(char* symName);

/**
 * @brief Update the active field of each symbol
 * 
 * @param scope Scope to update the activity for
 */
void Alpha_Hide(Uint32 scope);

/* ________________________ CHECKS ________________________ */

/**
 * @brief Check if a symbol already exists, this can be used 
 * to check if a name is already in use by a global, when creating
 * new variables inside of a block/scope, variables with the same
 * name can exist in different scopes, only library function names
 * are explicitly illegal in use. This is the most important check
 * , since, when a function f() at scope 0, we CAN have a variable f
 * at another scope!
 * 
 * @param sym Symbol to check
 * 
 * @returns The found symbol, else a null pointer
 */
Alpha_Symbol* Alpha_CheckIfSymbolAlreadyExistsInScope(Alpha_Symbol* sym);

/**
 * @brief Check if a symbol with the same name already exists, this will be used before creating
 * a function. Functions no matter the scope/block cannot have the same name, they exist in the same 'space'.
 * Unlike the variables, a new scope won't allow us to create a new function with the same name of one, of another scope
 * 
 * @param sym Symbol to check for function name
 * 
 * @returns The found symbol, else a null pointer
 */
Alpha_Symbol* Alpha_CheckIfFunctionSymbolAlreadyExists(Alpha_Symbol* sym);

/**
 * @brief Check if a symbol is a global, meaning
 * scope 0 and is of type ALPHA_SYMTYPE_GLOBAL, we
 * can use this on dereferencing the global using the '::'
 * operator.
 * 
 * @param sym Symbol to check if it is already a global
 * 
 * @returns true if it already exists, false if not 
 */
bool Alpha_CheckIfSymbolIsGlobal(Alpha_Symbol* sym);

/**
 * @brief Check if a symbol created has the same name as
 * a library function, since their names are explicit, we 
 * cannot declare a new variable on a different scope with a
 * library function names
 * 
 * @param sym Symbol to check if it is already a library function
 * 
 * @returns true if it is a library function name, false if not
 */
bool Alpha_SymbolHasLibraryFunctionName(Alpha_Symbol* sym);

/**
 * @brief Check if a function with that name exists, this
 * will be used before a function call
 * 
 * @param name Name of function
 * 
 * @returns true if it exists, false if not
 */
bool Alpha_CheckIfFunctionNameExists(char* name);

/* ________________________ EXPRESSION FUNCTIONS ________________________ */

/**
 * @brief Creates a new tempotary symbol to be used
 * for temporary expressions
 * 
 * @returns The newly created temp symbol
 */
Alpha_Symbol* Alpha_CreateTempSymbol(void);

#endif
