#include "Alpha_Token.h"

Alpha_Token **Alpha_Tokens = ALPHA_NIL;

Alpha_Token *Alpha_CommentSectionTokenTemp;

int Alpha_CommentSectionNesting = 0;

int Alpha_GlobalTokenNum = 0;

const char *Alpha_CategoryNames[ALPHA_REGEX_COUNT] = {
    "KEYWORD",
    "OPERATOR",
    "CONST_INT",
    "REAL_NUM",
    "STRING",
    "IDENTIFIER",
    "COMMENT",
    "PUNCTUATION",
};

Alpha_TokenCategory Alpha_RecognizeAlphaToken(const char *tokenContents)
{
    if (tokenContents == ALPHA_NIL)
    {
        AERR("Null token\n");
        return ALPHA_TOKEN_CATEGORY_ALPHA_NULL_TOKEN;
    }

    /* Keywords */
    if (strcmp(tokenContents, "if") == 0)
        return ALPHA_TOKEN_CATEGORY_IF;
    if (strcmp(tokenContents, "else") == 0)
        return ALPHA_TOKEN_CATEGORY_ELSE;
    if (strcmp(tokenContents, "while") == 0)
        return ALPHA_TOKEN_CATEGORY_WHILE;
    if (strcmp(tokenContents, "for") == 0)
        return ALPHA_TOKEN_CATEGORY_FOR;
    if (strcmp(tokenContents, "function") == 0)
        return ALPHA_TOKEN_CATEGORY_FUNCTION;
    if (strcmp(tokenContents, "return") == 0)
        return ALPHA_TOKEN_CATEGORY_RETURN;
    if (strcmp(tokenContents, "break") == 0)
        return ALPHA_TOKEN_CATEGORY_BREAK;
    if (strcmp(tokenContents, "continue") == 0)
        return ALPHA_TOKEN_CATEGORY_CONTINUE;
    if (strcmp(tokenContents, "and") == 0)
        return ALPHA_TOKEN_CATEGORY_AND;
    if (strcmp(tokenContents, "not") == 0)
        return ALPHA_TOKEN_CATEGORY_NOT;
    if (strcmp(tokenContents, "or") == 0)
        return ALPHA_TOKEN_CATEGORY_OR;
    if (strcmp(tokenContents, "local") == 0)
        return ALPHA_TOKEN_CATEGORY_LOCAL;
    if (strcmp(tokenContents, "true") == 0)
        return ALPHA_TOKEN_CATEGORY_TRUE;
    if (strcmp(tokenContents, "false") == 0)
        return ALPHA_TOKEN_CATEGORY_FALSE;
    if (strcmp(tokenContents, "nil") == 0)
        return ALPHA_TOKEN_CATEGORY_NIL;

    /* Operators */
    if (strcmp(tokenContents, "=") == 0)
        return ALPHA_TOKEN_CATEGORY_ASSIGN;
    if (strcmp(tokenContents, "+") == 0)
        return ALPHA_TOKEN_CATEGORY_ADDITION;
    if (strcmp(tokenContents, "-") == 0)
        return ALPHA_TOKEN_CATEGORY_SUBTRACTION;
    if (strcmp(tokenContents, "*") == 0)
        return ALPHA_TOKEN_CATEGORY_MULTIPLICATION;
    if (strcmp(tokenContents, "/") == 0)
        return ALPHA_TOKEN_CATEGORY_DIVISION;
    if (strcmp(tokenContents, "%") == 0)
        return ALPHA_TOKEN_CATEGORY_MODULO;
    if (strcmp(tokenContents, "==") == 0)
        return ALPHA_TOKEN_CATEGORY_EQUALITY;
    if (strcmp(tokenContents, "!=") == 0)
        return ALPHA_TOKEN_CATEGORY_NON_EQUAL;
    if (strcmp(tokenContents, "++") == 0)
        return ALPHA_TOKEN_CATEGORY_INCREMENT;
    if (strcmp(tokenContents, "--") == 0)
        return ALPHA_TOKEN_CATEGORY_DECREMENT;
    if (strcmp(tokenContents, ">") == 0)
        return ALPHA_TOKEN_CATEGORY_GREATER_THAN;
    if (strcmp(tokenContents, "<") == 0)
        return ALPHA_TOKEN_CATEGORY_LESS_THAN;
    if (strcmp(tokenContents, ">=") == 0)
        return ALPHA_TOKEN_CATEGORY_GREATER_EQUAL_THAN;
    if (strcmp(tokenContents, "<=") == 0)
        return ALPHA_TOKEN_CATEGORY_LESS_EQUAL_THAN;

    if (strcmp(tokenContents, "{") == 0)
        return ALPHA_TOKEN_CATEGORY_LEFT_CURLY_BRACES;
    if (strcmp(tokenContents, "}") == 0)
        return ALPHA_TOKEN_CATEGORY_RIGHT_CURLY_BRACES;
    if (strcmp(tokenContents, "[") == 0)
        return ALPHA_TOKEN_CATEGORY_LEFT_SQUARE_BRACKETS;
    if (strcmp(tokenContents, "]") == 0)
        return ALPHA_TOKEN_CATEGORY_RIGHT_SQUARE_BRACKETS;
    if (strcmp(tokenContents, "(") == 0)
        return ALPHA_TOKEN_CATEGORY_LEFT_PARENTHESES;
    if (strcmp(tokenContents, ")") == 0)
        return ALPHA_TOKEN_CATEGORY_RIGHT_PARENTHESES;
    if (strcmp(tokenContents, ",") == 0)
        return ALPHA_TOKEN_CATEGORY_COMA;
    if (strcmp(tokenContents, ";") == 0)
        return ALPHA_TOKEN_CATEGORY_SEMICOLON;
    if (strcmp(tokenContents, ":") == 0)
        return ALPHA_TOKEN_CATEGORY_COLON;
    if (strcmp(tokenContents, "::") == 0)
        return ALPHA_TOKEN_CATEGORY_DOUBLE_COLON;
    if (strcmp(tokenContents, ".") == 0)
        return ALPHA_TOKEN_CATEGORY_PERIOD;
    if (strcmp(tokenContents, "..") == 0)
        return ALPHA_TOKEN_CATEGORY_DOUBLE_PERIOD;

    /* Default to identifier since identifiers, are words that are non keyboards */

    return ALPHA_TOKEN_CATEGORY_IDENTIFIER;
}

