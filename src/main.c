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

    Parser parser;
    if (parserInit(&parser, argv[1])) {
        return -1;
    }
    compile(&parser);

    // print compiler status
    if (parser.errorCount > 0)
        printf("Program compiled with %d errors\n", parser.errorCount);
    else if (parser.errorCount == 0)
        printf("Program compiled successfully\n");

    parserDestroy(&parser);
    return 0;
}