/**
 * @file lexer.c
 * @brief Lexical analyser (lexer) implementation
 */
#include "../header/lexer.h"

#include <ctype.h>
#include <stdbool.h>
#include <string.h>

/**
 * @brief Builds structures needed for lexer operation.
 * @param lexer
 */
void lexerInit(Lexer* lexer) {
    lexer->currState = 0;
    lexer->currLine = 1;
    lexer->currCol = 1;
    lexer->lastWasNumberOrIdent = false;
    _buildTransitionMatrix(lexer->transitionMatrix);
    _buildFinalStates(lexer->finalState, lexer->finalStateClass);
    _buildProtectedSymbolMatrix(lexer->protectedSymbolMatrix);
    _buildProtectedSymbolFinalStates(lexer->protectedSymbolFinalStates);
}

void nextToken(Lexer* lexer, String* buffer, FILE* sourceCode, int* tokenClass) {
    // initial state
    lexer->currState = 0;

    // Cleaning the buffer
    writeToString(buffer, "", 0);

    while (!lexer->finalState[lexer->currState]) {
        _nextChar(lexer, sourceCode);

        if (lexer->fscanfFlag == EOF) {
            _dealWithEOF(lexer, buffer, sourceCode, tokenClass);
            return;
        }

        _nextState(lexer);

        // DEBUG
        if (lexer->currState == -1) {
            printf("ME AJUDA GILBERTO!!\n");
            break;
        }

        // Only append to the buffer if:
        // - Is an error (to show to the user)
        // - We're a not at the initial state (to avoid '\n' '\t' and such) and we won't retreat (to avoid showing twice)
        if (lexer->finalStateClass[lexer->currState] == -ERROR ||
            (lexer->currState != 0 && lexer->finalStateClass[lexer->currState] >= 0 && lexer->currState != 30)) {
            append(buffer, lexer->currChar);
        }
    }
    _identifyTokenClass(lexer, buffer, sourceCode, tokenClass);
}

/**
 * The transitionMatrix[][] has NUMBER_OF_STATES lines (number of states) and NUMBER_OF_CHARS rows
 * (number of ASCII characters). An element 'transionMatrix[i][j]'
 * represents the new state the automaton must go next when it is in state
 * 'i' and reads the character of ASCII number 'j'.
 * If transionMatrix[i][j] == -1, we have an invalid transition.
 **/
void _buildTransitionMatrix(int transitionMatrix[NUMBER_OF_STATES][NUMBER_OF_CHARS]) {
    // invalid state by default
    for (int i = 0; i < NUMBER_OF_STATES; i++)
        for (int j = 0; j < NUMBER_OF_CHARS; j++)
            transitionMatrix[i][j] = -1;

    // IDENTIFIERS
    _fillOther(transitionMatrix, 0, 3);
    _fillOther(transitionMatrix, 1, 2);
    transitionMatrix[0]['_'] = 1;
    transitionMatrix[1]['_'] = 1;
    for (int i = 'a'; i <= 'z'; i++) {
        transitionMatrix[0][i] = 1;
        transitionMatrix[1][i] = 1;
    }
    for (int i = 'A'; i <= 'Z'; i++) {
        transitionMatrix[0][i] = 1;
        transitionMatrix[1][i] = 1;
    }
    for (int i = '0'; i <= '9'; i++)
        transitionMatrix[1][i] = 1;

    // NUMBERS
    transitionMatrix[0]['+'] = 4;
    transitionMatrix[0]['-'] = 4;
    transitionMatrix[6]['.'] = 8;
    for (int i = '0'; i <= '9'; i++) {
        transitionMatrix[0][i] = 6;    // integer part, first number
        transitionMatrix[4][i] = 6;    // integer part, first number after signal
        transitionMatrix[6][i] = 6;    // integer part, following numbers
        transitionMatrix[8][i] = 10;   // first number after decimal place
        transitionMatrix[10][i] = 10;  // following number after decimal place
    }
    _fillOther(transitionMatrix, 4, 5);    // error signal without number
    _fillOther(transitionMatrix, 6, 7);    // end of an int
    _fillOther(transitionMatrix, 8, 9);    // end of a decimal number
    _fillOther(transitionMatrix, 10, 11);  // end of a decimal number

    // OPERANDS
    transitionMatrix[0]['+'] = 12;
    transitionMatrix[0]['-'] = 12;
    transitionMatrix[0]['*'] = 13;
    transitionMatrix[0]['/'] = 13;
    transitionMatrix[0]['='] = 14;
    transitionMatrix[0][':'] = 15;
    transitionMatrix[15]['='] = 16;
    transitionMatrix[0]['<'] = 18;
    transitionMatrix[18]['='] = 19;
    transitionMatrix[18]['>'] = 20;
    transitionMatrix[0]['>'] = 22;
    transitionMatrix[22]['='] = 24;
    _fillOther(transitionMatrix, 15, 17);
    _fillOther(transitionMatrix, 18, 21);
    _fillOther(transitionMatrix, 22, 23);

    // MISCELLANEOUS
    transitionMatrix[0][';'] = 25;
    transitionMatrix[0][','] = 26;
    transitionMatrix[0][')'] = 27;
    transitionMatrix[0]['('] = 28;
    transitionMatrix[0]['{'] = 30;
    transitionMatrix[0][' '] = 0;
    transitionMatrix[0]['\t'] = 0;
    transitionMatrix[0]['\n'] = 0;
    transitionMatrix[30]['}'] = 0;
    _fillOther(transitionMatrix, 30, 30);
}