Alpha_Token *Alpha_CreateToken(void)
{
    Alpha_Token *newToken = (Alpha_Token *)malloc(sizeof(Alpha_Token));
    if (!newToken)
    {
        return ALPHA_NIL;
    }

    if (yytext == ALPHA_NIL)
    {
        AERR("Null yytext\n");
    }
    newToken->tokenContent = strdup(yytext);
    if (newToken->tokenContent == ALPHA_NIL)
    {
        AERR("Null string after copying yytext\n");
    }
    newToken->tokenLineNum = yylineno;       /* Assign line number */
    newToken->tokenCommentBlockEndLine = -1; /* Out of bounds for normal tokens, at first that is... */
    newToken->tokenNum = Alpha_GlobalTokenNum++;

    return newToken;
}

int Alpha_StoreToken(Alpha_Token *token)
{
    if (token == ALPHA_NIL)
    {
        AERR("Null token to store\n");
        return ALPHA_FAIL;
    }

    if (*Alpha_Tokens == ALPHA_NIL)
    {
        *Alpha_Tokens = token;
        token->next = ALPHA_NIL;
    }
    else
    {
        Alpha_Token *current = *Alpha_Tokens;
        while (current->next != ALPHA_NIL)
        {
            current = current->next;
        }

        current->next = token;
        token->next = ALPHA_NIL;
    }

    return ALPHA_SUCCESS;
}

