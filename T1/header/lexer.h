/**
 * @file lexer.h
 * @brief  Lexical analyser 
 */
#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "../header/vector.h"

#define NUMBER_OF_STATES 32 // number of states of the lexic analyser automaton
#define NUMBER_OF_CHARS 128 // number of ASCII characters

#define NUMBER_OF_STATES_PROTECTED_SYMBOLS 62 // number of states of the protected symbol detection automaton
#define NUMBER_OF_LOWER_CASE_LETTERS 26       // number of lower case letters (inputs of the protected symbol detection automaton)

// existing token classes
enum TOKEN_CLASS { N_REAL, N_INTEGER, OP_ADD, OP_MULT, RELATION, 
                    ASSIGN, DECLARE_TYPE, SEMICOLON, COLON, 
                    OPEN_PAR, CLOSE_PAR, ID, PROGRAM, BEGIN, END,
                    CONST, VAR, REAL, INTEGER, PROCEDURE, ELSE, READ, WRITE, IF, THEN, WHILE, FOR,
                    ERROR };

// idexes token class names for user friendly printing
char* tokenClassName = { "N_REAL", "N_INTEGER", "OP_ADD", "OP_MULT", "RELATION", 
                    "ASSIGN", "DECLARE_TYPE", "SEMICOLON", "COLON", 
                    "OPEN_PAR", "CLOSE_PAR", "ID", "PROGRAM", "BEGIN", "END",
                    "CONST", "VAR", "REAL", "INTEGER", "PROCEDURE", "ELSE", "READ", "WRITE", "IF", "THEN", "WHILE", "FOR" }; 

typedef struct {
    char finalState[NUMBER_OF_STATES];
    char transitionMatrix[NUMBER_OF_STATES][NUMBER_OF_CHARS];
    char protectedSymbolMatrix[NUMBER_OF_STATES_PROTECTED_SYMBOLS][NUMBER_OF_LOWER_CASE_LETTERS];
    char curState;
    bool lastWasNumberOrIdent;
} Lexer;

void lexerInit(Lexer* lexer);
bool isNumber(char n);
bool isLetter(char c);
void fillOther( char transitionMatrix[NUMBER_OF_STATES][NUMBER_OF_CHARS], int startState, int endState);
void buildFinalStates( int finalState[NUMBER_OF_STATES] );
void buildTransitionMatrix( char transitionMatrix[NUMBER_OF_STATES][NUMBER_OF_CHARS] );
void nextToken(FILE* sourceCode, Lexer* lexer, String* buffer, int* token_class);
int lookUpProtectedSymbol( buffer );
void fillWord(char protectedSymbolMatrix[NUMBER_OF_STATES_PROTECTED_SYMBOLS][NUMBER_OF_LOWER_CASE_LETTERS], const char word[], int firstState, int secondState);

#endif // LEXER_H