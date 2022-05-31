/**
 * @file parser.h
 * @brief Syntax analyser (parser)
 */
#ifndef PARSER_H
#define PARSER_H

#include "../header/lexer.h"

int compile( FILE* sourceCode );            // the syntax analyser controls the compilation process
char* getTokenClassName( int token_class ); // returns token class name given token class number

#endif // PARSER_H