void Alpha_PrintToken(Alpha_Token *token)
{
    char *category = Alpha_EvaluateAlphaTokenType(token->tokenCategory);

    if (token->tokenCategory >= 0 && token->tokenCategory <= 14)
    {
        printf("%d: #%d  '%s'  %s  %s <- enumerated\n", token->tokenLineNum, token->tokenNum, token->tokenContent, Alpha_CategoryNames[0], category);
    }
    else if (token->tokenCategory >= 15 && token->tokenCategory <= 28)
    {
        printf("%d: #%d  '%s'  %s  %s <- enumerated\n", token->tokenLineNum, token->tokenNum, token->tokenContent, Alpha_CategoryNames[1], category);
    }
    else if (token->tokenCategory >= 29 && token->tokenCategory <= 40)
    {
        printf("%d: #%d  '%s'  %s  %s <- enumerated\n", token->tokenLineNum, token->tokenNum, token->tokenContent, Alpha_CategoryNames[7], category);
    }
    else if (token->tokenCategory == ALPHA_TOKEN_CATEGORY_CONST_INT)
    {
        printf("%d: #%d  '%s'  %s  %s <- integer\n", token->tokenLineNum, token->tokenNum, token->tokenContent, Alpha_CategoryNames[2], token->tokenContent);
    }
    else if (token->tokenCategory == ALPHA_TOKEN_CATEGORY_REAL_NUM)
    {
        printf("%d: #%d  '%s'  %s  %s <- real number\n", token->tokenLineNum, token->tokenNum, token->tokenContent, Alpha_CategoryNames[3], token->tokenContent);
    }
    else if (token->tokenCategory == ALPHA_TOKEN_CATEGORY_STRING)
    {
        printf("%d: #%d  %s  %s  %s <- char*\n", token->tokenLineNum, token->tokenNum, token->tokenContent, Alpha_CategoryNames[4], token->tokenContent);
    }
    else if (token->tokenCategory == ALPHA_TOKEN_CATEGORY_IDENTIFIER)
    {
        printf("%d: #%d  '%s'  %s  '%s' <- char*\n", token->tokenLineNum, token->tokenNum, token->tokenContent, Alpha_CategoryNames[5], token->tokenContent);
    }
    else if (token->tokenCategory == ALPHA_TOKEN_CATEGORY_LINE_COMMENT)
    {
        printf("%d: #%d  COMMENT LINE_COMMENT\n", token->tokenLineNum, token->tokenNum);
    }
    else if (token->tokenCategory == ALPHA_TOKEN_CATEGORY_COMMENT_SECTION)
    {
        /* Print here, for all tokens to be in order */
        Alpha_PrintTokenInCommentSection(token);
    }

    return;
}

void Alpha_PrintTokens(void){
    Alpha_Token *current = *Alpha_Tokens;

    while (current != ALPHA_NIL){
        Alpha_PrintToken(current);

        current = current->next;
    }

    return;
}

void Alpha_PrintTokenInCommentSection(Alpha_Token *token){
    printf("%d: #%d '%d - %d' %s BLOCK_COMMENT\n", token->tokenLineNum, token->tokenNum, token->tokenLineNum,
           token->tokenCommentBlockEndLine, Alpha_CategoryNames[6]);

    return;
}

