%{
    #include "Alpha_Parser.h" /* Produced elsewhere */
    #include "../src/Alpha_Symbol.h"

    /* Yakk functions */
    int yylex(void);
    void yyerror(char *error);
    
    /* Global variables - initialize properly */
    extern Uint32 Alpha_CurrentScope;
    extern Uint32 Alpha_GlobalBlock;
%}

/* Eligible expression types */
%union{
    int integerValue;
    float floatValue;
    char* stringValue; /* Comment */
}

/* Token types */
%token<integerValue>    LEX_INTEGER
%token<floatValue>      LEX_FLOAT
%token<stringValue>     LEX_STRING
%token<stringValue>     LEX_ID

/* Return types */
%type<stringValue> lvalue
%type<stringValue> functionDeclaration
%type<stringValue> objectDefinition
%type<stringValue> argumentList
%type<exprValue> expression assignmentExpression orExpression andExpression equalityExpression
%type<exprValue> relationalExpression additiveExpression multiplicativeExpression unaryExpression
%type<exprValue> postfixExpression primaryExpression expressionList

/* Token definition, see produced Alpha_Parser.h for yytokentype */
%token LEX_SEMI_COLON LEX_LEFT_PARENTHESIS LEX_RIGHT_PARENTHESIS
%token LEX_LEFT_BRACKET LEX_RIGHT_BRACKET LEX_LEFT_CURLY_BRACE LEX_RIGHT_CURLY_BRACE
%token LEX_EQUAL LEX_INCREMENT LEX_DECREMENT LEX_DOT LEX_COMMA LEX_COLON
%token LEX_BREAK LEX_CONTINUE LEX_LOCAL LEX_DOUBLE_COLON LEX_DOUBLE_DOT
%token LEX_NOT LEX_IF LEX_WHILE LEX_FOR LEX_RETURN LEX_FUNCTION LEX_ASSIGNMENT LEX_ELSE
%token LEX_NIL LEX_TRUE LEX_FALSE LEX_UNKNOWN
%token LEX_OR LEX_AND LEX_NOT_EQ LEX_LESS LEX_LESS_EQ LEX_GREATER LEX_GREATER_EQ
%token LEX_ADDITION LEX_SUBTRACTION LEX_MULTIPLICATION LEX_DIVISION LEX_MODULO
%token LEX_LOWER_THAN_ELSE

/* Priorities - Right associative assignment, then left associative operators */
%right LEX_ASSIGNMENT
%left LEX_OR
%left LEX_AND
%left LEX_EQUAL LEX_NOT_EQ
%left LEX_LESS LEX_LESS_EQ LEX_GREATER LEX_GREATER_EQ
%left LEX_ADDITION LEX_SUBTRACTION
%left LEX_MULTIPLICATION LEX_DIVISION LEX_MODULO
%right LEX_NOT LEX_UMINUS LEX_INCREMENT LEX_DECREMENT /* Unary operators */
%left LEX_DOT LEX_LEFT_BRACKET LEX_LEFT_PARENTHESIS /* Member access and function calls */
%nonassoc LEX_LOWER_THAN_ELSE
%nonassoc LEX_ELSE

%%

program:
    statementList
    ;

statementList:
    statementList statement
    | statement
    ;

statement:
    expressionStatement
    | ifStatement
    | whileStatement
    | forStatement
    | returnStatement
    | LEX_BREAK LEX_SEMI_COLON{
        if(Alpha_InLoop == 0){
            AERR("'break' keyword at line %d, used outside of a loop\n", yylineno);
        }
    }
    | LEX_CONTINUE LEX_SEMI_COLON{
        if(Alpha_InLoop == 0){
            AERR("'continue' keyword at line %d, used outside of a loop\n", yylineno);
        }
    }
    | block
    | functionDeclaration
    ;

