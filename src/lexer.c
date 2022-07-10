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
 *
 * @param lexer a lexer instance
 * @return true if there was some error
 * @return false if there was no error
 */
bool lexerInit(Lexer* lexer, const char* sourceFilePath) {
    lexer->currState = 0;
    lexer->currLine = 1;
    lexer->currCol = 1;
    lexer->tokenClass = 0;
    lexer->lastWasNumberOrIdent = false;
    stringInit(&lexer->buffer);

    // open P-- source code file
    lexer->sourceCode = fopen(sourceFilePath, "r");
    if (lexer->sourceCode == NULL) {
        printf("Error: no such file\n");
        return true;
    }

    lexer->tokenOutput = fopen("tokenOutput.txt", "w");
    if (lexer->sourceCode == NULL) {
        printf("Error: couldn't create tokenOutput file\n");
        return true;
    }

    _buildTransitionMatrix(lexer->transitionMatrix);
    _buildFinalStates(lexer->finalState, lexer->finalStateClass);
    _buildProtectedSymbolMatrix(lexer->protectedSymbolMatrix);
    _buildProtectedSymbolFinalStates(lexer->protectedSymbolFinalStates);

    return false;
}

/**
 * @brief Destroy file handles and the buffer
 *
 * @param lexer A lexer instance
 */
void lexerDestroy(Lexer* lexer) {
    fclose(lexer->sourceCode);
    fclose(lexer->tokenOutput);
    stringDestroy(&lexer->buffer);
}

/**
 * @brief Gets next token from P-- source code file
 *
 * @param lexer lexer instance
 * @param output file to output errors
 */
int nextToken(Lexer* lexer, FILE* output) {
    // initial state
    lexer->currState = 0;

    // Cleaning the buffer
    stringOverwrite(&lexer->buffer, "", 0);

    while (!lexer->finalState[lexer->currState]) {  // while the automaton hasn't reached a final state

        _nextChar(lexer);  // read char from file
        if (lexer->fscanfFlag == EOF) {
            _dealWithEOF(lexer);
            return 0;
        }
        _nextState(lexer);

        // Only append to the buffer if:
        // - Is an error (to show to the user)
        // - We're a not at the initial state or at a comment (state 31) (to avoid '\n' '\t' and such)
        // and we won't retreat (to avoid showing twice)
        if (lexer->finalStateClass[lexer->currState] == -ERROR ||
            (lexer->currState != 0 && lexer->currState != COMMENT_STATE && lexer->finalStateClass[lexer->currState] >= 0)) {
            stringAppendChar(&lexer->buffer, lexer->currChar);
        }
    }
    _identifyTokenClass(lexer);

    if (lexer->tokenClass == ERROR) {
        printf("Lexer error on line %d col %d ('%s'): %s\n", lexer->currLine, lexerCurrColWithoutRetreat(lexer), lexer->buffer.str, lexerErrorMessage(lexer->currState));
        fprintf(output, "Lexer error on line %d col %d ('%s'): %s\n", lexer->currLine, lexerCurrColWithoutRetreat(lexer), lexer->buffer.str, lexerErrorMessage(lexer->currState));
        return nextToken(lexer, output) + 1;
    } else if (lexer->tokenClass == LAMBDA) {
        printf("EOF\n");
        fprintf(output, "EOF\n");
    } else {
        fprintf(lexer->tokenOutput, "%s, %s\n", lexer->buffer.str, lexerTokenClassName(lexer->tokenClass));
    }
    return 0;
}

/**
 * @brief The transitionMatrix[][] has NUMBER_OF_STATES lines (number of states) and NUMBER_OF_CHARS rows
 * (number of ASCII characters). An element 'transionMatrix[i][j]'
 * represents the new state the automaton must go next when it is in state
 * 'i' and reads the character of ASCII number 'j'.
 * If transionMatrix[i][j] == -1, we have an invalid transition.
 *
 * @param transitionMatrix the transition matrix
 */