char* Alpha_EvaluateAlphaTokenType(Alpha_TokenCategory tokenCategory){
    switch(tokenCategory){
        case ALPHA_TOKEN_CATEGORY_IF:
            return "IF";
        case ALPHA_TOKEN_CATEGORY_ELSE:
            return "ELSE";
        case ALPHA_TOKEN_CATEGORY_WHILE:
            return "WHILE";
        case ALPHA_TOKEN_CATEGORY_FOR:
            return "FOR";
        case ALPHA_TOKEN_CATEGORY_FUNCTION:
            return "FUNCTION";
        case ALPHA_TOKEN_CATEGORY_RETURN:
            return "RETURN";
        case ALPHA_TOKEN_CATEGORY_BREAK:
            return "BREAK";
        case ALPHA_TOKEN_CATEGORY_CONTINUE:
            return "CONTINUE";
        case ALPHA_TOKEN_CATEGORY_AND:
            return "AND";
        case ALPHA_TOKEN_CATEGORY_NOT:
            return "NOT";
        case ALPHA_TOKEN_CATEGORY_OR:
            return "OR";
        case ALPHA_TOKEN_CATEGORY_LOCAL:
            return "LOCAL";
        case ALPHA_TOKEN_CATEGORY_TRUE:
            return "TRUE";
        case ALPHA_TOKEN_CATEGORY_FALSE:
            return "FALSE";
        case ALPHA_TOKEN_CATEGORY_NIL:
            return "NIL";
        case ALPHA_TOKEN_CATEGORY_ASSIGN:
            return "ASSIGN";
        case ALPHA_TOKEN_CATEGORY_ADDITION:
            return "ADDITION";
        case ALPHA_TOKEN_CATEGORY_SUBTRACTION:
            return "SUBTRACTION";
        case ALPHA_TOKEN_CATEGORY_MULTIPLICATION:
            return "MULTIPLICATION";
        case ALPHA_TOKEN_CATEGORY_DIVISION:
            return "DIVISION";
        case ALPHA_TOKEN_CATEGORY_MODULO:
            return "MODULO";
        case ALPHA_TOKEN_CATEGORY_EQUALITY:
            return "EQUALITY";
        case ALPHA_TOKEN_CATEGORY_NON_EQUAL:
            return "NON_EQUAL";
        case ALPHA_TOKEN_CATEGORY_INCREMENT:
            return "PLUS_PLUS";
        case ALPHA_TOKEN_CATEGORY_DECREMENT:
            return "MINUS_MINUS";
        case ALPHA_TOKEN_CATEGORY_GREATER_THAN:
            return "GREATER_THAN";
        case ALPHA_TOKEN_CATEGORY_LESS_THAN:
            return "LESS_THAN";
        case ALPHA_TOKEN_CATEGORY_GREATER_EQUAL_THAN:
            return "GREATER_EQUAL_THAN";
        case ALPHA_TOKEN_CATEGORY_LESS_EQUAL_THAN:
            return "LESS_EQUAL_THAN";
        case ALPHA_TOKEN_CATEGORY_LEFT_CURLY_BRACES:
            return "LEFT_CURLY_BRACES";
        case ALPHA_TOKEN_CATEGORY_RIGHT_CURLY_BRACES:
            return "RIGHT_CURLY_BRACES";
        case ALPHA_TOKEN_CATEGORY_LEFT_SQUARE_BRACKETS:
            return "LEFT_SQUARE_BRACKETS";
        case ALPHA_TOKEN_CATEGORY_RIGHT_SQUARE_BRACKETS:
            return "RIGHT_SQUARE_BRACKETS";
        case ALPHA_TOKEN_CATEGORY_LEFT_PARENTHESES:
            return "LEFT_PARENTHESES";
        case ALPHA_TOKEN_CATEGORY_RIGHT_PARENTHESES:
            return "RIGHT_PARENTHESES";
        case ALPHA_TOKEN_CATEGORY_COMA:
            return "COMA";
        case ALPHA_TOKEN_CATEGORY_SEMICOLON:
            return "SEMICOLON";
        case ALPHA_TOKEN_CATEGORY_COLON:
            return "COLON";
        case ALPHA_TOKEN_CATEGORY_DOUBLE_COLON:
            return "DOUBLE_COLON";
        case ALPHA_TOKEN_CATEGORY_PERIOD:
            return "PERIOD";
        case ALPHA_TOKEN_CATEGORY_DOUBLE_PERIOD:
            return "DOUBLE_PERIOD";
        case ALPHA_TOKEN_CATEGORY_LINE_COMMENT:
            return "LINE_COMMENT";
    }

    return "UNRECOGNIZED_TOKEN_TYPE";
}

int Alpha_FreeTokens(void){
    Alpha_Token *current = *Alpha_Tokens;
    Alpha_Token *next;

    while (current != ALPHA_NIL)
    {
        next = current->next;
        free(current->tokenContent);
        free(current);
        current = next;
    }

    *Alpha_Tokens = ALPHA_NIL; /* reset the list to null */

    return ALPHA_SUCCESS;
}

int Alpha_ReplaceEscapeCharacters(char* escapeString){
    if(escapeString == ALPHA_NIL){ AERR("Null escape character\n"); return ALPHA_FAIL; }

    char* src = escapeString + 1; /* We will scan the source to find any escape sequences, inside of the string, skip teh first '"' */
    char* dst = escapeString; 

    while(*src && *(src + 1) && *src != '"'){
        if(*src == '\\'){ /* Escape character found */
            src++; /* Move one up */
            switch(*src){
                case 'n':
                    *dst = '\n';
                    break;
                case 't':
                    *dst = '\t';
                    break;
                case '\\':
                    *dst = '\\';
                    break;
                case '"':
                    *dst = '"';
                    break;
                case 'r':
                    *dst = 'r';
                    break;
                default:
                    AERR("Unknown escape sequence: \\%c\n", *src);
                    return ALPHA_FAIL;
            }
        }else{
            *dst = *src; /* Just copy */
        }
        /* Move to the next character */
        src++; 
        dst++;
    }
        
    *dst = '\0'; /* Terminate string */

    return ALPHA_SUCCESS;
}

