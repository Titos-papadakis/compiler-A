#include "Alpha_Expression.h"

FILE* out;

Alpha_Expression *Alpha_CreateExpression(Alpha_ExpressionType type){
    Alpha_Expression *expr = malloc(sizeof(Alpha_Expression));

    expr->type = type;
    expr->idx = ALPHA_NIL;
    expr->next = ALPHA_NIL;
    expr->numConst = 0.0;
    expr->boolConst = ALPHA_TRUE;
    expr->strConst = ALPHA_NIL;
    expr->sym = ALPHA_NIL;
    expr->Alpha_Truelist = ALPHA_NIL;  
    expr->Alpha_Falselist = ALPHA_NIL; 
    expr->nextlist = ALPHA_NIL;

    return expr;
}

void Alpha_PrintExpression(Alpha_Expression* expr){
    if (!expr) {
        fprintf(out, "%-10s", "");
        return;
    }
    switch (expr->type) {
        case Alpha_Expression_Variable:
        case Alpha_Expression_Program_function:
        case Alpha_Expression_Library_function:
        case Alpha_Expression_Table_item:
        case Alpha_Expression_New_table:
        if (expr->sym && expr->sym->symbolName)
            fprintf(out, "%-10s", expr->sym->symbolName);
        else
            fprintf(out, "%-10s", "");
            break;

        case Alpha_Expression_Const_number:
            fprintf(out, "%-10.0f", expr->numConst);
            break;

        case Alpha_Expression_Const_string:
            fprintf(out, "%-10s", expr->strConst ? expr->strConst : "");
            break;

        case Alpha_Expression_Const_bool:
            fprintf(out, "%-10s", expr->boolConst ? "'true'" : "'false'");
            break;

        case Alpha_Expression_Nil:
            fprintf(out, "%-10s", "nil");
            break;

        default:
            if (expr->sym && expr->sym->symbolName)
                fprintf(out, "%-10s", expr->sym->symbolName);
            else
                fprintf(out, "%-10s", "");
            break;
    }

    return;
}

Alpha_Expression *Alpha_CreateTemporaryExpression(Alpha_ExpressionType type){
    Alpha_Symbol* sym = Alpha_CreateTempSymbol();
    if (!sym) AERR("Temp symbol is NULL\n");
    
    Alpha_Expression *expr = malloc(sizeof(Alpha_Expression));
    
    expr->type = type;
    expr->idx = ALPHA_NIL;
    expr->next = ALPHA_NIL;
    expr->numConst = 0.0;
    expr->boolConst = ALPHA_TRUE;
    expr->strConst = strdup(sym->symbolName);
    expr->sym = sym; 
    expr->Alpha_Truelist = ALPHA_NIL;  
    expr->Alpha_Falselist = ALPHA_NIL; 
    expr->nextlist = ALPHA_NIL;  

    return expr;
}

Alpha_Expression *Alpha_CreateRetTempExpression(Alpha_ExpressionType type){
    Alpha_Symbol* sym = Alpha_NewRet();
    Alpha_Expression *expr = malloc(sizeof(Alpha_Expression));
    if (!sym) AERR("Temp symbol is NULL\n");

    expr->type = type;
    expr->idx = ALPHA_NIL;
    expr->next = ALPHA_NIL;
    expr->numConst = 0.0;
    expr->boolConst = ALPHA_TRUE;
    expr->strConst = strdup(sym->symbolName);
    expr->sym = sym;   

    return expr;
}

Alpha_Expression *Alpha_CreateConstNumExpression(double num){
    Alpha_Expression *expr = malloc(sizeof(Alpha_Expression));

    expr->type = Alpha_Expression_Const_number;
    expr->idx = ALPHA_NIL;
    expr->next = ALPHA_NIL;
    expr->numConst = num;
    expr->boolConst = ALPHA_TRUE;
    expr->strConst = ALPHA_NIL;
    expr->sym = ALPHA_NIL;   

    return expr;
}

Alpha_Expression *Alpha_CreateConstStringExpression(char* str){
    Alpha_Expression *expr = malloc(sizeof(Alpha_Expression));

    expr->type = Alpha_Expression_Const_string;
    expr->idx = ALPHA_NIL;
    expr->next = ALPHA_NIL;
    expr->numConst = 0.0;
    expr->boolConst = ALPHA_TRUE;
    expr->strConst = strdup(str); /* no words..., strdup(...) was missing */
    expr->sym = ALPHA_NIL;   

    return expr;
}

Alpha_Expression *Alpha_CreateConstBoolExpression(bool b){
    Alpha_Expression *expr = malloc(sizeof(Alpha_Expression));

    expr->type = Alpha_Expression_Const_bool;
    expr->idx = ALPHA_NIL;
    expr->next = ALPHA_NIL;
    expr->numConst = 0.0;
    expr->boolConst = b;
    expr->strConst = ALPHA_NIL;
    expr->sym = ALPHA_NIL;   

    return expr;
}

Alpha_Expression* Alpha_CreateTableExpression(char* symbolName){
    Alpha_Expression* expr = (Alpha_Expression*)malloc(sizeof(Alpha_Expression));
    expr->type = Alpha_Expression_New_table;
    expr->sym = Alpha_CreateSymbol(symbolName, ALPHA_SYMTYPE_TABLE);
    
    if(!expr){
        AERR("Could not allocate memory for table creation\n");
        return ALPHA_NIL;
    }

    return expr;
}

Alpha_Expression* Alpha_CreateTableElementExpression(void){
    Alpha_Expression* expr = (Alpha_Expression*)malloc(sizeof(Alpha_Expression));
    expr->type = Alpha_Expression_Table_item; /* Damnn */
    /* Might need to pass some stuff in idk */

    /* Linking will be done elsewhere idfk */

    return expr;
}

/* POOPIE FUNCTIONS */

Alpha_Expression* Alpha_CreateVariableExpression(char *symbolName){
    if(!symbolName){ AERR("Null symbolName in expression creation via charval\n"); return ALPHA_NIL; }

    Alpha_Expression* expr = Alpha_NewExpression(Alpha_Expression_Variable);
    if(!expr){
        AERR("Could not allocate memory for Alpha_Expression\n");
        return ALPHA_NIL;
    }

    Alpha_Symbol* symbol = Alpha_LookupBysymbolName(symbolName);
    if(!symbol){
        symbol = Alpha_CreateSymbol(strdup(symbolName), ALPHA_SYMTYPE_LOCAL);
        if(!symbol){
            AERR("Could not allocate memory for Alpha_Symbol\n");
            return ALPHA_NIL;
        }
        Alpha_InsertSymbol(symbol);
    }
    expr->sym = symbol;

    return expr;
}

Uint8 Alpha_IsTheVariableTemporary(Alpha_Symbol *sym){
    if(sym == NULL || sym->symbolName == NULL) return ALPHA_FAIL;

    if(sym->symbolName[0] == '_' && sym->symbolName[1] == 't') return ALPHA_SUCCESS;

    return ALPHA_FAIL;
}
