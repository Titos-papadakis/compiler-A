# Compiler for lexical analyzer
FLEXX = flex
# Source code of lexer
FLEXSRC = Alpha_Lexer.l
# Source code of scanner
SCANNERSRC = Alpha_Token.c
# C/CXX Compiler
CC = gcc
# Executable name
EXEC_NAME = ac
# Flex flag
FLEXF = -lfl

# Bison-Yak
BIS = bison

# Generate the .c file
ALPHA_LEXER:
	$(FLEXX) -o lexer/Alpha_Lexer.c lexer/$(FLEXSRC)

# Compile the .c file
ALPHA_SCANNER:
	$(CC) src/Alpha_Compiler.c src/Alpha_Symbol.c lexer/Alpha_Lexer.c src/$(SCANNERSRC) parser/Alpha_Parser.c -o bin/$(EXEC_NAME) $(FLEXF)

ALPHA_PARSER:
	$(BIS) --yacc --defines --output=parser/Alpha_Parser.c parser/Alpha_Parser.y

all: ALPHA_PARSER ALPHA_LEXER ALPHA_SCANNER


clean:
	rm lexer/Alpha_Lexer.c
	rm parser/Alpha_Parser.h
	rm parser/Alpha_Parser.c
	rm bin/ac