enum yytokentype Alpha_RecogniseParsedToken(Alpha_Token* token){
    if(token == ALPHA_NIL){
        AERR("Null token\n");
        return YYerror;
    }

    /* Keywords */
    if (strcmp(token->tokenContent, "if") == 0) return LEX_IF;
    if (strcmp(token->tokenContent, "else") == 0) return LEX_ELSE;
    if (strcmp(token->tokenContent, "while") == 0) return LEX_WHILE;
    if (strcmp(token->tokenContent, "for") == 0) return LEX_FOR;
    if (strcmp(token->tokenContent, "function") == 0) return LEX_FUNCTION;
    if (strcmp(token->tokenContent, "return") == 0) return LEX_RETURN;
    if (strcmp(token->tokenContent, "break") == 0) return LEX_BREAK;
    if (strcmp(token->tokenContent, "continue") == 0) return LEX_CONTINUE;
    if (strcmp(token->tokenContent, "and") == 0) return LEX_AND;
    if (strcmp(token->tokenContent, "not") == 0) return LEX_NOT;
    if (strcmp(token->tokenContent, "or") == 0) return LEX_OR;
    if (strcmp(token->tokenContent, "local") == 0) return LEX_LOCAL;
    if (strcmp(token->tokenContent, "true") == 0) return LEX_TRUE;
    if (strcmp(token->tokenContent, "false") == 0) return LEX_FALSE;
    if (strcmp(token->tokenContent, "nil") == 0) return LEX_NIL;

    /* Operators */
    if (strcmp(token->tokenContent, "=") == 0) return LEX_ASSIGNMENT;
    if (strcmp(token->tokenContent, "+") == 0) return LEX_ADDITION;
    if (strcmp(token->tokenContent, "-") == 0) return LEX_SUBTRACTION;
    if (strcmp(token->tokenContent, "*") == 0) return LEX_MULTIPLICATION;
    if (strcmp(token->tokenContent, "/") == 0) return LEX_DIVISION;
    if (strcmp(token->tokenContent, "%") == 0) return LEX_MODULO;
    if (strcmp(token->tokenContent, "==") == 0) return LEX_EQUAL;
    if (strcmp(token->tokenContent, "!=") == 0) return LEX_NOT_EQ;
    if (strcmp(token->tokenContent, "++") == 0) return LEX_INCREMENT;
    if (strcmp(token->tokenContent, "--") == 0) return LEX_DECREMENT;
    if (strcmp(token->tokenContent, ">") == 0) return LEX_GREATER;
    if (strcmp(token->tokenContent, "<") == 0) return LEX_LESS;
    if (strcmp(token->tokenContent, ">=") == 0) return LEX_GREATER_EQ;
    if (strcmp(token->tokenContent, "<=") == 0) return LEX_LESS_EQ;

    /* Punctuation */
    if (strcmp(token->tokenContent, "{") == 0) return LEX_LEFT_CURLY_BRACE;
    if (strcmp(token->tokenContent, "}") == 0) return LEX_RIGHT_CURLY_BRACE;
    if (strcmp(token->tokenContent, "[") == 0) return LEX_LEFT_BRACKET;
    if (strcmp(token->tokenContent, "]") == 0) return LEX_RIGHT_BRACKET;
    if (strcmp(token->tokenContent, "(") == 0) return LEX_LEFT_PARENTHESIS;
    if (strcmp(token->tokenContent, ")") == 0) return LEX_RIGHT_PARENTHESIS;
    if (strcmp(token->tokenContent, ",") == 0) return LEX_COMMA;
    if (strcmp(token->tokenContent, ";") == 0) return LEX_SEMI_COLON;
    if (strcmp(token->tokenContent, ":") == 0) return LEX_COLON;
    if (strcmp(token->tokenContent, "::") == 0) return LEX_DOUBLE_COLON;
    if (strcmp(token->tokenContent, ".") == 0) return LEX_DOT;
    if (strcmp(token->tokenContent, "..") == 0) return LEX_DOUBLE_DOT;

    /* Unkown token */
    return LEX_UNKNOWN;
}