assignmentExpression:
    lvalue LEX_ASSIGNMENT expression{
        Alpha_Symbol* sym = Alpha_CreateSymbol($1, ALPHA_SYMTYPE_GLOBAL);

        /* Check if it corresponds to library function */
        bool isLibFunction = Alpha_SymbolHasLibraryFunctionName(sym);
        if(isLibFunction){
            AERR("Symbol %s at line %d, has name, equivalent to a library funtion\n", sym->symbolName, sym->line);
        }

        /* Avoid r-val assignment to function name sym */
        Alpha_Symbol* isFunc = ALPHA_NIL;

        if(Alpha_MemberAccess == false){ /* Not accessing a member field of a function call */
            isFunc = Alpha_CheckIfFunctionSymbolAlreadyExists(sym);

            if(isFunc != ALPHA_NIL){
                AERR("Symbol %s is of function type, and cannot be assigned a value\n", isFunc->symbolName);
            }
        }

        /* Check if it already exists in the same scope */
        Alpha_Symbol* foundSym = Alpha_CheckIfSymbolAlreadyExistsInScope(sym);
        if(foundSym != ALPHA_NIL && isFunc == ALPHA_NIL){
            /* If it is found and not a function, it is legal use since we assign a new value to it */
        }else if(isFunc == ALPHA_NIL){ /* If not a function, create a new one */
            if(Alpha_CurrentScope != 0){
                sym->type = ALPHA_SYMTYPE_LOCAL; /* It doesn't exist, create a new local, no need for the keyword */
            }else{
                sym->type = ALPHA_SYMTYPE_GLOBAL; /* Except at scope 0, where we have a global */
            }
            Alpha_InsertSymbol(sym);
        }

        $$ = ALPHA_NIL;
    }
    | LEX_LOCAL LEX_ID LEX_ASSIGNMENT expression{
        Alpha_Symbol* sym = Alpha_CreateSymbol($2, ALPHA_SYMTYPE_LOCAL);

        /* Check if it corresponds to library function */
        bool isLibFunction = Alpha_SymbolHasLibraryFunctionName(sym);
        if(isLibFunction){
            AERR("Symbol %s at line %d, has name, equivalent to a library funtion\n", sym->symbolName, sym->line);
        }

        /* Check if it already exists in the same scope */
        Alpha_Symbol* foundSym = Alpha_CheckIfSymbolAlreadyExistsInScope(sym);
        if(foundSym != ALPHA_NIL){
            AERR("Symbol %s already exists at line %d\n", foundSym->symbolName, foundSym->line);
        }

        /* If it isn't a library function, and has not been found in the same scope, 
        then insert to the table */
        if(isLibFunction == false && foundSym == ALPHA_NIL){
            Alpha_InsertSymbol(sym);
        }

        $$ = ALPHA_NIL;
    }
    | LEX_DOUBLE_COLON LEX_ID LEX_ASSIGNMENT expression{
        Alpha_Symbol* sym = Alpha_CreateSymbol($2, ALPHA_SYMTYPE_GLOBAL);

        /* Check if it exists */
        Alpha_Symbol* foundSym = Alpha_LookUp($2);
        if(foundSym == ALPHA_NIL){
            AERR("Symbol %s at line %d, cannot be dereferenced as a global, since it has not been declared yet\n", sym->symbolName, sym->line);
        }

        /* Check if it is a global */
        if(foundSym){
            if(Alpha_CheckIfSymbolIsGlobal(foundSym) == false){
                AERR("Symbol %s at line %d, cannot be dereferenced as a global, since it isn't declared as such\n", sym->symbolName, sym->line);
            }
        }

        $$ = ALPHA_NIL;
    }
    ;

expressionStatement:
    expression LEX_SEMI_COLON
    ;

/* Clean expression hierarchy */
expression:
    assignmentExpression
    | orExpression
    ;

orExpression:
    orExpression LEX_OR andExpression
    | andExpression
    ;

andExpression:
    andExpression LEX_AND equalityExpression
    | equalityExpression
    ;

equalityExpression:
    equalityExpression LEX_EQUAL relationalExpression
    | equalityExpression LEX_NOT_EQ relationalExpression
    | relationalExpression
    ;

relationalExpression:
    relationalExpression LEX_LESS additiveExpression
    | relationalExpression LEX_LESS_EQ additiveExpression
    | relationalExpression LEX_GREATER additiveExpression
    | relationalExpression LEX_GREATER_EQ additiveExpression
    | additiveExpression
    ;

