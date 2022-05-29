#include <stdlib.h>
#include <stdio.h>
#include "../header/parser.h"

/**
 * @brief 
 * 
 * @param argc number of command line arguments (expects 2 arguments)
 * @param argv commmand line arguments ( expects {executable name, source code file name} )
 * @return int 
 */
int main(int argc, char** argv) {

    if( argc != 2 ) {
        printf("Error: no input files\n");
        return -1;
    }
    
    FILE* sourceCode = fopen( argv[1], "r" );
    if( sourceCode == NULL ){
        printf("Error: no such file\n");  
        return -1;
    }

    // calls parser (syntax analyzer)
    int flag = compile( sourceCode );
    fclose( sourceCode );

    return flag;
}