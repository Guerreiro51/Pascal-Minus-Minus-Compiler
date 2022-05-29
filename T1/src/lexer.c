#include <stdbool.h>
#include <string.h>
#include "lexer.h"

void lexerInit(Lexer* lexer) {
    buildFinalStates(lexer->finalState);
    buildTransitionMatrix(lexer->transitionMatrix);
    buildProtectedSymbolMatrix(lexer->protectedSymbolMatrix);
    lexer->curState = 0;
    lexer->lastWasNumberOrIdent = false;
}

void fillWord(char protectedSymbolMatrix[NUMBER_OF_STATES_PROTECTED_SYMBOLS][NUMBER_OF_LOWER_CASE_LETTERS], const char word[], int firstState, int secondState){
    
    protectedSymbolMatrix[firstState][word[0]];

    for(int i = 1; i < strlen(word); i++) {
        protectedSymbolMatrix[secondState++][word[i]];
    }
}


void buildProtectedSymbolMatrix( char protectedSymbolMatrix[NUMBER_OF_STATES_PROTECTED_SYMBOLS][NUMBER_OF_LOWER_CASE_LETTERS] ){
    
    // invalid state by default
    for(char i = 0; i < NUMBER_OF_STATES_PROTECTED_SYMBOLS; i++)
        for(char j = 0; j < NUMBER_OF_LOWER_CASE_LETTERS; j++)
            protectedSymbolMatrix[i][j] = -1;

    

    // begin     
    protectedSymbolMatrix[0]['b'] = 1;
    protectedSymbolMatrix[1]['e'] = 2;
    protectedSymbolMatrix[2]['g'] = 3;
    protectedSymbolMatrix[3]['i'] = 4;
    protectedSymbolMatrix[4]['n'] = 5; 
    
    // const
    protectedSymbolMatrix[0]['b'] = 1;
    protectedSymbolMatrix[1]['e'] = 2;
    protectedSymbolMatrix[2]['g'] = 3;
    protectedSymbolMatrix[3]['i'] = 4;
    protectedSymbolMatrix[4]['n'] = 5; 
    
    
    // write
    protectedSymbolMatrix[0]['w'] = 54;    
    protectedSymbolMatrix[53]['r'] = 54;    
    protectedSymbolMatrix[54]['i'] = 55;    
    protectedSymbolMatrix[55]['t'] = 56;    
    protectedSymbolMatrix[56]['e'] = 57;    
    
    // while
    protectedSymbolMatrix[0]['w'] = 54;    
    protectedSymbolMatrix[53]['h'] = 58;    
    protectedSymbolMatrix[58]['i'] = 59;    
    protectedSymbolMatrix[59]['l'] = 60;    
    protectedSymbolMatrix[60]['e'] = 61;    
}

void fillOther(char transitionMatrix[NUMBER_OF_STATES][NUMBER_OF_CHARS], int startState, int endState) {
    for(char i = 0; i < NUMBER_OF_CHARS; i++)
        if(transitionMatrix[startState][i] == -1)
            transitionMatrix[startState][i] = endState;
}

/**
 * @brief Build a vector that identifies final states.
 * 
 * @param isFinalState 
 */
void buildFinalStates( int isFinalState[NUMBER_OF_STATES] ) {
    const char notFinals[] = {0, 1, 4, 6, 8, 10, 15, 18, 22, 30};
    // Define everything as final
    for(int i = 0; i < NUMBER_OF_STATES; i++) 
        isFinalState[i] = 1;

    // Mark not final states
    for(int i = 0; i < sizeof notFinals; i++)
        isFinalState[notFinals[i]] = 0;
}

