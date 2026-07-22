#include "Alpha_Symbol.h"
#include "Alpha_Token.h"

extern int yyparse(void);
extern FILE *yyin;

int main(int argc, char **argv){
    Alpha_AllocateSymbolTableBuckets();
    Alpha_CreateLibraryFunctions();
    
    if(argc > 1){
        yyin = fopen(argv[1], "r");
        
        if(!yyin){
            AERR("Cannot open input file %s\n", argv[1]);
            return ALPHA_FAIL;
        }
    }else{
        yyin = stdin;
    }
    
    ADEB("Parse entry\n");
    yyparse();
    ADEB("Parse exit\n");

    for(Uint8 i = 0 ; i < ALPHA_SYMBOL_TABLE_BUCKETS ; i++){
        Alpha_PrintBucket(i);
    }

    return ALPHA_SUCCESS;
}