void _buildTransitionMatrix(int transitionMatrix[NUMBER_OF_STATES][NUMBER_OF_CHARS]) {
    // invalid state by default
    for (int i = 0; i < NUMBER_OF_STATES; i++)
        for (int j = 0; j < NUMBER_OF_CHARS; j++)
            transitionMatrix[i][j] = -1;

    // IDENTIFIERS
    _fillOther(transitionMatrix, 0, 3);  // invalid char
    _fillOther(transitionMatrix, 1, 2);  // end of identifier
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
    for (int i = '0'; i <= '9'; i++) {
        transitionMatrix[0][i] = 4;  // integer part, first number
        transitionMatrix[4][i] = 4;  // integer part, following numbers
        transitionMatrix[6][i] = 8;  // first number after decimal place
        transitionMatrix[8][i] = 8;  // following number after decimal place
    }
    transitionMatrix[4]['.'] = 6;
    _fillOther(transitionMatrix, 4, 5);  // end of an int
    _fillOther(transitionMatrix, 6, 7);  // error: decimal number without following number
    _fillOther(transitionMatrix, 8, 9);  // end of a decimal number

    // OPERANDS
    transitionMatrix[0]['+'] = 10;
    transitionMatrix[0]['-'] = 10;
    transitionMatrix[0]['*'] = 11;
    transitionMatrix[0]['/'] = 11;
    transitionMatrix[0]['='] = 12;
    transitionMatrix[0][':'] = 13;
    transitionMatrix[13]['='] = 14;
    transitionMatrix[0]['<'] = 16;
    transitionMatrix[16]['='] = 18;
    transitionMatrix[16]['>'] = 18;
    transitionMatrix[0]['>'] = 20;
    transitionMatrix[20]['='] = 22;
    _fillOther(transitionMatrix, 13, 15);  // : Declare type
    _fillOther(transitionMatrix, 16, 19);  // < Relation
    _fillOther(transitionMatrix, 20, 21);  // > Relation

    // MISCELLANEOUS
    transitionMatrix[0][' '] = 0;
    transitionMatrix[0]['\t'] = 0;
    transitionMatrix[0]['\n'] = 0;
    transitionMatrix[0][';'] = 24;
    transitionMatrix[0][','] = 25;
    transitionMatrix[0]['('] = 26;
    transitionMatrix[0][')'] = 27;
    transitionMatrix[0]['.'] = 28;
    transitionMatrix[0]['{'] = 30;
    transitionMatrix[30]['}'] = 0;
    _fillOther(transitionMatrix, COMMENT_STATE, COMMENT_STATE);  // comment
}

/**
 * @brief Build a vector that identifies final states.
 *
 * @param finalState vector that identifies final states
 * @param finalStateClass vector that identifies to which token class each final state corresponds
 */
void _buildFinalStates(bool finalState[NUMBER_OF_STATES], char finalStateClass[NUMBER_OF_STATES]) {
    // list of states that aren't final
    static const char notFinals[] = {0, 1, 4, 6, 8, 13, 16, 20, 30};
    // list of final states
    static const char finals[] = {2, 3, 5, 7, 9, 10, 11, 12, 14, 15, 17, 18, 19, 21, 22, 23, 24, 25, 26, 27, 28, 29, 31};
    // list of token classes corresponding to each final state, negative values indicate that
    // we must retreat on the source code file after such final state is reacheded
    static const int stateClasses[] = {-ID, ERROR, -N_INTEGER, -ERROR, -N_REAL, OP_ADD, OP_MULT, RELATION, ASSIGN,
                                       -DECLARE_TYPE, RELATION, RELATION, -RELATION, -RELATION, RELATION, OP_UN,
                                       SEMICOLON, COLON, OPEN_PAR, CLOSE_PAR, DOT, EOF, ERROR};

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

/**
 * @brief Builds protected symbol recognizer automaton transition matrix.
 * The protectedSymbolMatrix[][] has NUMBER_OF_STATES_PROTECTED_SYMBOLS lines (number of states)
 * and NUMBER_OF_CHARS rows (number of ASCII characters). An element 'protectedSymbolMatrix[i][j]'
 * represents the new state the automaton must go next when it is in state
 * 'i' and reads the character of ASCII number 'j'.
 * If protectedSymbolMatrix[i][j] == -1, we have an invalid transition.
 *
 * @param protectedSymbolMatrix
 */
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
    _fillWord(protectedSymbolMatrix, "o", 48, 52);          // to
    _fillWord(protectedSymbolMatrix, "var", 0, 53);
    _fillWord(protectedSymbolMatrix, "write", 0, 56);
    _fillWord(protectedSymbolMatrix, "hile", 56, 61);       // while
}

/**
 * @brief Build a vector that identifies final states regarding protected symbols.
 *
 * @param protectedSymbolFinalState vector that identifies final states token classes
 */