/**
 * @brief Build a vector that identifies final states.
 */
void _buildFinalStates(bool finalState[NUMBER_OF_STATES], char finalStateClass[NUMBER_OF_STATES]) {
    static const char notFinals[] = {0, 1, 4, 6, 8, 10, 15, 18, 22, 30};
    static const char finals[] = {2, 3, 5, 7, 9, 11, 12, 13, 14, 16, 17, 19, 20, 21, 23, 24, 25, 26, 27, 28, 29, 31};
    static const int stateClasses[] = {-ID, ERROR, -ERROR, -N_INTEGER, -ERROR, -N_REAL, OP_ADD, OP_MULT, RELATION, ASSIGN,
                                       -DECLARE_TYPE, RELATION, RELATION, -RELATION, -RELATION, RELATION, SEMICOLON, COLON,
                                       CLOSE_PAR, OPEN_PAR, EOF, ERROR};

    // Mark not final states as ERROR by default
    for (unsigned long i = 0; i < sizeof notFinals; i++) {
        finalState[notFinals[i]] = 0;
        finalStateClass[notFinals[i]] = ERROR;
    }

    // mark final states appropriately
    for (unsigned long i = 0; i < sizeof finals; i++) {
        finalState[finals[i]] = 1;
        finalStateClass[finals[i]] = stateClasses[i];
    }
}

void _buildProtectedSymbolMatrix(int protectedSymbolMatrix[NUMBER_OF_STATES_PROTECTED_SYMBOLS][NUMBER_OF_LOWER_CASE_LETTERS]) {
    // invalid state by default
    for (int i = 0; i < NUMBER_OF_STATES_PROTECTED_SYMBOLS; i++)
        for (int j = 0; j < NUMBER_OF_LOWER_CASE_LETTERS; j++)
            protectedSymbolMatrix[i][j] = -1;

    _fillWord(protectedSymbolMatrix, "begin", 0, 1);
    _fillWord(protectedSymbolMatrix, "const", 0, 6);
    _fillWord(protectedSymbolMatrix, "do", 0, 11);
    _fillWord(protectedSymbolMatrix, "end", 0, 13);
    _fillWord(protectedSymbolMatrix, "lse", 13, 16);        // else
    _fillWord(protectedSymbolMatrix, "if", 0, 19);
    _fillWord(protectedSymbolMatrix, "nteger", 19, 21);     // integer
    _fillWord(protectedSymbolMatrix, "for", 0, 27);
    _fillWord(protectedSymbolMatrix, "program", 0, 30);
    _fillWord(protectedSymbolMatrix, "cedure", 32, 37);     // procedure
    _fillWord(protectedSymbolMatrix, "real", 0, 43);
    _fillWord(protectedSymbolMatrix, "d", 45, 47);          // read
    _fillWord(protectedSymbolMatrix, "then", 0, 48);
    _fillWord(protectedSymbolMatrix, "var", 0, 52);
    _fillWord(protectedSymbolMatrix, "write", 0, 55);
    _fillWord(protectedSymbolMatrix, "hile", 55, 60);       // while
}

