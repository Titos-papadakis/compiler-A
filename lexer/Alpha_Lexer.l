%{
    #include "../src/Alpha_Token.h"
%}

%option noyywrap
%option yylineno

IDENTIFIER          [_a-zA-Z][a-zA-Z_0-9]*
OPERATOR            (==|!=|>=|<=|\+\+|--|=|\+|-|\*|\/|%|>|<)
PUNCTUATION         (;|,|\:|::|\.|\.\.|\{|\}|\]|\[|\(|\))
CONST_INT           [1-9]+[0-9]*|0
REAL_NUM            [0-9]+(\.[0-9]+)
STRING              \"([^\\\"\n]|(\\.))*\"
WHITESPACE          [\r\t\n\\ ]+
LINE_COMMENT        "//".*

%x COMMENT

%%

"local"    { return LEX_LOCAL; }
"if"       { return LEX_IF; }
"while"    { return LEX_WHILE; }
"for"      { return LEX_FOR; }
"function" { return LEX_FUNCTION; }
"return"   { return LEX_RETURN; }
"break"    { return LEX_BREAK; }
"continue" { return LEX_CONTINUE; }
"true"     { return LEX_TRUE; }
"false"    { return LEX_FALSE; }
"nil"      { return LEX_NIL; }
"else"     { return LEX_ELSE; }
"not"      { return LEX_NOT; }
"and"      { return LEX_AND; }
"or"       { return LEX_OR; }
"%"        { return LEX_MODULO; }



"/*"                {
                        if (Alpha_CommentSectionNesting == 0) {
                            Alpha_Token* commentToken = Alpha_CreateToken();
                            commentToken->tokenNum = yylineno;
                            commentToken->tokenCategory = ALPHA_TOKEN_CATEGORY_COMMENT_SECTION;
                            Alpha_CommentSectionTokenTemp = commentToken;
                        }

                        Alpha_CommentSectionNesting++;
                        BEGIN(COMMENT);
                    }

<COMMENT>"/*"       {
                        Alpha_CommentSectionNesting++;
                    }

<COMMENT>"*/"       {
                        Alpha_CommentSectionNesting--;
                        if (Alpha_CommentSectionNesting == 0) {
                            Alpha_CommentSectionTokenTemp->tokenCommentBlockEndLine = yylineno;
                            // Alpha_StoreToken(Alpha_CommentSectionTokenTemp);
                            Alpha_CommentSectionTokenTemp = ALPHA_NIL;
                            BEGIN(INITIAL);
                        }
                    }

<COMMENT>.|\n       {
                        /* Consume all other characters inside comment */
                    }


<COMMENT><<EOF>>    {
                        int line = 0;
                        if(Alpha_CommentSectionTokenTemp != NULL){
                            line = Alpha_CommentSectionTokenTemp->tokenLineNum;
                        }
                        AERR("Lexical Error: Unterminated block comment (opened at line %d, reached EOF)\n", line);

                        Alpha_CommentSectionTokenTemp = ALPHA_NIL;
                        Alpha_CommentSectionNesting = 0;

                        exit(ALPHA_FAIL);
                    }

{LINE_COMMENT}  {
                Alpha_Token* token = Alpha_CreateToken();
                token->tokenCategory = ALPHA_TOKEN_CATEGORY_LINE_COMMENT;
                // Alpha_StoreToken(token);
                }
            
{OPERATOR}      {
                Alpha_Token* token = Alpha_CreateToken();
                token->tokenCategory = Alpha_RecognizeAlphaToken(token->tokenContent);
                // Alpha_StoreToken(token);

                /* This function recognizes the token, and returns it to the parser */
                return Alpha_RecogniseParsedToken(token);
                }

{IDENTIFIER}    {
                Alpha_Token* token = Alpha_CreateToken();
                token->tokenCategory = Alpha_RecognizeAlphaToken(token->tokenContent);
                // Alpha_StoreToken(token);


                /* We return immediately, after setting the value */
                yylval.stringValue = strdup(yytext); /* Very important line */
                return LEX_ID;
                }

{PUNCTUATION}   {
                Alpha_Token* token = Alpha_CreateToken();
                token->tokenCategory = Alpha_RecognizeAlphaToken(token->tokenContent);
                // Alpha_StoreToken(token);

                return Alpha_RecogniseParsedToken(token);
                }
            
{CONST_INT}     {
                Alpha_Token* token = Alpha_CreateToken();
                token->tokenCategory = ALPHA_TOKEN_CATEGORY_CONST_INT;
                // Alpha_StoreToken(token);

                return LEX_INTEGER;
                }
            
{REAL_NUM}      {
                Alpha_Token* token = Alpha_CreateToken();
                token->tokenCategory = ALPHA_TOKEN_CATEGORY_REAL_NUM;
                // Alpha_StoreToken(token);

                return LEX_FLOAT;
                }
            
{STRING}        {
                Alpha_Token* token = Alpha_CreateToken();
                token->tokenCategory = ALPHA_TOKEN_CATEGORY_STRING;
                Alpha_ReplaceEscapeCharacters(token->tokenContent);
                // Alpha_StoreToken(token);

                return LEX_STRING;
                }
            
{WHITESPACE}    {
                /* Do nothing */
                }

.               {
                AERR("Unrecognized Token: [%s] at l:%d\n", yytext, yylineno);
                }
%%