additiveExpression:
    additiveExpression LEX_ADDITION multiplicativeExpression{
        Alpha_Expression* result = Alpha_ExpressionWithTemp(Alpha_Expression_Arithmetic);
        Alpha_CreateQuad(Alpha_InstructionOpcode_Add, $1, $3, result);
        $$ = result;
    }
    | additiveExpression LEX_SUBTRACTION multiplicativeExpression{
        Alpha_Expression* result = Alpha_ExpressionWithTemp(Alpha_Expression_Arithmetic);
        Alpha_CreateQuad(Alpha_InstructionOpcode_Sub, $1, $3, result);
        $$ = result;
    }
    | multiplicativeExpression{
        $$ = $1;
    }
    ;

multiplicativeExpression:
    multiplicativeExpression LEX_MULTIPLICATION unaryExpression
    | multiplicativeExpression LEX_DIVISION unaryExpression
    | multiplicativeExpression LEX_MODULO unaryExpression
    | unaryExpression
    ;

unaryExpression:
    LEX_NOT unaryExpression{
        /* ΑΓΧ */
        /* Η Μυρτώ είπε ψέματα τι δεν τη γαμοάει ο Τιτουλίνης ΑΓΧ ΑΓΧ ΑΓΧ ΑΓΧ */
    }
    | LEX_SUBTRACTION unaryExpression %prec LEX_UMINUS{
        /* No idea */
    }
    | LEX_INCREMENT postfixExpression{
        Alpha_Symbol* sym = Alpha_CreateSymbol($2, ALPHA_SYMTYPE_GLOBAL);

        /* Check if it corresponds to library function */
        bool isLibFunction = Alpha_SymbolHasLibraryFunctionName(sym);
        if(isLibFunction){
            AERR("Symbol %s at line %d, has name, equivalent to a library function, and cannot be incremented\n", sym->symbolName, sym->line);
        }

        /* Avoid r-val assignment to function name sym */
        Alpha_Symbol* isFunc = ALPHA_NIL;

        if(Alpha_MemberAccess == false){ /* Not accessing a member field of a function call */
            isFunc = Alpha_CheckIfFunctionSymbolAlreadyExists(sym);

            if(isFunc != ALPHA_NIL){
                AERR("Symbol %s is of function type, and cannot be incremented\n", isFunc->symbolName);
            }
        }

        /* Check if it already exists in the same scope */
        Alpha_Symbol* foundSym = Alpha_CheckIfSymbolAlreadyExistsInScope(sym);
        if(foundSym == ALPHA_NIL){
            AERR("Symbol %s at line %d not found in the current scope to increment\n", sym->symbolName, yylineno);
        }
    }
    | LEX_DECREMENT postfixExpression{
        Alpha_Symbol* sym = Alpha_CreateSymbol($2, ALPHA_SYMTYPE_GLOBAL);

        /* Check if it corresponds to library function */
        bool isLibFunction = Alpha_SymbolHasLibraryFunctionName(sym);
        if(isLibFunction){
            AERR("Symbol %s at line %d, has name, equivalent to a library function, and cannot be incremented\n", sym->symbolName, sym->line);
        }

        /* Avoid r-val assignment to function name sym */
        Alpha_Symbol* isFunc = ALPHA_NIL;

        if(Alpha_MemberAccess == false){ /* Not accessing a member field of a function call */
            isFunc = Alpha_CheckIfFunctionSymbolAlreadyExists(sym);

            if(isFunc != ALPHA_NIL){
                AERR("Symbol %s is of function type, and cannot be incremented\n", isFunc->symbolName);
            }
        }

        /* Check if it already exists in the same scope */
        Alpha_Symbol* foundSym = Alpha_CheckIfSymbolAlreadyExistsInScope(sym);
        if(foundSym == ALPHA_NIL){
            AERR("Symbol %s at line %d not found in the current scope to increment\n", sym->symbolName, yylineno);
        }
    }
    | postfixExpression
    ;

