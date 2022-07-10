/**
 * @file lexer.h
 * @brief Lexical analyser (lexer)
 */
#ifndef LEXER_H
#define LEXER_H

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../header/string.h"

#define NUMBER_OF_STATES 32                    // number of states of the lexic analyser automaton
#define NUMBER_OF_STATES_PROTECTED_SYMBOLS 65  // number of states of the protected symbol detection automaton
#define NUMBER_OF_CHARS 128                    // number of ASCII characters
#define NUMBER_OF_LOWER_CASE_LETTERS 26        // number of lower case letters (inputs of the protected symbol detection automaton)

#define COMMENT_STATE 30  // state of the automaton when reading a comment
#define OP_ADD_STATE 10
#define OP_UN_STATE 23

#define N_TOKEN_CLASS 33  // number of token classes

// existing token classes
enum TOKEN_CLASS { LAMBDA,
                   N_REAL,
                   N_INTEGER,
                   OP_UN,
                   OP_ADD,
                   OP_MULT,
                   RELATION,
                   ASSIGN,
                   DECLARE_TYPE,
                   SEMICOLON,
                   COLON,
                   OPEN_PAR,
                   CLOSE_PAR,
                   DOT,
                   ID,
                   BEGIN,
                   CONST,
                   DO,
                   END,
                   ELSE,
                   IF,
                   INTEGER,
                   FOR,
                   PROGRAM,
                   PROCEDURE,
                   REAL,
                   READ,
                   THEN,
                   TO,
                   VAR,
                   WRITE,
                   WHILE,
                   ERROR };

enum MULTIPLE_EXPECTED { NUMBER = INT_MIN,
                         TYPES,
                         COMMAND,
                         EQUALS };

// defines the structures necessary for lexer operation
typedef struct {
    String buffer;
    FILE* sourceCode;
    FILE* tokenOutput;
    int tokenClass;

    int transitionMatrix[NUMBER_OF_STATES][NUMBER_OF_CHARS];  // automaton transition matrix
    bool finalState[NUMBER_OF_STATES];                        // whether a state is final or not
    char finalStateClass[NUMBER_OF_STATES];                   // associates each final state with a token class

    // protected symbol recognition automaton transition matrix
    int protectedSymbolMatrix[NUMBER_OF_STATES_PROTECTED_SYMBOLS][NUMBER_OF_LOWER_CASE_LETTERS];
    // whether a state is final or not and their token class
    char protectedSymbolFinalStates[NUMBER_OF_STATES_PROTECTED_SYMBOLS];

    char currChar;
    int fscanfFlag;
    int currState;  // current automaton state
    int currLine;   // current line on P-- source code file
    int currCol;    // current column on P-- source code file

    bool lastWasNumberOrIdent;  // indicates whether the last token was a number or identifier
} Lexer;

bool lexerInit(Lexer* lexer, const char* sourceFilePath);
void lexerDestroy(Lexer* lexer);
int nextToken(Lexer* lexer, FILE* output);  // gets next token

int lexerCurrColWithoutRetreat(Lexer* lexer);                 // return lexer col considering eventual retreats (for readability only)
const char* lexerErrorMessage(int currState);                 // return error description given current automaton state
const char* lexerTokenClassName(int token_class);             // returns token class name given token class number
const char* lexerTokenClassUserFriendlyName(int tokenClass);  // return user friendly token class name given token class number
char* lexerBuffer(Lexer* lexer);                              // return the buffer string

// auxiliary functions called on lexer initialization to build some necessary structures
void _buildTransitionMatrix(int transitionMatrix[NUMBER_OF_STATES][NUMBER_OF_CHARS]);
void _buildFinalStates(bool finalState[NUMBER_OF_STATES], char finalStateClass[NUMBER_OF_STATES]);
void _buildProtectedSymbolMatrix(int protectedSymbolMatrix[NUMBER_OF_STATES_PROTECTED_SYMBOLS][NUMBER_OF_LOWER_CASE_LETTERS]);
void _buildProtectedSymbolFinalStates(char protectedSymbolFinalState[NUMBER_OF_STATES_PROTECTED_SYMBOLS]);

// auxiliary functions used to fill the transition matrices
void _fillOther(int transitionMatrix[NUMBER_OF_STATES][NUMBER_OF_CHARS], int startState, int endState);
void _fillWord(int protectedSymbolMatrix[NUMBER_OF_STATES_PROTECTED_SYMBOLS][NUMBER_OF_LOWER_CASE_LETTERS], const char word[], int firstState, int secondState);

// auxiliary functions used during lexer operation
void _nextChar(Lexer* lexer);
void _dealWithEOF(Lexer* lexer);
void _nextState(Lexer* lexer);
void _identifyTokenClass(Lexer* lexer);
int _checkIfProtectedSymbol(Lexer* lexer);

#endif  // LEXER_H