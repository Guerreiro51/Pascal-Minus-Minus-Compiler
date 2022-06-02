/**
 * @file lexer.h
 * @brief  Lexical analyser (lexer)
 */
#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "../header/vector.h"

#define NUMBER_OF_STATES 32 // number of states of the lexic analyser automaton
#define NUMBER_OF_CHARS 128 // number of ASCII characters

#define NUMBER_OF_STATES_PROTECTED_SYMBOLS 64 // number of states of the protected symbol detection automaton
#define NUMBER_OF_LOWER_CASE_LETTERS 26       // number of lower case letters (inputs of the protected symbol detection automaton)

// existing token classes
enum TOKEN_CLASS { N_REAL, N_INTEGER, OP_ADD, OP_MULT, RELATION, 
                    ASSIGN, DECLARE_TYPE, SEMICOLON, COLON, 
                    OPEN_PAR, CLOSE_PAR, ID, BEGIN, CONST, 
                    DO, END, ELSE, IF, INTEGER, FOR, PROGRAM, PROCEDURE,
                    REAL, READ, THEN, VAR, WRITE, WHILE, ERROR };

typedef struct {

    int transitionMatrix[NUMBER_OF_STATES][NUMBER_OF_CHARS];
    bool finalState[NUMBER_OF_STATES];          // marks state as final
    char finalStateClass[NUMBER_OF_STATES];     // associates final state with token class

    int protectedSymbolMatrix[NUMBER_OF_STATES_PROTECTED_SYMBOLS][NUMBER_OF_LOWER_CASE_LETTERS];
    char protectedSymbolFinalStates[NUMBER_OF_STATES_PROTECTED_SYMBOLS];

    int currState;
    bool lastWasNumberOrIdent;

    int currLine;
    int currCol;
} Lexer;

void lexerInit(Lexer* lexer);
bool isNumber(char n);
bool isLetter(char c);
void fillOther( int transitionMatrix[NUMBER_OF_STATES][NUMBER_OF_CHARS], int startState, int endState);
void buildFinalStates( bool finalState[NUMBER_OF_STATES], char finalStateClass[NUMBER_OF_STATES] );
void buildTransitionMatrix( int transitionMatrix[NUMBER_OF_STATES][NUMBER_OF_CHARS] );
void nextToken(FILE* sourceCode, Lexer* lexer, String* buffer, int* token_class);
int lookUpProtectedSymbol( String* buffer, Lexer* lexer );
void fillWord(int protectedSymbolMatrix[NUMBER_OF_STATES_PROTECTED_SYMBOLS][NUMBER_OF_LOWER_CASE_LETTERS], const char word[], int firstState, int secondState);
void buildProtectedSymbolFinalStates( char protectedSymbolFinalState[NUMBER_OF_STATES_PROTECTED_SYMBOLS] );
void buildProtectedSymbolMatrix( int protectedSymbolMatrix[NUMBER_OF_STATES_PROTECTED_SYMBOLS][NUMBER_OF_LOWER_CASE_LETTERS] );
void identifyTokenClass( FILE* sourceCode, Lexer* lexer, String* buffer, int* tokenClass, bool isEOF );

#endif // LEXER_H