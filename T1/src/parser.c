#include <stdlib.h>
#include <stdio.h>
#include "../header/parser.h"
#include "../header/vector.h"


int compile( FILE* sourceCode ){
    
    Lexer* lexer;
    lexerInit(lexer);
    
    String* buffer;
    stringInit( buffer );
    
    int tokenClass = 0;
    int flag = 0;
    while( tokenClass != EOF ){
        nextToken( sourceCode, lexer, buffer, &tokenClass );
        
        if( tokenClass == ERROR ){
            printf( "%s", buffer );
            flag = -1;
        }
        else
            printf( "%s, %s\n", buffer->str, getTokenClassName( tokenClass ) );
    }

}

char* getTokenClassName( int tokenClass ){
    return tokenClassName[ tokenClass ];
}