postfixExpression:
    postfixExpression LEX_INCREMENT{
        Alpha_Symbol* sym = Alpha_CreateSymbol($1, ALPHA_SYMTYPE_GLOBAL);

        /* Check if it corresponds to library function */
        bool isLibFunction = Alpha_SymbolHasLibraryFunctionName(sym);
        if(isLibFunction){
            AERR("Symbol %s at line %d, has name, equivalent to a library function, and cannot be incremented\n", sym->symbolName, sym->line);
        }

        /* Avoid r-val assignment to function name sym */
        Alpha_Symbol* isFunc = ALPHA_NIL;

        if(Alpha_MemberAccess == false){ /* Not accessing a member field of a function call */
            isFunc = Alpha_CheckIfFunctionSymbolAlreadyExists(sym);

            if(isFunc != ALPHA_NIL){
                AERR("Symbol %s is of function type, and cannot be incremented\n", isFunc->symbolName);
            }
        }

        /* Check if it already exists in the same scope */
        Alpha_Symbol* foundSym = Alpha_CheckIfSymbolAlreadyExistsInScope(sym);
        if(foundSym == ALPHA_NIL){
            AERR("Symbol %s at line %d not found in the current scope to increment\n", sym->symbolName, yylineno);
        }
    }
    | postfixExpression LEX_DECREMENT{
        Alpha_Symbol* sym = Alpha_CreateSymbol($1, ALPHA_SYMTYPE_GLOBAL);

        /* Check if it corresponds to library function */
        bool isLibFunction = Alpha_SymbolHasLibraryFunctionName(sym);
        if(isLibFunction){
            AERR("Symbol %s at line %d, has name, equivalent to a library function, and cannot be incremented\n", sym->symbolName, sym->line);
        }

        /* Avoid r-val assignment to function name sym */
        Alpha_Symbol* isFunc = ALPHA_NIL;

        if(Alpha_MemberAccess == false){ /* Not accessing a member field of a function call */
            isFunc = Alpha_CheckIfFunctionSymbolAlreadyExists(sym);

            if(isFunc != ALPHA_NIL){
                AERR("Symbol %s is of function type, and cannot be incremented\n", isFunc->symbolName);
            }
        }

        /* Check if it already exists in the same scope */
        Alpha_Symbol* foundSym = Alpha_CheckIfSymbolAlreadyExistsInScope(sym);
        if(foundSym == ALPHA_NIL){
            AERR("Symbol %s at line %d not found in the current scope to increment\n", sym->symbolName, yylineno);
        }
    }
    | postfixExpression LEX_DOT LEX_ID
    | postfixExpression LEX_LEFT_BRACKET expression LEX_RIGHT_BRACKET
    | postfixExpression LEX_LEFT_PARENTHESIS argumentList LEX_RIGHT_PARENTHESIS{
        if(Alpha_CheckIfFunctionNameExists($1) == false){
            AERR("Cannot call function %s at line %d, as it has not been previously declared\n", $1, yylineno);
        }
    }
    | postfixExpression LEX_DOUBLE_DOT LEX_ID LEX_LEFT_PARENTHESIS argumentList LEX_RIGHT_PARENTHESIS
    | primaryExpression
    ;

primaryExpression:
    LEX_ID{
        $$ = $1;
    }
    | LEX_LOCAL LEX_ID{
        $$ = $2;
    }
    | LEX_DOUBLE_COLON LEX_ID{
        $$ = $2;
    }
    | constant{
        $$ = ALPHA_NIL;
    }
    | LEX_LEFT_PARENTHESIS expression LEX_RIGHT_PARENTHESIS{
        $$ = ALPHA_NIL;
    }
    | objectDefinition{
        $$ = ALPHA_NIL;
    }
    | LEX_LEFT_PARENTHESIS functionDeclaration LEX_RIGHT_PARENTHESIS{
        $$ = ALPHA_NIL;
    }
    ;

lvalue:
    LEX_ID{
        $$ = $1;

        Alpha_MemberAccess = false;
    }
    | postfixExpression LEX_DOT LEX_ID{
        Alpha_MemberAccess = true; /* No need to check if the postfix expression is a user function call
        as we are can have assignments on member access */

    }
    | postfixExpression LEX_LEFT_BRACKET expression LEX_RIGHT_BRACKET{
        /* Unsure, indexed list member access */
    }
    ;

