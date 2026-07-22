#ifndef __ALPHA_TOKEN_H__
/**
 * @brief Implemention the basic token structure
 * for parsing and recognizing parsed lines, recognized tokens
 * are handled differenty based on what regex rule they trigger
 * some of them do not require recognition, as we already know their
 * category by the regex itself, so inside their rule we set
 * their category manually (CONSTINT, REALNUM, STRING ect.)
 * excess recognition is required for segregating keywords
 * from identifiers, operators ect. (as seen from the
 * Alpha_RecognizeAlphaToken(...) function)
 */
#define __ALPHA_TOKEN_H__

#include "Alpha_Utilities.h"
#include "../parser/Alpha_Parser.h" /* Produced by Yakk-Bison */

/* The different type of regex used in .l file */
#define ALPHA_REGEX_COUNT 8

/*
    Token categories enumerators each
recognized token will belong to one of these
categories, else it will be invalid.
*/
typedef enum alpha_token_category
{
    /* Keywords */
    ALPHA_TOKEN_CATEGORY_ALPHA_NULL_TOKEN = -2, /* Null tokenContent */
    ALPHA_TOKEN_UNRECOGNIZED_TOKEN = -1,        /* This will be defaulted in the switch case function, if no category is valid for the token */
    ALPHA_TOKEN_CATEGORY_IF,
    ALPHA_TOKEN_CATEGORY_ELSE,
    ALPHA_TOKEN_CATEGORY_WHILE,
    ALPHA_TOKEN_CATEGORY_FOR,
    ALPHA_TOKEN_CATEGORY_FUNCTION,
    ALPHA_TOKEN_CATEGORY_RETURN,
    ALPHA_TOKEN_CATEGORY_BREAK,
    ALPHA_TOKEN_CATEGORY_CONTINUE,
    ALPHA_TOKEN_CATEGORY_AND,
    ALPHA_TOKEN_CATEGORY_NOT,
    ALPHA_TOKEN_CATEGORY_OR,
    ALPHA_TOKEN_CATEGORY_LOCAL,
    ALPHA_TOKEN_CATEGORY_TRUE,
    ALPHA_TOKEN_CATEGORY_FALSE,
    ALPHA_TOKEN_CATEGORY_NIL,

    /* Operators */
    ALPHA_TOKEN_CATEGORY_ASSIGN,
    ALPHA_TOKEN_CATEGORY_ADDITION,
    ALPHA_TOKEN_CATEGORY_SUBTRACTION,
    ALPHA_TOKEN_CATEGORY_MULTIPLICATION,
    ALPHA_TOKEN_CATEGORY_DIVISION,
    ALPHA_TOKEN_CATEGORY_MODULO,
    ALPHA_TOKEN_CATEGORY_EQUALITY,
    ALPHA_TOKEN_CATEGORY_NON_EQUAL,
    ALPHA_TOKEN_CATEGORY_INCREMENT,
    ALPHA_TOKEN_CATEGORY_DECREMENT,
    ALPHA_TOKEN_CATEGORY_GREATER_THAN,
    ALPHA_TOKEN_CATEGORY_LESS_THAN,
    ALPHA_TOKEN_CATEGORY_GREATER_EQUAL_THAN,
    ALPHA_TOKEN_CATEGORY_LESS_EQUAL_THAN,

    /* Punctuation */
    ALPHA_TOKEN_CATEGORY_LEFT_CURLY_BRACES,     /* { */
    ALPHA_TOKEN_CATEGORY_RIGHT_CURLY_BRACES,    /* } */
    ALPHA_TOKEN_CATEGORY_LEFT_SQUARE_BRACKETS,  /* [ */
    ALPHA_TOKEN_CATEGORY_RIGHT_SQUARE_BRACKETS, /* ] */
    ALPHA_TOKEN_CATEGORY_LEFT_PARENTHESES,      /* ( */
    ALPHA_TOKEN_CATEGORY_RIGHT_PARENTHESES,     /* ) */
    ALPHA_TOKEN_CATEGORY_COMA,                  /* , */
    ALPHA_TOKEN_CATEGORY_SEMICOLON,             /* ; */
    ALPHA_TOKEN_CATEGORY_COLON,                 /* : */
    ALPHA_TOKEN_CATEGORY_DOUBLE_COLON,          /* :: */
    ALPHA_TOKEN_CATEGORY_PERIOD,                /* . */
    ALPHA_TOKEN_CATEGORY_DOUBLE_PERIOD,         /* .. */

    ALPHA_TOKEN_CATEGORY_CONST_INT, /* 13 */
    ALPHA_TOKEN_CATEGORY_REAL_NUM,  /* 3.143 */
    ALPHA_TOKEN_CATEGORY_STRING,    /* "thisIsAString" */

    ALPHA_TOKEN_CATEGORY_IDENTIFIER, /* Basically a variable name, words that are not keywords */

    ALPHA_TOKEN_CATEGORY_LINE_COMMENT,    /* "//" Comments */
    ALPHA_TOKEN_CATEGORY_COMMENT_SECTION, /* '/*' Comments */
} Alpha_TokenCategory;

