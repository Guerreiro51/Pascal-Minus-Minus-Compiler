/**
 * @file lexer.c
 * @brief Lexical analyser (lexer) implementation
 */
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "../header/lexer.h"

/************************************** LEXER INITIALIZATION **************************************/

/**
 * @brief Builds structures needed for lexer operation.
 * @param lexer 
 */
void lexerInit(Lexer* lexer) {

    buildFinalStates(lexer->finalState, lexer->finalStateClass);
    buildTransitionMatrix(lexer->transitionMatrix);
    buildProtectedSymbolMatrix(lexer->protectedSymbolMatrix);
    buildProtectedSymbolFinalStates(lexer->protectedSymbolFinalStates);
    lexer->currState = 0;
    lexer->currLine = 1;
    lexer->currCol = 1;
    lexer->lastWasNumberOrIdent = false;
}

void fillWord(int protectedSymbolMatrix[NUMBER_OF_STATES_PROTECTED_SYMBOLS][NUMBER_OF_LOWER_CASE_LETTERS], const char word[], int firstState, int secondState) {
    protectedSymbolMatrix[firstState][word[0] - 'a'] = secondState;

    for(int i = 1; i < strlen(word); i++, secondState++) {
        protectedSymbolMatrix[secondState][word[i] - 'a'] = secondState + 1;
    }
}

void buildProtectedSymbolMatrix( int protectedSymbolMatrix[NUMBER_OF_STATES_PROTECTED_SYMBOLS][NUMBER_OF_LOWER_CASE_LETTERS] ){
    
    // invalid state by default
    for(int i = 0; i < NUMBER_OF_STATES_PROTECTED_SYMBOLS; i++)
        for(int j = 0; j < NUMBER_OF_LOWER_CASE_LETTERS; j++)
            protectedSymbolMatrix[i][j] = -1;
    
    fillWord(protectedSymbolMatrix, "begin", 0, 1);  
    fillWord(protectedSymbolMatrix, "const", 0, 6);
    fillWord(protectedSymbolMatrix, "do", 0, 11);
    fillWord(protectedSymbolMatrix, "end", 0, 13);
    fillWord(protectedSymbolMatrix, "lse", 13, 16);      // else
    fillWord(protectedSymbolMatrix, "if", 0, 19);
    fillWord(protectedSymbolMatrix, "nteger", 19, 21);   // integer
    fillWord(protectedSymbolMatrix, "for", 0, 27);
    fillWord(protectedSymbolMatrix, "program", 0, 30);
    fillWord(protectedSymbolMatrix, "cedure", 32, 37);   // procedure
    fillWord(protectedSymbolMatrix, "real", 0, 43);
    fillWord(protectedSymbolMatrix, "d", 45, 47);        // read
    fillWord(protectedSymbolMatrix, "then", 0, 48);
    fillWord(protectedSymbolMatrix, "var", 0, 52);
    fillWord(protectedSymbolMatrix, "write", 0, 55);
    fillWord(protectedSymbolMatrix, "hile", 55, 60);     // while 
}

void fillOther(int transitionMatrix[NUMBER_OF_STATES][NUMBER_OF_CHARS], int startState, int endState) {
    for(int i = 0; i < NUMBER_OF_CHARS; i++)
        if(transitionMatrix[startState][i] == -1)
            transitionMatrix[startState][i] = endState;
}

/**
 * @brief Build a vector that identifies final states.
 * 
 * @param isFinalState 
 */
void buildFinalStates( bool finalState[NUMBER_OF_STATES], char finalStateClass[NUMBER_OF_STATES] ) {
    static const char notFinals[] = {0, 1, 4, 6, 8, 10, 15, 18, 22, 30};
    // Define everything as final
    for(int i = 0; i < NUMBER_OF_STATES; i++){
        finalState[i] = 1;
        finalStateClass[i] = ERROR;
    }

    // Mark not final states
    for(int i = 0; i < sizeof notFinals; i++)
        finalState[notFinals[i]] = 0;
    
    finalStateClass[2] = -ID;
    finalStateClass[3] = ERROR;
    finalStateClass[5] = -ERROR;
    finalStateClass[7] = -N_INTEGER;
    finalStateClass[9] = -ERROR;
    finalStateClass[11] = -N_REAL;
    finalStateClass[12] = OP_ADD;
    finalStateClass[13] = OP_MULT;
    finalStateClass[14] = RELATION;
    finalStateClass[16] = ASSIGN;
    finalStateClass[17] = -DECLARE_TYPE;
    finalStateClass[19] = RELATION;
    finalStateClass[20] = RELATION;
    finalStateClass[21] = -RELATION;
    finalStateClass[23] = -RELATION;
    finalStateClass[24] = RELATION;
    finalStateClass[25] = SEMICOLON;
    finalStateClass[26] = COLON;
    finalStateClass[27] = CLOSE_PAR;
    finalStateClass[28] = OPEN_PAR;
    finalStateClass[29] = EOF;
    finalStateClass[31] = ERROR;
    
}

void buildProtectedSymbolFinalStates( char protectedSymbolFinalState[NUMBER_OF_STATES_PROTECTED_SYMBOLS] ){
    
    static const char finals[] = {5, 10, 12, 15, 18, 20, 26, 29, 36, 42, 46, 47, 51, 54, 59, 63};
    static const char classes[] = {BEGIN, CONST, DO, END, ELSE, IF, INTEGER, FOR, PROGRAM, PROCEDURE, REAL, READ, THEN, VAR, WRITE, WHILE};
    
    for(int i = 0; i < NUMBER_OF_STATES_PROTECTED_SYMBOLS; i++)
        protectedSymbolFinalState[i] = ID;
    for(int i = 0; i < sizeof finals; i++)
        protectedSymbolFinalState[finals[i]] = classes[i];

}

