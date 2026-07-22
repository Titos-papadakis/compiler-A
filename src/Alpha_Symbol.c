#include "Alpha_Symbol.h"

Uint32 Alpha_CurrentScope = 0;
Uint32 Alpha_GlobalBlock = 0;
Uint32 Alpha_UnnamedFunctionCounter = 0;
Uint32 Alpha_FunctionLineProxy;
Uint32 Alpha_InLoop = 0;
Uint32 Alpha_InFunc = 0;
bool Alpha_MemberAccess = false;
Uint32 Alpha_TemporaryCounter;

Alpha_Symbol *Alpha_Symbols[ALPHA_SYMBOL_TABLE_BUCKETS];

char* Alpha_TypeNames[5] = {
    "LOCAL",
    "GLOBAL",
    "PARAMETER",
    "FUNCTION",
    "TABLE"
};

Uint8 Alpha_HashFunction(char* symbolName){
    Uint32 ASCIIcount = 0;

    for(int i = 0 ; symbolName[i] != '\0' ; i++){
        ASCIIcount += symbolName[i];
    }

    /* Limit the index to the amount of buckets we have */
    return ASCIIcount % ALPHA_SYMBOL_TABLE_BUCKETS;
}

Sint8 Alpha_AllocateSymbolTableBuckets(void){
    for(int i = 0; i < ALPHA_SYMBOL_TABLE_BUCKETS; i++){
        Alpha_Symbols[i] = ALPHA_NIL; /* Init to null */
    }

    return ALPHA_SUCCESS;
}

Alpha_Symbol* Alpha_CreateSymbol(char *name, Alpha_SymbolType type){
    if(name == ALPHA_NIL){
        AERR("Name of symbol is null\n");
        return ALPHA_NIL;
    }

    Alpha_Symbol* newSymbol = (Alpha_Symbol*)malloc(sizeof(Alpha_Symbol));
    if(newSymbol == ALPHA_NIL){
        AERR("Unable to create symbol %s\n", name);
        return ALPHA_NIL;
    }

    newSymbol->scope = Alpha_CurrentScope;

    if(type == ALPHA_SYMTYPE_GLOBAL){
        newSymbol->scope = 0; /* Alll globals exist in the same scope */
    }

    newSymbol->block = Alpha_GlobalBlock;
    newSymbol->line = yylineno;
    newSymbol->symbolName = strdup(name); /* This can be problematic, check parser return types */
    newSymbol->type = type;

    return newSymbol;
}

void Alpha_InsertSymbol(Alpha_Symbol* sym){
    if(!sym){
        AERR("Null symbol :<\n");
        return;
    }

    Uint8 bucket = h(sym->symbolName);

    if(Alpha_Symbols[bucket] == ALPHA_NIL){
        Alpha_Symbols[bucket] = sym; /* Head of chain is empty, first insertion */
        sym->nextInBucket = ALPHA_NIL;
    }else{
        Alpha_Symbol *current = Alpha_Symbols[bucket];

        while(current->nextInBucket != ALPHA_NIL){
            current = current->nextInBucket;
        }

        current->nextInBucket = sym;
        sym->nextInBucket = ALPHA_NIL;
    }

    /* Peaky: After the main chain linking, we need to link the scope lists (ascending from the bottom indexed bucket)
    we link the nodes with the same scope inside the chain first, then we move to the next chain */
    Alpha_LinkSymbolToScopeList(sym, bucket);

    return;
}

void Alpha_LinkSymbolToScopeList(Alpha_Symbol* symbol, Uint8 bucket){
    Alpha_Symbol *searchList = ALPHA_NIL;
    Alpha_Symbol *prevScopeNode = ALPHA_NIL;

    Uint32 i = 0;
    while(i <= bucket){
        searchList = Alpha_Symbols[i];
        while(searchList != ALPHA_NIL){
            if(searchList != symbol && searchList->scope == symbol->scope){
                prevScopeNode = searchList;
            }

            searchList = searchList->nextInBucket;
        }

        i++;
    }

    if(prevScopeNode != ALPHA_NIL){ /* Not the first element in the scope */
        if(prevScopeNode->nextInScope == ALPHA_NIL){ /* last element in the scope list */
            prevScopeNode->nextInScope = symbol;
            symbol->nextInScope = ALPHA_NIL;
        }else{
            symbol->nextInScope = prevScopeNode->nextInScope;
            prevScopeNode->nextInScope = symbol;
        }
    }else{
        Alpha_Symbol *traverse = ALPHA_NIL;
        Uint8 start = 0;
        for(Uint8 i = 0 ; i < ALPHA_SYMBOL_TABLE_BUCKETS ; i++){
            traverse = Alpha_Symbols[i];
            while(traverse != ALPHA_NIL){
                if(traverse != symbol && traverse->scope == symbol->scope){ /* Checks all the table to find a node with the same scope with the newnode. It goes and after the bucket of the newnode */
                    start = 1;
                    break;
                }

                traverse = traverse->nextInBucket;
            }if(start == 1){
                break;
            }
        }

        symbol->nextInScope = traverse;
    }

    return;
}