argumentList:
    expressionList
    | {
        /* Empty */
        $$ = ALPHA_NIL;
    }
    ;

expressionList:
    expression
    | expressionList LEX_COMMA expression
    ;

objectDefinition:
    LEX_LEFT_BRACKET objectComponents LEX_RIGHT_BRACKET{
        $$ = ALPHA_NIL;
    }
    ;

objectComponents:
    expressionList
    | indexedElementList
    | /* Empty */
    ;

indexedElementList:
    indexedElement
    | indexedElementList LEX_COMMA indexedElement
    ; 

indexedElement:
    LEX_LEFT_CURLY_BRACE expression LEX_COLON expression LEX_RIGHT_CURLY_BRACE
    ;

block:
    LEX_LEFT_CURLY_BRACE{
        Alpha_CurrentScope++;
        Alpha_GlobalBlock++;
    }statementList LEX_RIGHT_CURLY_BRACE{
        Alpha_Hide(Alpha_CurrentScope);
        Alpha_CurrentScope--;
    }
    | LEX_LEFT_CURLY_BRACE{
        Alpha_CurrentScope++;
        Alpha_GlobalBlock++;
    }LEX_RIGHT_CURLY_BRACE{
        Alpha_Hide(Alpha_CurrentScope);
        Alpha_CurrentScope--;
    }
    ;

functionDeclaration:
    LEX_FUNCTION LEX_ID LEX_LEFT_PARENTHESIS parameterList LEX_RIGHT_PARENTHESIS{
        Alpha_InFunc++;

        Alpha_FunctionLineProxy = yylineno;

        Alpha_Symbol* funcSym = Alpha_CreateSymbol($2, ALPHA_SYMTYPE_FUNCTION);

        Alpha_Symbol* alreadyExists = Alpha_CheckIfFunctionSymbolAlreadyExists(funcSym);

        if(alreadyExists != ALPHA_NIL){
            AERR("Function symbol %s already exists at line %d\n", funcSym->symbolName, alreadyExists->line);
        }

        Alpha_Symbol* inScope = Alpha_CheckIfSymbolAlreadyExistsInScope(funcSym);
        
        if(inScope != ALPHA_NIL){
            AERR("Symbol %s already exists at line %d\n", funcSym->symbolName, inScope->line);
        }

        if(alreadyExists == ALPHA_NIL && inScope == ALPHA_NIL){
            funcSym->line = Alpha_FunctionLineProxy;
            Alpha_InsertSymbol(funcSym);
        }
    }block{
        Alpha_InFunc--;
    }
    | LEX_FUNCTION LEX_LEFT_PARENTHESIS parameterList LEX_RIGHT_PARENTHESIS{
        Alpha_InFunc++;

        Alpha_FunctionLineProxy = yylineno;
        
        /* Unnamed function symbol special creation function, the counter is updated inside the function */
        Alpha_Symbol* funcSym = Alpha_CreateUnNamedFunctionSymbol();

        Alpha_Symbol* alreadyExists = Alpha_CheckIfFunctionSymbolAlreadyExists(funcSym);

        if(alreadyExists != ALPHA_NIL){
            AERR("Function symbol %s already exists at line %d\n", funcSym->symbolName, alreadyExists->line);
        }

        Alpha_Symbol* inScope = Alpha_CheckIfSymbolAlreadyExistsInScope(funcSym);
        
        if(inScope != ALPHA_NIL){
            AERR("Symbol %s already exists at line %d\n", funcSym->symbolName, inScope->line);
        }

        if(alreadyExists == ALPHA_NIL && inScope == ALPHA_NIL){
            funcSym->line = Alpha_FunctionLineProxy;
            Alpha_InsertSymbol(funcSym);
        }
    }block{
        Alpha_InFunc--;
    }
    ;

constant:
    LEX_INTEGER
    | LEX_FLOAT
    | LEX_STRING
    | LEX_NIL
    | LEX_TRUE
    | LEX_FALSE
    ;