void _buildProtectedSymbolFinalStates(char protectedSymbolFinalState[NUMBER_OF_STATES_PROTECTED_SYMBOLS]) {
    // list of final states
    static const char finals[] = {5, 10, 12, 15, 18, 20, 26, 29, 36, 42, 46, 47, 51, 52, 55, 60, 64};
    // list of token classes (protected symbols) corresponding to final states
    static const char classes[] = {BEGIN, CONST, DO, END, ELSE, IF, INTEGER, FOR, PROGRAM, PROCEDURE, REAL, READ, THEN, TO, VAR, WRITE, WHILE};

    // invalid states correspond to IDs
    for (int i = 0; i < NUMBER_OF_STATES_PROTECTED_SYMBOLS; i++)
        protectedSymbolFinalState[i] = ID;
    // mark final states
    for (unsigned long i = 0; i < sizeof finals; i++)
        protectedSymbolFinalState[finals[i]] = classes[i];
}

/**
 * @brief Auxiliary function used to fill the transition matrix. Fills in "other" transitions.
 *
 * @param transitionMatrix the transition matrix
 * @param startState the state where the 'other' transition starts
 * @param endState the state where the 'other' transition goes
 */
void _fillOther(int transitionMatrix[NUMBER_OF_STATES][NUMBER_OF_CHARS], int startState, int endState) {
    for (int i = 0; i < NUMBER_OF_CHARS; i++)
        if (transitionMatrix[startState][i] == -1)
            transitionMatrix[startState][i] = endState;
}

/**
 * @brief Auxiliary function used to fill the protected symbol transition matrix. Given a protected symbol,
 * fills in the corresponding entries in the matrix.
 *
 * @param protectedSymbolMatrix the protected symbol matrix
 * @param word protected symbol
 * @param firstState state from where recognizition starts
 * @param secondState second state on word recognition flow
 */
void _fillWord(int protectedSymbolMatrix[NUMBER_OF_STATES_PROTECTED_SYMBOLS][NUMBER_OF_LOWER_CASE_LETTERS], const char word[], int firstState, int secondState) {
    protectedSymbolMatrix[firstState][word[0] - 'a'] = secondState;

    for (unsigned long i = 1; i < strlen(word); i++, secondState++) {
        protectedSymbolMatrix[secondState][word[i] - 'a'] = secondState + 1;
    }
}

/**
 * @brief Get next char from P-- source code file. Increments current columns
 * and line appropriately.
 *
 * @param lexer a lexer instance
 */
void _nextChar(Lexer* lexer) {
    lexer->fscanfFlag = fscanf(lexer->sourceCode, "%c", &lexer->currChar);
    if (lexer->fscanfFlag != EOF) {
        lexer->currLine += (lexer->currChar == '\n');
        lexer->currCol = (lexer->currChar == '\n') ? 1 : lexer->currCol + 1 + (lexer->currChar == '\t') * 3;
    }
}

/**
 * @brief EOF is not a char, but we must treat it as such.
 * If we read EOF while on the initial state, we say the
 * token was EOF at which point the compilation stops.
 * Otherwise, if EOF is found in another state, we treat it as a random char
 *
 * @param lexer a lexer instance
 */
void _dealWithEOF(Lexer* lexer) {
    if (lexer->currState == 0) {  // EOF is recognized only from a0
        lexer->tokenClass = LAMBDA;
    } else if (lexer->currState == COMMENT_STATE) {  // EOF inside a comment, error
        lexer->currState = COMMENT_STATE + 1;
        lexer->tokenClass = lexer->finalStateClass[lexer->currState];
    } else {
        // hacky fix since we're treating EOF as just another char
        lexer->currState = lexer->transitionMatrix[lexer->currState]['@'];

        lexer->tokenClass = lexer->finalStateClass[lexer->currState];
        fseek(lexer->sourceCode, 0, SEEK_END);  // retreat

        lexer->tokenClass = abs(lexer->tokenClass);
        if (lexer->tokenClass == ID)
            lexer->tokenClass = _checkIfProtectedSymbol(lexer);
    }
}

/**
 * @brief Advances in the transition matrix given the currState and currChar
 *
 * @param lexer a lexer instance
 */
void _nextState(Lexer* lexer) {
    lexer->currState = lexer->transitionMatrix[lexer->currState][lexer->currChar];

    // by default +/- is recognized as an operation
    // but if the previous token was neither a number nor an id, it should be considered as a unary operator
    if (lexer->currState == OP_ADD_STATE && !lexer->lastWasNumberOrIdent)
        lexer->currState = OP_UN_STATE;
}

/**
 * @brief Identifies what is the last token class. Deals with negative token class
 * (retreat) and checks if it is a protected symbol. Update lastWasNumberOrIdent flag
 *
 * @param lexer a lexer instance
 */