void Alpha_PrintScopeList(Uint32 scope){
    Alpha_Symbol* scopeHead = Alpha_FindScopeListHead(scope);

    if(scopeHead == ALPHA_NIL){ return; }

    static bool print1 = false;

    if(print1 == false){
        ADEB("%-20s %-6s %-6s %-6s %-10s\n", "Name", "Line", "Block", "Scope", "Type");
        print1 = true;
    }

    while(scopeHead != ALPHA_NIL){
        ADEB("%-20s %-6d %-6d %-6d %-10s\n",
        scopeHead->symbolName,
        scopeHead->line,
        scopeHead->block,
        scopeHead->scope,
        Alpha_TypeNames[scopeHead->type]);

        scopeHead = scopeHead->nextInScope;
    }

    return;
}

void Alpha_PrintBucket(Uint8 bucket){
    Alpha_Symbol* bucketHead = Alpha_Symbols[bucket];

    if(bucketHead == ALPHA_NIL){ return; }

    static bool print2 = false;

    if(print2 == false){
        ADEB("%-20s %-6s %-6s %-6s %-10s\n", "Name", "Line", "Block", "Scope", "Type");
        print2 = true;
    }


    while(bucketHead != ALPHA_NIL){
        ADEB("%-20s %-6d %-6d %-6d %-10s\n", 
        bucketHead->symbolName, 
        bucketHead->line, 
        bucketHead->block, 
        bucketHead->scope, 
        Alpha_TypeNames[bucketHead->type]);

        bucketHead = bucketHead->nextInBucket;
    }

    return;
}

void Alpha_CreateLibraryFunctions(void){
    Alpha_InsertSymbol(Alpha_CreateSymbol("print", ALPHA_SYMTYPE_FUNCTION));
    Alpha_InsertSymbol(Alpha_CreateSymbol("input", ALPHA_SYMTYPE_FUNCTION));
    Alpha_InsertSymbol(Alpha_CreateSymbol("objectmemberkeys", ALPHA_SYMTYPE_FUNCTION));
    Alpha_InsertSymbol(Alpha_CreateSymbol("objecttotalmembers", ALPHA_SYMTYPE_FUNCTION));
    Alpha_InsertSymbol(Alpha_CreateSymbol("objectcopy", ALPHA_SYMTYPE_FUNCTION));
    Alpha_InsertSymbol(Alpha_CreateSymbol("totalarguments", ALPHA_SYMTYPE_FUNCTION));
    Alpha_InsertSymbol(Alpha_CreateSymbol("typeof", ALPHA_SYMTYPE_FUNCTION));
    Alpha_InsertSymbol(Alpha_CreateSymbol("strtonum", ALPHA_SYMTYPE_FUNCTION));
    Alpha_InsertSymbol(Alpha_CreateSymbol("sqrt", ALPHA_SYMTYPE_FUNCTION));
    Alpha_InsertSymbol(Alpha_CreateSymbol("cos", ALPHA_SYMTYPE_FUNCTION));
    Alpha_InsertSymbol(Alpha_CreateSymbol("sin", ALPHA_SYMTYPE_FUNCTION));

    return;
}

Alpha_Symbol* Alpha_FindScopeListHead(Uint32 scope){
    Alpha_Symbol* scopeListHead = ALPHA_NIL;
    bool headFound = false;

    /* Try and find the scope head */
    for(Uint8 i = 0 ; i < ALPHA_SYMBOL_TABLE_BUCKETS ; i++){
        Alpha_Symbol* current = Alpha_Symbols[i];
        
        while(current != ALPHA_NIL){
            if(current->scope == scope){
                scopeListHead = current;
                headFound = true;

                break;
            }
            current = current->nextInScope;
        }

        if(headFound){
            break;
        }
    }

    return scopeListHead;
}

