/**
 * @file parser.c
 * @brief Syntax analyser (parser) implementation
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include "../header/parser.h"
#include "../header/vector.h"

#pragma GCC diagnostic ignored "-Wchar-subscripts"
/**
 * @brief Controls the compilation process.
 * 
 * @param sourceCode pointer to P-- source code file
 * @return int compiler termination status
 */
int compile( FILE* sourceCode ){

    // initializes a lexical analyser (lexer)
    Lexer* lexer;
    lexerInit(lexer);
    
    // stores the last lexem read by the lexer
    String* buffer;
    stringInit( buffer );
    
    int tokenClass = 0;     // last token class identified by the lexer     
    int flag = 0;           // compiler status flag

    while( tokenClass != EOF ){ // stops when the lexer has finished reading the source code file

        // asks lexer for next token
        nextToken( sourceCode, lexer, buffer, &tokenClass );
        
        if( tokenClass == ERROR ){
            printf( "%s", buffer );
            flag = -1;              // if an error was found, compilation fails
        }
        else
            printf( "%s, %s\n", buffer->str, getTokenClassName( tokenClass ) ); // print token pair: <value, class>
    }

    return flag;
}

/**
 * @brief Given token class number, returns token class name for user-friendly printing.
 * 
 * @param tokenClass token class number
 * @return char* token class name
 */
char* getTokenClassName( int tokenClass ){
    return tokenClassName[ tokenClass ];
}