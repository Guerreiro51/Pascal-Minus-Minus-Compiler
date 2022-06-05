#include <stdio.h>
#include <stdlib.h>

#include "../header/parser.h"

/**
 * @brief P-- compiler
 *
 * @param argc number of command line arguments (expects 2 arguments)
 * @param argv commmand line arguments ( expects {executable name, source code file name} )
 * @return int
 */
int main(int argc, char** argv) {
    // wrong number of command line arguments error
    if (argc != 2) {
        printf("Error: no input files\n");
        return -1;
    }

    // open P-- source code file
    FILE* sourceCode = fopen(argv[1], "r");
    if (sourceCode == NULL) {
        printf("Error: no such file\n");
        return -1;
    }

    // calls parser (syntax analyzer)
    CompileRet compileRet = compile(sourceCode);
    fclose(sourceCode);

    // print compiler status
    if( compileRet.errorCount > 0 )
        printf("Program compiled with error\n");
    else  if( compileRet.errorCount == 0 )
        printf("Program compiled successfully\n");

    return 0;
}