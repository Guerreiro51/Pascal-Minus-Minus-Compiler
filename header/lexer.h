/**
 * @file lexer.h
 * @brief  Lexical analyser (lexer)
 */
#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../header/vector.h"

#define NUMBER_OF_STATES 32  // number of states of the lexic analyser automaton
#define NUMBER_OF_CHARS 128  // number of ASCII characters

#define NUMBER_OF_STATES_PROTECTED_SYMBOLS 64  // number of states of the protected symbol detection automaton
#define NUMBER_OF_LOWER_CASE_LETTERS 26        // number of lower case letters (inputs of the protected symbol detection automaton)

// existing token classes
enum TOKEN_CLASS {  N_REAL, N_INTEGER, OP_ADD, OP_MULT, RELATION, 
                    ASSIGN, DECLARE_TYPE, SEMICOLON, COLON, 
                    OPEN_PAR, CLOSE_PAR, ID, BEGIN, CONST, 
                    DO, END, ELSE, IF, INTEGER, FOR, PROGRAM, PROCEDURE,
                    REAL, READ, THEN, VAR, WRITE, WHILE, ERROR};

typedef struct {
    int transitionMatrix[NUMBER_OF_STATES][NUMBER_OF_CHARS];
    bool finalState[NUMBER_OF_STATES];       // whether a state is final or not
    char finalStateClass[NUMBER_OF_STATES];  // associates each final state with a token class

    int protectedSymbolMatrix[NUMBER_OF_STATES_PROTECTED_SYMBOLS][NUMBER_OF_LOWER_CASE_LETTERS];
    char protectedSymbolFinalStates[NUMBER_OF_STATES_PROTECTED_SYMBOLS];

    char currChar;
    int fscanfFlag;
    int currState;
    int currLine;
    int currCol;

    bool lastWasNumberOrIdent;
} Lexer;

void lexerInit(Lexer* lexer);
void nextToken(Lexer* lexer, String* buffer, FILE* sourceCode, int* tokenClass);

void _buildTransitionMatrix(int transitionMatrix[NUMBER_OF_STATES][NUMBER_OF_CHARS]);
void _buildFinalStates(bool finalState[NUMBER_OF_STATES], char finalStateClass[NUMBER_OF_STATES]);
void _buildProtectedSymbolMatrix(int protectedSymbolMatrix[NUMBER_OF_STATES_PROTECTED_SYMBOLS][NUMBER_OF_LOWER_CASE_LETTERS]);
void _buildProtectedSymbolFinalStates(char protectedSymbolFinalState[NUMBER_OF_STATES_PROTECTED_SYMBOLS]);

void _fillOther(int transitionMatrix[NUMBER_OF_STATES][NUMBER_OF_CHARS], int startState, int endState);
void _fillWord(int protectedSymbolMatrix[NUMBER_OF_STATES_PROTECTED_SYMBOLS][NUMBER_OF_LOWER_CASE_LETTERS], const char word[], int firstState, int secondState);

void _nextChar(Lexer* lexer, FILE* sourceCode);
void _dealWithEOF(Lexer* lexer, String* buffer, FILE* sourceCode, int* tokenClass);
void _nextState(Lexer* lexer);
void _identifyTokenClass(Lexer* lexer, String* buffer, FILE* sourceCode, int* tokenClass);
int _checkIfProtectedSymbol(Lexer* lexer, String* buffer);

#endif  // LEXER_H