parameterList:
    LEX_ID{
        /* Peaky: Do I really need to check anything here? */
        Alpha_Symbol* paramSym = Alpha_CreateSymbol($1, ALPHA_SYMTYPE_PARAM);
        paramSym->scope++;
        paramSym->block++;

        Alpha_Symbol* isFound = Alpha_CheckIfSymbolAlreadyExistsInScope(paramSym);
        if(isFound){
            AERR("Parameter %s already exists\n", isFound->symbolName);
        }else{
            Alpha_InsertSymbol(paramSym);
        }
    }
    | parameterList LEX_COMMA LEX_ID{
        Alpha_Symbol* paramSym = Alpha_CreateSymbol($3, ALPHA_SYMTYPE_PARAM);
        paramSym->scope++;
        paramSym->block++;

        Alpha_Symbol* isFound = Alpha_CheckIfSymbolAlreadyExistsInScope(paramSym);
        if(isFound){
            AERR("Parameter %s already exists\n", isFound->symbolName);
        }else{
            Alpha_InsertSymbol(paramSym);
        }
    }
    | /* Empty */
    ;

ifStatement:
    LEX_IF LEX_LEFT_PARENTHESIS expression LEX_RIGHT_PARENTHESIS statement %prec LEX_LOWER_THAN_ELSE
    | LEX_IF LEX_LEFT_PARENTHESIS expression LEX_RIGHT_PARENTHESIS statement LEX_ELSE statement
    ;

whileStatement:
    LEX_WHILE LEX_LEFT_PARENTHESIS expression LEX_RIGHT_PARENTHESIS{
        Alpha_InLoop++;
    }statement{
        Alpha_InLoop--;
    }
    ;

forStatement: /* IDEA: This might need some more stuff */
    LEX_FOR LEX_LEFT_PARENTHESIS argumentList LEX_SEMI_COLON expression LEX_SEMI_COLON argumentList LEX_RIGHT_PARENTHESIS{ /* The assign statement already has the semicolon */
        Alpha_InLoop++;
    }statement{
        Alpha_InLoop--;
    }
    ;

returnStatement: /* FIXME: 2 conflicts here */
    LEX_RETURN LEX_SEMI_COLON{
        if(Alpha_InFunc == 0){ /* See if we are inside a function */
            AERR("'return' keyword at line %d called outside of a function call\n", yylineno);
        }

        /* Wow, literally does nothing */
    }
    | LEX_RETURN LEX_ID LEX_SEMI_COLON{
        if(Alpha_InFunc == 0){
            AERR("'return' keyword at line %d called outside of a function call\n", yylineno);
        }

        /* We need to check if the identifier can be returned */
        Alpha_Symbol* retSym = Alpha_CreateSymbol($2, ALPHA_SYMTYPE_LOCAL);
        
        Alpha_Symbol* symFound = Alpha_CheckIfSymbolAlreadyExistsInScope(retSym);
        if(symFound == ALPHA_NIL){
            AERR("Symbol %s, line %d, cannot be returned in this scope, as it was not declared here\n", $2, yylineno);
        }
    }
    | LEX_RETURN LEX_DOUBLE_COLON LEX_ID LEX_SEMI_COLON{
        if(Alpha_InFunc == 0){
            AERR("'return' keyword at line %d called outside of a function call\n", yylineno);
        }

        /* Check if it is a global */
        // Alpha_Symbol* retSym = Alpha_CreateSymbol($3, ALPHA_SYMTYPE_LOCAL);

        /* Check if the symbol already exists */
        Alpha_Symbol* foundSym = Alpha_LookUp($3);

        bool isGlobal;

        if(foundSym != ALPHA_NIL){
            isGlobal = Alpha_CheckIfSymbolIsGlobal(foundSym);
        }else{
            AERR("Symbol %s, line %d, cannot be returned in this scope, as it was not declared here\n", $3, yylineno);
        }

        if(isGlobal == false){
            AERR("Symbol %s, line %d, cannot be returned in this scope, as it is not a global\n", $3, yylineno);
        }
    } 
        | LEX_RETURN expression LEX_SEMI_COLON{
            if(Alpha_InFunc == 0){
                AERR("'return' keyword at line %d called outside of a function call\n", yylineno);
            }
        }
    ;

%%

void yyerror(char* errorMsg){
    printf("Parsing failed: %s when reading token %s at line %d\n", errorMsg, yytext, yylineno);
    return;
}