/*
    Tokenized inputs, required fields for printing
the recognized token's data.
*/
typedef struct alpha_token_t
{
    int tokenLineNum; /* Line number of token */

    /**
     * @brief Line number of the end of comment block token
     *
     * @warning ## Use only for block comment tokens
     * and NOT other types
     */
    int tokenCommentBlockEndLine;
    int tokenNum;                      /* Number of recognized token */
    char *tokenContent;                /* Token contents */
    Alpha_TokenCategory tokenCategory; /* Token category, enumerator will hash in a switch case later */
    struct alpha_token_t *next;
} Alpha_Token;

/* Alpha tokens list */
extern Alpha_Token **Alpha_Tokens;

/* This will hold data aligned with sectioned comments, to bypass problems with labels inside conditional
regex rules */
extern Alpha_Token *Alpha_CommentSectionTokenTemp;

/* This will be used as a nesting counter for the program not to read other '/*' as a division and
a multiplication inside of a comment section */
extern int Alpha_CommentSectionNesting;

/* Alpha token category names */
extern const char *Alpha_CategoryNames[ALPHA_REGEX_COUNT];

/**
 * @brief Recognizes the given token, and assings the token category to it
 *
 * @param tokenContents Token contents to scan, in order to find what type of category the token is
 *
 * @returns The recognized category
 */
Alpha_TokenCategory Alpha_RecognizeAlphaToken(const char *tokenContents);

/**
 * @brief Creates the newly parsed token from yytext
 * and assings to it it's fields
 *
 * @returns The newly created token
 */
Alpha_Token *Alpha_CreateToken(void);

/**
 * @brief Stores a token inside the dunamic
 * Alpha_Tokens list l:120
 *
 * @param token Token to store
 *
 * @returns 0 on success, 1 on failure
 */
int Alpha_StoreToken(Alpha_Token *token);

/**
 * @brief Prints the fields of the recognized tokens
 *
 * @param token Token to print
 */
void Alpha_PrintToken(Alpha_Token *token);

/**
 * @brief Prints all tokens stored
 * inside the dynamic Alpha_Tokens list l:120
 */
void Alpha_PrintTokens(void);

/**
 * @brief This function prints the contents of a token
 * inside a sectioned comment, this is used at the condition
 * for sectioned comments, that are handled differently
 *
 * @warning # ONLY USED AT THE CONDITIONAL REGEX OF SECTIONED COMMENTS
 * DON'T CALL OTHERWISE
 *
 * @param token Token to print
 */
void Alpha_PrintTokenInCommentSection(Alpha_Token *token);

/**
 * @brief Finds the correct type of the parsed token
 * based on the triggered category of the token
 *
 * @param tokenCategory Token's category
 *
 * @returns A string for printing further info of the token
 */
char *Alpha_EvaluateAlphaTokenType(Alpha_TokenCategory tokenCategory);

/**
 * @brief Frees all created tokens from
 * the dynamic Alpha_Tokens list l:120
 *
 * @returns 0 on success, 1 on failure
 */
int Alpha_FreeTokens(void);

/**
 * @brief Replaces found string escape
 * character with the actual escape character
 * 
 * @param escapeString String to scan
 * 
 * @returns 0 on success, 1 on failure 
 */
int Alpha_ReplaceEscapeCharacters(char* escapeString);

/**
 * @brief This is what will return the parsed token
 * to the parser
 * 
 * @param token Token to recognize
 * 
 * @returns The enumerator token that corresponds to the defined
 * grammar tokens
 */
enum yytokentype Alpha_RecogniseParsedToken(Alpha_Token* token);

#endif /* __ALPHA_TOKEN_H__ */