/**
 * The transitionMatrix[][] has NUMBER_OF_STATES lines (number of states) and NUMBER_OF_CHARS rows
 * (number of ASCII characters). An element 'transionMatrix[i][j]'
 * represents the new state the automaton must go next when it is in state
 * 'i' and reads the character of ASCII number 'j'. 
 * If transionMatrix[i][j] == -1, we have an invalid transition.
**/
void buildTransitionMatrix( char transitionMatrix[NUMBER_OF_STATES][NUMBER_OF_CHARS] ) {
    // invalid state by default
    for(char i = 0; i < NUMBER_OF_STATES; i++)
        for(char j = 0; j < NUMBER_OF_CHARS; j++)
            transitionMatrix[i][j] = -1;
    
    // IDENTIFIERS 
    fillOther(transitionMatrix, 0, 3);
    transitionMatrix[0]['_'] = 1;
    transitionMatrix[1]['_'] = 1;
    for(char i = 'a'; i <= 'z'; i++) {
        transitionMatrix[0][i] = 1;
        transitionMatrix[1][i] = 1;
    }
    for(char i = 'A'; i <= 'Z'; i++) {
        transitionMatrix[0][i] = 1;
        transitionMatrix[1][i] = 1;
    }
    for(char i = '0'; i <= '9'; i++)
        transitionMatrix[1][i] = 1;
    
    // NUMBERS
    transitionMatrix[0]['+'] = 4; // Vamo ta vendo de ver o last
    transitionMatrix[0]['-'] = 4; // Vamo ta vendo de ver o last
    transitionMatrix[6]['.'] = 8;
    for(char i = '0'; i <= '9'; i++) {
        transitionMatrix[0][i] = 6;     // integer part, first number
        transitionMatrix[4][i] = 6;     // integer part, first number after signal
        transitionMatrix[6][i] = 6;     // integer part, following numbers
        transitionMatrix[8][i] = 10;    // first number after decimal place
        transitionMatrix[10][i] = 10;   // following number after decimal place
    }
    fillOther(transitionMatrix, 4, 5);  // error signal without number
    fillOther(transitionMatrix, 6, 7);  // end of an int
    fillOther(transitionMatrix, 8, 9);  // end of a decimal number

    // OPERANDS
    transitionMatrix[0]['+'] = 12;
    transitionMatrix[0]['-'] = 12;
    transitionMatrix[0]['*'] = 13;
    transitionMatrix[0]['/'] = 13;
    transitionMatrix[0]['='] = 14;
    transitionMatrix[0][':'] = 15;
    transitionMatrix[15]['='] = 16;
    fillOther( transitionMatrix, 15, 17 );
    transitionMatrix[0]['<'] = 18;
    transitionMatrix[18]['='] = 19;
    transitionMatrix[18]['>'] = 20;
    fillOther( transitionMatrix, 18, 21 ); 
    transitionMatrix[0]['>'] = 22;
    transitionMatrix[22]['='] = 24;
    fillOther( transitionMatrix, 22, 23 ); 

    // MISCELLANEOUS
    transitionMatrix[0][';'] = 25;
    transitionMatrix[0][','] = 26;
    transitionMatrix[0][')'] = 27;
    transitionMatrix[0]['('] = 28;
    transitionMatrix[0]['{'] = 30;
    transitionMatrix[30]['}'] = 32;
    
    // Vendo de ver EOF  
}



void nextToken(FILE* sourceCode, Lexer* lexer, String* buffer, int* tokenClass) {
    lexer->curState = 0;
    
    while( !lexer->finalState[lexer->curState] ) {
        // Read character
        char c;
        
        // EOF
        if( fscanf(sourceCode, "%c", &c) == EOF &&  lexer->curState == 0 ) {
            tokenClass = EOF;   // EOF is recognized only from a0
            break;
        }
        else
            break;  // recognize current token and retreat
 
        // Look for next state
        lexer->curState = lexer->transitionMatrix[lexer->curState][c];

        // by default +/- is recognized as an operation
        if( !lexer->lastWasNumberOrIdent )  // if last token wasn't a number or an ID...
            lexer->curState = 4;            //... recognize as signed number

        // DEBUG
        if(lexer->curState == -1) {
            printf("ME AJUDA GILBERTO!!!!!!!!!!!\n");
            break;
        }

        // Append to buffer
        append( buffer, c );
    }


    switch(lexer->curState) {
        case 2:
            // VERIFICA PALAVRAS RESERVADAS
            tokenClass = lookUpProtectedSymbol( buffer );
            fseek( sourceCode, - sizeof(char), SEEK_CUR );
            break;
        case 3:
            tokenClass = ERROR;
            break;
        case 5:
            tokenClass = ERROR;
            break;
        case 7:
            tokenClass = N_INTEGER;
            break;
        case 9:
            tokenClass = ERROR;
            break;
        case 11:
            tokenClass = N_REAL;
            break;
        case 12:
            if(lexer->lastWasNumberOrIdent)
                tokenClass = OP_ADD;
            break;
        case 13:
            tokenClass = OP_MULT;
            break;
        case 14:
            tokenClass = RELATION;
            break;
        case 16:
            tokenClass = ASSIGN;
            break;
        case 17:
            tokenClass = DECLARE_TYPE;
            break;
        case 19:
            tokenClass = RELATION;
            break;
        case 20:
            tokenClass = RELATION;
            break;
        case 21:
            tokenClass = RELATION;
            break;
        case 23:
            tokenClass = RELATION;
            break;
        case 24:
            tokenClass = RELATION;
            break;
        case 25:
            tokenClass = SEMICOLON;
            break;
        case 26:
            tokenClass = COLON;
            break;
        case 27:
            tokenClass = CLOSE_PAR;
            break;
        case 28:
            tokenClass = OPEN_PAR;
            break;
    }    

    // Retreat
    fseek( sourceCode, - sizeof(char), SEEK_CUR );
}

int lookUpProtectedSymbol( String* buffer ){

    int tokenClass;

    
    
    return tokenClass;
}