void _identifyTokenClass(Lexer* lexer) {
    lexer->tokenClass = lexer->finalStateClass[lexer->currState];

    if (lexer->tokenClass < 0) {
        // retreat the file and the count
        fseek(lexer->sourceCode, -sizeof(char), SEEK_CUR);
        lexer->currLine -= (lexer->currChar == '\n');
        lexer->currCol -= (lexer->currChar != '\n') + (lexer->currChar == '\t') * 3;
        lexer->tokenClass = -1 * (lexer->tokenClass);
    }
    if (lexer->tokenClass == ID)
        lexer->tokenClass = _checkIfProtectedSymbol(lexer);

    // update the flag based on the tokenClass
    lexer->lastWasNumberOrIdent = (lexer->tokenClass == ID || lexer->tokenClass == N_INTEGER || lexer->tokenClass == N_REAL);
}

/**
 * @brief Checks if the last token read was a protected symbol by
 * walking through the protected symbol matrix.
 *
 * @param lexer a lexer instance
 * @return int corresponding token class (protected symbol or ID)
 */
int _checkIfProtectedSymbol(Lexer* lexer) {
    int state = 0;
    for (unsigned long i = 0; state != -1 && i < lexer->buffer.size; i++) {
        if (islower(lexer->buffer.str[i]))
            state = lexer->protectedSymbolMatrix[state][lexer->buffer.str[i] - 'a'];
        else
            return ID;
    }

    if (state == -1)
        return ID;

    return lexer->protectedSymbolFinalStates[state];
}

/**
 * @brief Returns error description given current automaton state.
 *
 * @param currState current automaton state
 * @return char* error description
 */
const char* lexerErrorMessage(int currState) {
    static const char* lexerErrorMessages[NUMBER_OF_STATES] = {"", "", "",
                                                               "Error: Invalid character", "", "", "",
                                                               "Error: did you mean to type a real number?", "", "", "", "", "",
                                                               ""
                                                               "",
                                                               "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
                                                               "Error: Unexpected end of file"};
    return lexerErrorMessages[currState];
}

/**
 * @brief Returns token string to be used as output for debugging
 *
 * @param tokenClass token class number
 * @return char* token class name
 */
const char* lexerTokenClassName(int tokenClass) {
    // indexes token class names for tokenOutput.txt
    static const char* tokenClassName[] = {"EOF", "N_REAL", "N_INTEGER", "OP_UN", "OP_ADD", "OP_MULT", "RELATION",
                                           "ASSIGN", "DECLARE_TYPE", "SEMICOLON", "COLON",
                                           "OPEN_PAR", "CLOSE_PAR", "DOT", "ID", "BEGIN", "CONST",
                                           "DO", "END", "ELSE", "IF", "INTEGER", "FOR", "PROGRAM", "PROCEDURE",
                                           "REAL", "READ", "THEN", "TO", "VAR", "WRITE", "WHILE", "ERROR"};
    return tokenClassName[tokenClass];
}

/**
 * @brief Returns token string to be used as output for the user
 *
 * @param tokenClass token class number
 * @return char* user friendly token class name
 */
const char* lexerTokenClassUserFriendlyName(int tokenClass) {
    // indexes token class names for user-friendly printing
    static const char* tokenClassName[] = {"end of file", "real number", "integer number", "+ or - (unary operator)",
                                           "+ or - (addition operator)", "* or /", "=, <>, >, <, >= or <=",
                                           ":=", ":", ";", ",", "(", ")", ".", "identifier", "begin",
                                           "const", "do", "end", "else", "if", "'integer'", "for", "program", "procedure",
                                           "'real'", "read", "then", "to", "var", "write", "while", "ERROR"};
    
    switch(tokenClass) {
        case NUMBER:
            return "number";
        case TYPES:
            return "type-specifier";
        case COMMAND:
            return "command";
        case EQUALS:
            return "=";
        default:
            return tokenClassName[tokenClass];
    }
    
}

/**
 * @brief Col count is incremented, just for showing purposes, in case there was a retreat
 *
 * @param lexer a lexer instance
 * @return int current col without considering a possible retreat
 */
int lexerCurrColWithoutRetreat(Lexer* lexer) {
    // Col count is incremented, just for showing purposes, in case there was a retreat
    return lexer->currCol + (lexer->finalStateClass[lexer->currState] < 0);
}

/**
 * @brief Returns the lexer buffer or EOF if the last token was EOF
 *
 * @param lexer a lexer instance
 * @return the lexer buffer or EOF if the last token was EOF
 */
char* lexerBuffer(Lexer* lexer) {
    return (lexer->fscanfFlag != -1) ? lexer->buffer.str : "EOF";
}