/**
 * @file parser.h
 * @brief Syntax analyser (parser)
 */
#ifndef PARSER_H
#define PARSER_H

#include "../header/lexer.h"

typedef struct {
    int errorCount;
} CompileRet;

CompileRet compile(FILE* sourceCode);       // the syntax analyser controls the compilation process
char* _getTokenClassName(int token_class);  // returns token class name given token class number
char* _getLexerErrorMessage(int currState);

#endif  // PARSER_H