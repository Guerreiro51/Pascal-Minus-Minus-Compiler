/**
 * @file parser.c
 * @brief Syntax analyser (parser) implementation
 *
 */
#include "../header/parser.h"

#include <stdio.h>
#include <stdlib.h>

#include "../header/vector.h"

/**
 * @brief Controls the compilation process.
 *
 * @param sourceCode pointer to P-- source code file
 * @return int compiler termination status
 */
CompileRet compile(FILE* sourceCode) {
    // initializes a lexical analyser (lexer)
    Lexer lexer;
    lexerInit(&lexer);

    // stores the last lexem read by the lexer
    String buffer;
    stringInit(&buffer);

    int tokenClass = 0;  // last token class identified by the lexer
    CompileRet compileRet;
    compileRet.errorCount = 0;

    while (tokenClass != EOF) {  // stops when the lexer has finished reading the source code file
        nextToken(&lexer, &buffer, sourceCode, &tokenClass);

        if (tokenClass == ERROR) {
            printf("Line %d Col %d -- '%s'\n%s\n", lexer.currLine, lexer.currCol, buffer.str, _getLexerErrorMessage(lexer.currState));
            compileRet.errorCount++;
        } else if (tokenClass == EOF)
            printf("EOF\n");
        else
            printf("%s, %s\n", buffer.str, _getTokenClassName(tokenClass));  // print token pair: <value, class>
    }

    return compileRet;
}

/**
 * @brief Given token class number, returns token class name for user-friendly printing.
 *
 * @param tokenClass token class number
 * @return char* token class name
 */
char* _getTokenClassName(int tokenClass) {
    // indexes token class names for user-friendly printing
    static char* tokenClassName[] = {"N_REAL", "N_INTEGER", "OP_ADD", "OP_MULT", "RELATION",
                                     "ASSIGN", "DECLARE_TYPE", "SEMICOLON", "COLON",
                                     "OPEN_PAR", "CLOSE_PAR", "ID", "BEGIN", "CONST",
                                     "DO", "END", "ELSE", "IF", "INTEGER", "FOR", "PROGRAM", "PROCEDURE",
                                     "REAL", "READ", "THEN", "VAR", "WRITE", "WHILE", "ERROR"};
    return tokenClassName[tokenClass];
}

char* _getLexerErrorMessage(int currState) {
    static char* lexerErrorMessages[NUMBER_OF_STATES] = {"", "", "",
                                                         "Error: Invalid Identifier", "",
                                                         "Error: expected a number but found a character", "", "", "",
                                                         "Error: did you mean to type a real number?", "", "", "", "",
                                                         "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
                                                         "Error: Unexpected end of file"};
    return lexerErrorMessages[currState];
}