/**
 * The transitionMatrix[][] has NUMBER_OF_STATES lines (number of states) and NUMBER_OF_CHARS rows
 * (number of ASCII characters). An element 'transionMatrix[i][j]'
 * represents the new state the automaton must go next when it is in state
 * 'i' and reads the character of ASCII number 'j'. 
 * If transionMatrix[i][j] == -1, we have an invalid transition.
**/
void buildTransitionMatrix( int transitionMatrix[NUMBER_OF_STATES][NUMBER_OF_CHARS] ) {
    // invalid state by default
    for(int i = 0; i < NUMBER_OF_STATES; i++)
        for(int j = 0; j < NUMBER_OF_CHARS; j++)
            transitionMatrix[i][j] = -1;
    
    // IDENTIFIERS 
    fillOther(transitionMatrix, 0, 3);
    fillOther(transitionMatrix, 1, 2);
    transitionMatrix[0]['_'] = 1;
    transitionMatrix[1]['_'] = 1;
    for(int i = 'a'; i <= 'z'; i++) {
        transitionMatrix[0][i] = 1;
        transitionMatrix[1][i] = 1;
    }
    for(int i = 'A'; i <= 'Z'; i++) {
        transitionMatrix[0][i] = 1;
        transitionMatrix[1][i] = 1;
    }
    for(int i = '0'; i <= '9'; i++)
        transitionMatrix[1][i] = 1;
    
    // NUMBERS
    transitionMatrix[0]['+'] = 4; // Vamo ta vendo de ver o last
    transitionMatrix[0]['-'] = 4; // Vamo ta vendo de ver o last
    transitionMatrix[6]['.'] = 8;
    for(int i = '0'; i <= '9'; i++) {
        transitionMatrix[0][i] = 6;     // integer part, first number
        transitionMatrix[4][i] = 6;     // integer part, first number after signal
        transitionMatrix[6][i] = 6;     // integer part, following numbers
        transitionMatrix[8][i] = 10;    // first number after decimal place
        transitionMatrix[10][i] = 10;   // following number after decimal place
    }
    fillOther(transitionMatrix, 4, 5);  // error signal without number
    fillOther(transitionMatrix, 6, 7);  // end of an int
    fillOther(transitionMatrix, 8, 9);  // end of a decimal number
    fillOther(transitionMatrix, 10, 11);  // end of a decimal number

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
    fillOther( transitionMatrix, 15, 17 );
    fillOther( transitionMatrix, 18, 21 ); 
    fillOther( transitionMatrix, 22, 23 );

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
    
    // Vendo de ver EOF  
}

/***************************************** LEXER OPERATION ****************************************/

void nextToken(FILE* sourceCode, Lexer* lexer, String* buffer, int* tokenClass) {
    lexer->currState = 0;
    
    // Cleaning the buffer
    buffer->size = 0;
    buffer->str[0] = '\0';

    while( !lexer->finalState[lexer->currState] ) {
        // Read character
        char c;
        
        // EOF
        int fscanfFlag = fscanf(sourceCode, "%c", &c);
        lexer->currLine += (c == '\n');
        lexer->currCol = (c == '\n') ? 1 : lexer->currCol + 1;
        if( fscanfFlag == EOF && lexer->currState == 0 ) {
            *tokenClass = EOF;   // EOF is recognized only from a0
            return;
        }
        else if( fscanfFlag == EOF ) {
            lexer->currState = lexer->transitionMatrix[lexer->currState]['@']; // hacky fix since EOF is just another char
            identifyTokenClass( sourceCode, lexer, buffer, tokenClass, true );
            return;
            // *tokenClass = ERROR;
            // write( buffer, "" );
            // return;  // recognize current token and retreat
        }

        // Look for next state
        lexer->currState = lexer->transitionMatrix[lexer->currState][c];

        
        // by default +/- is recognized as an operation
        // but if the previous token was a number or id, it should be considered as a sign
        if(lexer->currState == 12 && !lexer->lastWasNumberOrIdent)
            lexer->currState = 4;
        
       
        // DEBUG
        if(lexer->currState == -1) {
            printf("ME AJUDA GILBERTO!!\n");
            break;
        }

        // Not have to retreat and different from zero
        if((lexer->finalStateClass[lexer->currState] > 0 && lexer->currState) || lexer->finalStateClass[lexer->currState] == -ERROR) {
            // Append to buffer
            append( buffer, c );
        }
    }

    identifyTokenClass( sourceCode, lexer, buffer, tokenClass, false );
    lexer->lastWasNumberOrIdent = (*tokenClass == ID || *tokenClass == N_INTEGER || *tokenClass == N_REAL);  
}

void identifyTokenClass( FILE* sourceCode, Lexer* lexer, String* buffer, int* tokenClass, bool isEOF ){

    *tokenClass = lexer->finalStateClass[ lexer->currState ];
    
    if( (*tokenClass) < 0  ){

        if( isEOF )
            fseek( sourceCode, 0, SEEK_END ); // retreat
        else
            fseek( sourceCode, -sizeof(char), SEEK_CUR ); // retreat

        *tokenClass = -1*(*tokenClass);
    }
    if( (*tokenClass) == ID )
        *tokenClass = lookUpProtectedSymbol( buffer, lexer );

}

int lookUpProtectedSymbol( String* buffer, Lexer* lexer ) {
    int state = 0;
    for(int i = 0; state != -1 && i < buffer->size; i++) {
        if(islower(buffer->str[i]))
            state = lexer->protectedSymbolMatrix[state][buffer->str[i] -'a'];
        else
            return ID;
    }
     
    if( state == -1 )
        return ID;
        
    return lexer->protectedSymbolFinalStates[state];
}