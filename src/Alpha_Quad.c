#include "Alpha_Quad.h"

Alpha_Quad** Alpha_Quads = ALPHA_NIL;
Uint32 Alpha_TotalQuads = 0;
Uint32 Alpha_CurrentQuad = 0;

void Alpha_AllocateQuadsArray(void){
    Alpha_Quads = (Alpha_Quad**)malloc(sizeof(Alpha_Quad*) * ALPHA_INITIAL_QUADS_LIMIT);
    for(Uint8 i = 0 ; i < ALPHA_INITIAL_QUADS_LIMIT ; i++){
        Alpha_Quads[i] = ALPHA_NIL; /* Initialise to nil at first */
    }

    return; /* So goofy */
}

Alpha_Quad** Alpha_ReallocateQuadsArray(unsigned quadCount){
    if(quadCount >= ALPHA_INITIAL_QUADS_LIMIT){
        ADEB("Reallocating bytes for quads array\n");

        Alpha_Quad** newQuadsArray = (Alpha_Quad**)realloc(Alpha_Quads, sizeof(Alpha_Quad*) * quadCount);
        if(newQuadsArray == ALPHA_NIL){
            AERR("Failed to realloc quad array\n");
        }

        return newQuadsArray;
    }

    return Alpha_Quads;
}

Alpha_Quad* Alpha_CreateQuad(Alpha_IOpcode op, Alpha_Expression* arg1, Alpha_Expression* arg2, Alpha_Expression* res){
    Alpha_Quad* newQuad = (Alpha_Quad*)malloc(sizeof(Alpha_Quad)); /* Allocate memory for the quad */
    if(newQuad == ALPHA_NIL){ AERR("Couldn't create quad\n"); }

    newQuad->op = op;
    newQuad->arg1 = arg1;
    newQuad->arg2 = arg2;
    newQuad->result = res;    
    newQuad->line = 0; /* Unsure: idfk. */
    newQuad->label = -1;

    Alpha_CurrentQuad++;
    Alpha_TotalQuads++;

    Alpha_InsertQuad(newQuad);
    return newQuad;
}

void Alpha_InsertQuad(Alpha_Quad* quad){
    /* This checks if we need to allocate more memory onto the buffer 
    with the quads, before we insert the new quad :< */
    Alpha_Quads = Alpha_ReallocateQuadsArray(Alpha_TotalQuads); /* Assign the newly allocated quads array to our global */

    for(Uint8 i = 0 ; i < Alpha_TotalQuads ; i++){
        if(Alpha_Quads[i] == ALPHA_NIL){
            Alpha_Quads[i] = quad; /* Very easy linking */
            break;
        }
    }

    return;
}

void Alpha_PrintAllQuads(void) {
    out = fopen("out.txt", "w");
    fprintf(out, "\n%-6s %-12s %-10s %-10s %-10s %-6s\n", "quad#", "opcode", "result", "arg1", "arg2", "label");
    fprintf(out, "---------------------------------------------------------------\n");

    for (unsigned i = 0; i < Alpha_TotalQuads; i++) {
        Alpha_Quad* q = Alpha_Quads[i];

        if(q != ALPHA_NIL){
            fprintf(out, "%-6u %-12s ", i, Alpha_IOpcodeToString(q->op));
            Alpha_PrintExpression(q->result);
            Alpha_PrintExpression(q->arg1);
            Alpha_PrintExpression(q->arg2);
            if(q->label >= 0){
                fprintf(out, "%-6d\n", q->label);
            }
            else{
                fprintf(out, "\n");
            }
        }
    }
    fprintf(out, "---------------------------------------------------------------\n");
    fclose(out);

    return;
}

void Alpha_Backpatch(Alpha_List *head, int quadLabel){
    Alpha_List *current = head;

    while(current != ALPHA_NIL){
        int indexQuad = current->label;
        printf(">> Backpatching quad #%d with label %d\n", indexQuad, quadLabel); 

        if(indexQuad < Alpha_TotalQuads && Alpha_Quads[indexQuad] != ALPHA_NIL){
            Alpha_Quads[indexQuad]->label = quadLabel;
        }

        Alpha_List *tmp = current;
        current = current->next;
        free(tmp);
    }
}

Alpha_List* Alpha_Merge(Alpha_List *list1, Alpha_List * list2){
    
    if(list1 == ALPHA_NIL) return list2;

    Alpha_List *tmp = list1;

    while (tmp->next != ALPHA_NIL)
    {
        tmp = tmp->next;
    }
    tmp->next = list2;

    return list1;
}

Alpha_List*  Alpha_MakeList(int nextQuadLabel){
    Alpha_List *list = (Alpha_List*)malloc(sizeof(Alpha_List));

    list->label = nextQuadLabel;
    list->next = ALPHA_NIL;

    return list;
}

void Alpha_PrintList(Alpha_List *list, const char *name) {
    #ifdef ALPHA_DEBUG

    printf("%s = [", name);
    while (list != ALPHA_NIL) {
        printf("%d", list->label);
        list = list->next;
        if (list) printf(", ");
    }
    printf("]\n");

    #endif

    return;
}


Alpha_Expression* Alpha_CreateBoolExpression(int value) {
    Alpha_Expression* expr = (Alpha_Expression*)malloc(sizeof(Alpha_Expression));
    expr->type = Alpha_Expression_Const_bool;
    expr->boolConst = value;
    return expr;
}