Alpha_Symbol* Alpha_CreateUnNamedFunctionSymbol(void){
    char stringBuf[64];
    sprintf(stringBuf, "%s%d", "_f", Alpha_UnnamedFunctionCounter); /* Concat the two strings */

    Alpha_Symbol* sym = Alpha_CreateSymbol(stringBuf, ALPHA_SYMTYPE_FUNCTION);

    Alpha_UnnamedFunctionCounter++;

    return sym;
}

Alpha_Symbol* Alpha_LookUp(char* symName){
    Uint32 bucket = h(symName);

    Alpha_Symbol* current = Alpha_Symbols[bucket];

    while(current != ALPHA_NIL){
        if(strcmp(current->symbolName, symName) == 0){
            return current; /* Symbol found */
        }
        
        current = current->nextInBucket;
    }

    return ALPHA_NIL;
}

void Alpha_Hide(Uint32 scope){
    Alpha_Symbol* current = Alpha_FindScopeListHead(scope);

    while(current != ALPHA_NIL){
        if(current){
            current->active = false;
        }

        current = current->nextInScope;
    }    

    return;
}

/* ________________________ CHECKS ________________________ */

Alpha_Symbol* Alpha_CheckIfSymbolAlreadyExistsInScope(Alpha_Symbol* sym){
    Uint8 bucket = h(sym->symbolName);
    Alpha_Symbol* current = Alpha_Symbols[bucket];

    while(current != ALPHA_NIL){
        if(strcmp(current->symbolName, sym->symbolName) == 0 && current->scope == sym->scope && current->block == sym->block){
            return current;
        }
        current = current->nextInBucket;
    }

    return ALPHA_NIL;
}

Alpha_Symbol* Alpha_CheckIfFunctionSymbolAlreadyExists(Alpha_Symbol* sym){
    Uint8 bucket = h(sym->symbolName);
    Alpha_Symbol* current = Alpha_Symbols[bucket];

    while(current != ALPHA_NIL){
        /* We simply cannot have functions with the same name, anywhere */
        if(strcmp(current->symbolName, sym->symbolName) == 0 && current->type == ALPHA_SYMTYPE_FUNCTION){
            return current;
        }
        current = current->nextInBucket;
    }

    return ALPHA_NIL;
}

bool Alpha_CheckIfSymbolIsGlobal(Alpha_Symbol* sym){
    Alpha_Symbol* scopeHead = Alpha_FindScopeListHead(0);

    while(scopeHead != ALPHA_NIL){
        if(strcmp(sym->symbolName, sym->symbolName) == 0 && scopeHead->type == ALPHA_SYMTYPE_GLOBAL){
            return true;
        }

        scopeHead = scopeHead->nextInScope;
    }

    return false;
}

bool Alpha_SymbolHasLibraryFunctionName(Alpha_Symbol* sym){
    if(strcmp("print", sym->symbolName) == 0 ) return true;
    if(strcmp("input", sym->symbolName) == 0 ) return true;
    if(strcmp("objectmemberkeys", sym->symbolName) == 0 ) return true;
    if(strcmp("objecttotalmembers", sym->symbolName) == 0 ) return true;
    if(strcmp("objectcopy", sym->symbolName) == 0 ) return true;
    if(strcmp("totalarguments", sym->symbolName) == 0 ) return true;
    if(strcmp("typeof", sym->symbolName) == 0 ) return true;
    if(strcmp("strtonum", sym->symbolName) == 0 ) return true;
    if(strcmp("sqrt", sym->symbolName) == 0 ) return true;
    if(strcmp("cos", sym->symbolName) == 0 ) return true;
    if(strcmp("sin", sym->symbolName) == 0 ) return true;

    return false;
}

bool Alpha_CheckIfFunctionNameExists(char* name){
    Uint8 bucket = h(name);
    Alpha_Symbol* current = Alpha_Symbols[bucket];

    while(current != ALPHA_NIL){
        /* We simply cannot have functions with the same name, anywhere */
        if(strcmp(current->symbolName, name) == 0 && current->type == ALPHA_SYMTYPE_FUNCTION){
            return true;
        }
        current = current->nextInBucket;
    }

    return false;
}

Alpha_Symbol* Alpha_CreateTempSymbol(void){
    char name[64]; /* Buffer to edit the temporary name */
    sprintf(name, "_t%d", Alpha_TemporaryCounter++);
    Alpha_Symbol* tau = Alpha_CreateSymbol(name, ALPHA_SYMTYPE_TEMP); /* Create the temporary symbol */
    Alpha_InsertSymbol(tau);

    return tau;
}