void _buildProtectedSymbolFinalStates(char protectedSymbolFinalState[NUMBER_OF_STATES_PROTECTED_SYMBOLS]) {
    static const char finals[] = {5, 10, 12, 15, 18, 20, 26, 29, 36, 42, 46, 47, 51, 54, 59, 63};
    static const char classes[] = {BEGIN, CONST, DO, END, ELSE, IF, INTEGER, FOR, PROGRAM, PROCEDURE, REAL, READ, THEN, VAR, WRITE, WHILE};

    for (int i = 0; i < NUMBER_OF_STATES_PROTECTED_SYMBOLS; i++)
        protectedSymbolFinalState[i] = ID;
    for (unsigned long i = 0; i < sizeof finals; i++)
        protectedSymbolFinalState[finals[i]] = classes[i];
}

void _fillOther(int transitionMatrix[NUMBER_OF_STATES][NUMBER_OF_CHARS], int startState, int endState) {
    for (int i = 0; i < NUMBER_OF_CHARS; i++)
        if (transitionMatrix[startState][i] == -1)
            transitionMatrix[startState][i] = endState;
}

void _fillWord(int protectedSymbolMatrix[NUMBER_OF_STATES_PROTECTED_SYMBOLS][NUMBER_OF_LOWER_CASE_LETTERS], const char word[], int firstState, int secondState) {
    protectedSymbolMatrix[firstState][word[0] - 'a'] = secondState;

    for (unsigned long i = 1; i < strlen(word); i++, secondState++) {
        protectedSymbolMatrix[secondState][word[i] - 'a'] = secondState + 1;
    }
}

void _nextChar(Lexer* lexer, FILE* sourceCode) {
    lexer->fscanfFlag = fscanf(sourceCode, "%c", &lexer->currChar);
    lexer->currLine += (lexer->currChar == '\n');
    lexer->currCol = (lexer->currChar == '\n') ? 1 : lexer->currCol + 1;
}

/**
 * @brief EOF is not a char, but we must treat it as such.
 * If we read EOF while on the initial state, we say the
 * token was EOF at which point the compilation stops.
 * Otherwise, if EOF is found in another state, we treat it as a random char
 */
void _dealWithEOF(Lexer* lexer, String* buffer, FILE* sourceCode, int* tokenClass) {
    if (lexer->currState == 0) {
        *tokenClass = EOF;  // EOF is recognized only from a0
    }
    else if(lexer->currState == 30) {
        lexer->currState = 31;
        *tokenClass = ERROR;
    }
    else {
        // hacky fix since we're treating EOF as just another char
        lexer->currState = lexer->transitionMatrix[lexer->currState]['@'];

        *tokenClass = lexer->finalStateClass[lexer->currState];
        fseek(sourceCode, 0, SEEK_END);  // retreat

        *tokenClass = abs(*tokenClass);
        if ((*tokenClass) == ID)
            *tokenClass = _checkIfProtectedSymbol(lexer, buffer);
    }
}

void _nextState(Lexer* lexer) {
    lexer->currState = lexer->transitionMatrix[lexer->currState][lexer->currChar];

    // by default +/- is recognized as an operation
    // but if the previous token was a number or id, it should be considered as a sign
    if (lexer->currState == 12 && !lexer->lastWasNumberOrIdent)
        lexer->currState = 4;
}

void _identifyTokenClass(Lexer* lexer, String* buffer, FILE* sourceCode, int* tokenClass) {
    *tokenClass = lexer->finalStateClass[lexer->currState];

    if ((*tokenClass) < 0) {
        fseek(sourceCode, -sizeof(char), SEEK_CUR);  // retreat
        *tokenClass = -1 * (*tokenClass);
    }
    if ((*tokenClass) == ID)
        *tokenClass = _checkIfProtectedSymbol(lexer, buffer);

    // update the flag based on the tokenClass
    lexer->lastWasNumberOrIdent = (*tokenClass == ID || *tokenClass == N_INTEGER || *tokenClass == N_REAL);
}

int _checkIfProtectedSymbol(Lexer* lexer, String* buffer) {
    int state = 0;
    for (int i = 0; state != -1 && i < buffer->size; i++) {
        if (islower(buffer->str[i]))
            state = lexer->protectedSymbolMatrix[state][buffer->str[i] - 'a'];
        else
            return ID;
    }

    if (state == -1)
        return ID;

    return lexer->protectedSymbolFinalStates[state];
}