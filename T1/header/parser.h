/**
 * @file parser.h
 * @brief Syntax analyser
 */
#ifndef PARSER_H
#define PARSER_H

#include "../header/lexer.h"

int compile( FILE* sourceCode );
char* getTokenClassName( int token_class );

#endif // PARSER_H