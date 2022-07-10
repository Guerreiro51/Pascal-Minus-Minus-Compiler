/**
 * @file parser.c
 * @brief Syntax analyser (parser) implementation
 *
 */
#include "../header/parser.h"

#include <stdio.h>
#include <stdlib.h>

#include "../header/string.h"

/**
 * @brief Default error function call.
 *
 * @param expectedTokenClass expected token
 * @param ... synchronization tokens that must be added
 */
#define FOLLOWERS(expectedTokenClass, ...) {                                   \
    static const int followers[] = {__VA_ARGS__};                              \
    _sincTokensAdd(sincTokens, followers, sizeof(followers) / sizeof(int));    \
    _error(parser, expectedTokenClass, sincTokens);                            \
    int level = sincTokens[parser->lexer.tokenClass];                          \
    _sincTokensRemove(sincTokens, followers, sizeof(followers) / sizeof(int)); \
    if (level != 0) {                                                          \
        _sincTokensDecr(sincTokens);                                           \
        return;                                                                \
    }                                                                          \
}

/**
 * @brief Default treatment of next rule call.
 *
 * @param _rule next rule function name
 * @param ... followers of the corresponding variable
 */
#define NEXTRULE(rule, ...) {                                                  \
    static const int followers[] = {__VA_ARGS__};                              \
    _sincTokensAdd(sincTokens, followers, sizeof(followers) / sizeof(int));    \
    rule(parser, sincTokens);                                                  \
    int return_flag = 0;                                                       \
    if (sincTokens[parser->lexer.tokenClass] > 0) {                            \
        return_flag = 1;                                                       \
    }                                                                          \
    _sincTokensRemove(sincTokens, followers, sizeof(followers) / sizeof(int)); \
    if (return_flag){                                                          \
        _sincTokensDecr(sincTokens);                                           \
        return;                                                                \
    }                                                                          \
}

/**
 * @brief Initializes parser variables
 *
 * @param parser a parser instance
 * @param sourceCodePath a source code path to be compiled
 * @return true if there was some error
 * @return false if there was no error
 */
bool parserInit(Parser* parser, const char* sourceCodePath) {
    parser->errorCount = 0;

    if (lexerInit(&parser->lexer, sourceCodePath)) {
        return true;
    }

    // open output file
    parser->output = fopen("output.txt", "w");
    if (parser->output == NULL) {
        printf("Error: couldn't create output file\n");
        return true;
    }

    return false;
}

/**
 * @brief Destroy file handles and the lexer
 *
 * @param parser initialized parser instance
 */
void parserDestroy(Parser* parser) {
    fclose(parser->output);
    lexerDestroy(&parser->lexer);
}

/**
 * @brief Initialize vector of synchronization tokens, -1 indicates
 * the token isn't a synchronization token in the current context.
 * Values greater then or equal to 0 indicate the depth of the synchronization
 * token.
 *
 * @param sincTokens sybchronization token vector
 */
void _sincTokensInit(int sincTokens[]) {
    for (int i = 0; i < N_TOKEN_CLASS; i++) {
        sincTokens[i] = -1;
    }
}

/**
 * @brief Make a copy of the synchronization token vector
 *
 * @param sincTokens original synchronization token vector
 * @param copySincTokens copy
 */
void _sincTokensCopy(int sincTokens[], int copySincTokens[]) {
    for (int i = 0; i < N_TOKEN_CLASS; i++) {
        copySincTokens[i] += sincTokens[i];
    }
}

/**
 * @brief Increment depth of synchronization tokens.
 *
 * @param sincTokens sybchronization token vector
 */
void _sincTokensIncr(int sincTokens[]) {
    for (int i = 0; i < N_TOKEN_CLASS; i++) {
        sincTokens[i] += (sincTokens[i] >= 0);
    }
}

/**
 * @brief Decrement depth of synchronization tokens.
 *
 * @param sincTokens sybchronization token vector
 */
void _sincTokensDecr(int sincTokens[]) {
    for (int i = 0; i < N_TOKEN_CLASS; i++) {
        sincTokens[i] -= (sincTokens[i] >= 0);
    }
}

/**
 * @brief Add synchronization tokens.
 *
 * @param sincTokens synchronization token vector
 */
void _sincTokensAdd(int sincTokens[], const int toAdd[], unsigned long toAddSize) {
    for (unsigned long i = 0; i < toAddSize; i++) {
        sincTokens[toAdd[i]] = 0;
    }
}

/**
 * @brief Remove synchronization tokens.
 *
 * @param sincTokens synchronization token vector
 */
void _sincTokensRemove(int sincTokens[], const int toRemove[], unsigned long toRemoveSize) {
    for (unsigned long i = 0; i < toRemoveSize; i++) {
        sincTokens[toRemove[i]] = -1;
    }
}

/**
 * @brief Controls the compilation process.
 *
 * @param parser initialized parser instance
 */
void compile(Parser* parser) {
    // get first token
    parser->errorCount += nextToken(&parser->lexer, parser->output);

    // initialize synchronization tokens vector
    int sincTokens[N_TOKEN_CLASS];
    _sincTokensInit(sincTokens);

    // starts building the implicit parse tree
    const int followers[] = {LAMBDA};
    _sincTokensAdd(sincTokens, followers, sizeof(followers) / sizeof(int));
    _programa(parser, sincTokens);

    // check if source code ended
    if (parser->lexer.fscanfFlag != EOF) {
        _error(parser, LAMBDA, sincTokens);
    }
}

/**
 * @brief Implements rule 1 of the grammar:
 * <programa> ::= program ident ; <corpo> .
 * @param parser initialized parser instance
 */
void _programa(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);

    if (parser->lexer.tokenClass == PROGRAM) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(PROGRAM, ID)
    }
    if (parser->lexer.tokenClass == ID) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(ID, SEMICOLON)
    }
    if (parser->lexer.tokenClass == SEMICOLON) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(SEMICOLON, CONST, VAR, PROCEDURE, BEGIN)
    }

    NEXTRULE(_corpo, DOT)

    if (parser->lexer.tokenClass == DOT) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(DOT, LAMBDA)
    }

    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 2 of the grammar:
 * <corpo> ::= <dc> begin <comandos> end
 * @param parser initialized parser instance
 */
void _corpo(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    NEXTRULE(_dc, BEGIN)
    if (parser->lexer.tokenClass == BEGIN) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(BEGIN, READ, WRITE, WHILE, IF, FOR, ID, BEGIN, END)
    }

    NEXTRULE(_comandos, END)
    if (parser->lexer.tokenClass == END) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(END, DOT)
    }
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 3 of the grammar:
 * <dc> ::= <dc_c> <dc_v> <dc_p>
 * @param parser initialized parser instance
 */
void _dc(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    NEXTRULE(_dc_c, BEGIN, VAR, PROCEDURE)
    NEXTRULE(_dc_v, BEGIN, PROCEDURE)
    NEXTRULE(_dc_p, BEGIN)
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 4 of the grammar:
 * <dc_c> ::= const ident = <numero>  ; <dc_c> | lambda
 * @param parser initialized parser instance
 */
void _dc_c(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    if (parser->lexer.tokenClass == CONST) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {  // lambda
        _sincTokensDecr(sincTokens);
        return;
    }
    if (parser->lexer.tokenClass == ID) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(ID, ASSIGN)  // instead of ASSIGN should be only '='
    }
    if (parser->lexer.tokenClass == ASSIGN) {  // instead of ASSIGN should be only '='
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(ASSIGN, N_INTEGER, N_REAL)  // instead of ASSIGN should be only '='
    }

    NEXTRULE(_numero, SEMICOLON)
    if (parser->lexer.tokenClass == SEMICOLON) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(SEMICOLON, CONST, BEGIN, VAR, PROCEDURE);
    }

    NEXTRULE(_dc_c, BEGIN, VAR, PROCEDURE)
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 5 of the grammar:
 * <dc_v> ::= var <variaveis> : <tipo_var> ; <dc_v> | lambda
 * @param parser initialized parser instance
 */
void _dc_v(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    if (parser->lexer.tokenClass == VAR) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {  // lambda
        _sincTokensDecr(sincTokens);
        return;
    }

    NEXTRULE(_variaveis, DECLARE_TYPE)
    if (parser->lexer.tokenClass == DECLARE_TYPE) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(DECLARE_TYPE, REAL, INTEGER)
    }

    NEXTRULE(_tipo_var, SEMICOLON)
    if (parser->lexer.tokenClass == SEMICOLON) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(SEMICOLON, VAR, BEGIN, PROCEDURE)
    }

    NEXTRULE(_dc_v, BEGIN, PROCEDURE)
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 6 of the grammar:
 * <tipo_var> ::= real | integer
 * @param parser initialized parser instance
 */
void _tipo_var(Parser* parser, int sincTokens[]) {
    
    _sincTokensIncr(sincTokens);

    if (parser->lexer.tokenClass == REAL || parser->lexer.tokenClass == INTEGER) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {  // change
        // parser->errorCount++;
        // printf("Parser error on line %d col %d: expected real or integer but found %s\n", parser->lexer.currLine, lexerCurrColWithoutRetreat(&parser->lexer), lexerBuffer(&parser->lexer));
        FOLLOWERS(REAL, SEMICOLON, CLOSE_PAR)
    }
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 7 of the grammar:
 * <variaveis> ::= ident <mais_var>
 * @param parser initialized parser instance
 */
void _variaveis(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    if (parser->lexer.tokenClass == ID) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(ID, COLON, DECLARE_TYPE, CLOSE_PAR)
    }

    NEXTRULE(_mais_var, DECLARE_TYPE, CLOSE_PAR)
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 8 of the grammar:
 * <mais_var> ::= , <variaveis> | lambda
 * @param parser initialized parser instance
 */
void _mais_var(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    if (parser->lexer.tokenClass == COLON) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {  // lambda
        _sincTokensDecr(sincTokens);
        return;
    }

    NEXTRULE(_variaveis, DECLARE_TYPE, CLOSE_PAR)
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 9 of the grammar:
 * <dc_p> ::= procedure ident <parametros> ; <corpo_p> <dc_p> | lambda
 * @param parser initialized parser instance
 */
void _dc_p(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    if (parser->lexer.tokenClass == PROCEDURE) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {  // lambda
        _sincTokensDecr(sincTokens);
        return;
    }
    if (parser->lexer.tokenClass == ID) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(ID, OPEN_PAR, SEMICOLON)
    }

    NEXTRULE(_parametros, SEMICOLON)
    if (parser->lexer.tokenClass == SEMICOLON) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(SEMICOLON, VAR, BEGIN)
    }

    NEXTRULE(_corpo_p, BEGIN, PROCEDURE)
    NEXTRULE(_dc_p, BEGIN)
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 10 of the grammar:
 * <parametros> ::= ( <lista_par> ) | lambda
 * @param parser initialized parser instance
 */
void _parametros(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    if (parser->lexer.tokenClass == OPEN_PAR) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {  // lambda
        _sincTokensDecr(sincTokens);
        return;
    }

    NEXTRULE(_lista_par, CLOSE_PAR)
    if (parser->lexer.tokenClass == CLOSE_PAR) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(CLOSE_PAR, SEMICOLON)
    }
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 11 of the grammar:
 * <lista_par> ::= <variaveis> : <tipo_var> <mais_par>
 * @param parser initialized parser instance
 */
void _lista_par(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    NEXTRULE(_variaveis, DECLARE_TYPE)

    if (parser->lexer.tokenClass == DECLARE_TYPE) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(DECLARE_TYPE, REAL, INTEGER)
    }

    NEXTRULE(_tipo_var, COLON, DECLARE_TYPE, CLOSE_PAR)
    NEXTRULE(_mais_par, CLOSE_PAR)
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 12 of the grammar:
 * <mais_par> ::= ; <lista_par> | lambda
 * @param parser initialized parser instance
 */
void _mais_par(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    if (parser->lexer.tokenClass == SEMICOLON) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {  // lambda
        _sincTokensDecr(sincTokens);
        return;
    }

    NEXTRULE(_lista_par, CLOSE_PAR)
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 13 of the grammar:
 * <corpo_p> ::= <dc_loc> begin <comandos> end ;
 * @param parser initialized parser instance
 */
void _corpo_p(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    NEXTRULE(_dc_loc, BEGIN)

    if (parser->lexer.tokenClass == BEGIN) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(BEGIN, READ, WRITE, WHILE, IF, FOR, ID, BEGIN, END)
    }

    NEXTRULE(_comandos, END)
    if (parser->lexer.tokenClass == END) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(END, SEMICOLON)
    }
    if (parser->lexer.tokenClass == SEMICOLON) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(SEMICOLON, BEGIN, PROCEDURE)
    }
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 14 of the grammar:
 * <dc_loc> ::= <dc_v>
 * @param parser initialized parser instance
 */
void _dc_loc(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    NEXTRULE(_dc_v, BEGIN)
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 15 of the grammar:
 * <lista_arg> ::= ( <argumentos> ) | lambda
 * @param parser initialized parser instance
 */
void _lista_arg(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    if (parser->lexer.tokenClass == OPEN_PAR) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {  // lambda
        _sincTokensDecr(sincTokens);
        return;
    }

    NEXTRULE(_argumentos, CLOSE_PAR)
    if (parser->lexer.tokenClass == CLOSE_PAR) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(CLOSE_PAR, SEMICOLON)
    }
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 16 of the grammar:
 * <argumentos> ::= ident <mais_ident>
 * @param parser initialized parser instance
 */
void _argumentos(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    if (parser->lexer.tokenClass == ID) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(ID, SEMICOLON, CLOSE_PAR)
    }

    NEXTRULE(_mais_ident, CLOSE_PAR)
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 17 of the grammar:
 * <mais_ident> ::= ; <argumentos> | lambda
 * @param parser initialized parser instance
 */
void _mais_ident(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    if (parser->lexer.tokenClass == SEMICOLON) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {  // lambda
        _sincTokensDecr(sincTokens);
        return;
    }

    NEXTRULE(_argumentos, CLOSE_PAR)
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 18 of the grammar:
 * <pfalsa> ::= else <cmd> | lambda
 * @param parser initialized parser instance
 */
void _pfalsa(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    if (parser->lexer.tokenClass == ELSE) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {  // lambda
        _sincTokensDecr(sincTokens);
        return;
    }

    NEXTRULE(_cmd, SEMICOLON)
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 19 of the grammar:
 * <comandos> ::= <cmd> ; <comandos> | lambda
 * @param parser initialized parser instance
 */
void _comandos(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    if (parser->lexer.tokenClass != READ &&
        parser->lexer.tokenClass != WRITE &&
        parser->lexer.tokenClass != WHILE &&
        parser->lexer.tokenClass != IF &&
        parser->lexer.tokenClass != FOR &&
        parser->lexer.tokenClass != ID &&
        parser->lexer.tokenClass != BEGIN) {  // lookahead
        _sincTokensDecr(sincTokens);
        return;
    }

    NEXTRULE(_cmd, SEMICOLON)
    if (parser->lexer.tokenClass == SEMICOLON) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(SEMICOLON, READ, WRITE, WHILE, IF, FOR, ID, BEGIN, END)
    }

    NEXTRULE(_comandos, END)
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 20 of the grammar:
 * <cmd> ::= read ( <variaveis> ) |
            write ( <variaveis> ) |
            while ( <condicao> ) do <cmd> |
            if <condicao> then <cmd> <pfalsa> |
            for ident := <expressão> to <expressão> do <cmd>
            ident <pos_ident> |
            begin <comandos> end
 * @param parser initialized parser instance
 */
void _cmd(Parser* parser, int sincTokens[]) {
    
    _sincTokensIncr(sincTokens);
    
    if (parser->lexer.tokenClass == READ) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        if (parser->lexer.tokenClass == OPEN_PAR) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
        } else {
            FOLLOWERS(OPEN_PAR, ID)
        }
        NEXTRULE(_variaveis, CLOSE_PAR)
        if (parser->lexer.tokenClass == CLOSE_PAR) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
        } else {
            FOLLOWERS(CLOSE_PAR, SEMICOLON)
        }
    } else if (parser->lexer.tokenClass == WRITE) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        if (parser->lexer.tokenClass == OPEN_PAR) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
        } else {
            FOLLOWERS(OPEN_PAR, ID)
        }
        NEXTRULE(_variaveis, CLOSE_PAR)
        if (parser->lexer.tokenClass == CLOSE_PAR) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
        } else {
            FOLLOWERS(CLOSE_PAR, SEMICOLON)
        }
    } else if (parser->lexer.tokenClass == WHILE) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        if (parser->lexer.tokenClass == OPEN_PAR) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
        } else {
            FOLLOWERS(OPEN_PAR, OP_UN, ID, OPEN_PAR, N_INTEGER, N_REAL)
        }
        NEXTRULE(_condicao, CLOSE_PAR)
        if (parser->lexer.tokenClass == CLOSE_PAR) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
        } else {
            FOLLOWERS(CLOSE_PAR, DO)
        }
        if (parser->lexer.tokenClass == DO) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
        } else {
            FOLLOWERS(DO, READ, WRITE, WHILE, IF, FOR, ID, BEGIN)
        }
        NEXTRULE(_cmd, SEMICOLON)
    } else if (parser->lexer.tokenClass == IF) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        NEXTRULE(_condicao, THEN)
        if (parser->lexer.tokenClass == THEN) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
        } else {
            FOLLOWERS(THEN, READ, WRITE, WHILE, IF, FOR, ID, BEGIN)
        }
        NEXTRULE(_cmd, ELSE, SEMICOLON)
        NEXTRULE(_pfalsa, SEMICOLON)
    } else if (parser->lexer.tokenClass == FOR) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        if (parser->lexer.tokenClass == ID) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
        } else {
            FOLLOWERS(ID, ASSIGN)
        }
        if (parser->lexer.tokenClass == ASSIGN) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
        } else {
            FOLLOWERS(ASSIGN, OP_UN, ID, OPEN_PAR, N_INTEGER, N_REAL)
        }
        NEXTRULE(_expressao, TO)
        if (parser->lexer.tokenClass == TO) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
        } else {
            FOLLOWERS(TO, OP_UN, ID, OPEN_PAR, N_INTEGER, N_REAL)
        }
        NEXTRULE(_expressao, DO)
        if (parser->lexer.tokenClass == DO) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
        } else {
            FOLLOWERS(DO, READ, WRITE, WHILE, IF, FOR, ID, BEGIN)
        }
        NEXTRULE(_cmd, SEMICOLON)
    } else if (parser->lexer.tokenClass == ID) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        NEXTRULE(_pos_ident, SEMICOLON)
    } else if (parser->lexer.tokenClass == BEGIN) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        NEXTRULE(_comandos, END)
        if (parser->lexer.tokenClass == END) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
        } else {
            FOLLOWERS(END, SEMICOLON)
        }
    } else {
        FOLLOWERS(SEMICOLON, SEMICOLON)  // change expected token
    }
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 20.1 of the grammar:
 * <pos_ident> ::= := <expressao> | <lista_arg>
 * @param parser initialized parser instance
 */
void _pos_ident(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);

    if (parser->lexer.tokenClass == OPEN_PAR) {  // lookahead
        NEXTRULE(_lista_arg, SEMICOLON)
        _sincTokensDecr(sincTokens);
        return;
    } 
    else if (parser->lexer.tokenClass == ASSIGN) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(ASSIGN, OP_UN, ID, OPEN_PAR, N_INTEGER, N_REAL)
    }
    NEXTRULE(_expressao, SEMICOLON, RELATION, CLOSE_PAR, THEN, TO, DO)
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 21 of the grammar:
 * <condicao> ::= <expressao> <relacao> <expressao>
 * @param parser initialized parser instance
 */
void _condicao(Parser* parser, int sincTokens[]) {
    
    _sincTokensIncr(sincTokens);

    NEXTRULE(_expressao, RELATION)
    NEXTRULE(_relacao, OP_UN, ID, OPEN_PAR, N_INTEGER, N_REAL)
    NEXTRULE(_expressao, SEMICOLON, RELATION, CLOSE_PAR, THEN, TO, DO)
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 22 of the grammar:
 * <relacao> ::= = | <> | >= | <= | > | <
 * @param parser initialized parser instance
 */
void _relacao(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);

    if (parser->lexer.tokenClass == RELATION) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(RELATION, OP_UN, ID, OPEN_PAR, N_INTEGER, N_REAL)
    }
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 23 of the grammar:
 * <expressao> ::= <termo> <outros_termos>
 * @param parser initialized parser instance
 */
void _expressao(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    NEXTRULE(_termo, OP_UN)
    NEXTRULE(_outros_termos, SEMICOLON, RELATION, CLOSE_PAR, THEN, TO, DO)
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 24 of the grammar:
 * <op_un> ::= + | - | lambda
 * @param parser initialized parser instance
 */
void _op_un(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);

    if (parser->lexer.tokenClass == OP_UN) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    }
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 25 of the grammar:
 * <outros_termos> ::= <op_ad> <termo> <outros_termos> | lambda
 * @param parser initialized parser instance
 */
void _outros_termos(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    if (parser->lexer.tokenClass == OP_ADD) {  // lookahead
        NEXTRULE(_op_ad, OP_UN, ID, OPEN_PAR, N_INTEGER, N_REAL)
        NEXTRULE(_termo, OP_UN)
        NEXTRULE(_outros_termos, SEMICOLON, RELATION, CLOSE_PAR, THEN, TO, DO)
    }
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 26 of the grammar:
 * <op_ad> ::= + | -
 * @param parser initialized parser instance
 */
void _op_ad(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);

    if (parser->lexer.tokenClass == OP_ADD) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(OP_ADD, OP_UN, ID, OPEN_PAR, N_INTEGER, N_REAL)
    }
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 27 of the grammar:
 * <termo> ::= <op_un> <fator> <mais_fatores>
 * @param parser initialized parser instance
 */
void _termo(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    NEXTRULE(_op_un, ID, OPEN_PAR, N_INTEGER, N_REAL)
    NEXTRULE(_fator, OP_MULT)
    NEXTRULE(_mais_fatores, OP_UN)
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 28 of the grammar:
 * <mais_fatores> ::= <op_mul> <fator> <mais_fatores> | lambda
 * @param parser initialized parser instance
 */
void _mais_fatores(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);

    if (parser->lexer.tokenClass == OP_MULT) {  // lookahead
        NEXTRULE(_op_mul, ID, OPEN_PAR, N_INTEGER, N_REAL)
        NEXTRULE(_fator, OP_MULT)
        NEXTRULE(_mais_fatores, OP_UN)
    }
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 29 of the grammar:
 * <op_mul> ::= *|/
 * @param parser initialized parser instance
 */
void _op_mul(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    if (parser->lexer.tokenClass == OP_MULT) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        FOLLOWERS(OP_MULT, ID, OPEN_PAR, N_INTEGER, N_REAL)
    }
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 30 of the grammar:
 * <fator> ::= ident | <numero> | (<expressao>)
 * @param parser initialized parser instance
 */
void _fator(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);
    
    if (parser->lexer.tokenClass == ID) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else if (parser->lexer.tokenClass == OPEN_PAR) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        NEXTRULE(_expressao, CLOSE_PAR)
        if (parser->lexer.tokenClass == CLOSE_PAR) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
        } else {
            FOLLOWERS(CLOSE_PAR, OP_MULT)
        }
    } else {
        NEXTRULE(_numero, SEMICOLON, OP_MULT)
    }
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Implements rule 31 of the grammar:
 * <numero> ::= numero_int | numero_real
 * @param parser initialized parser instance
 */
void _numero(Parser* parser, int sincTokens[]) {

    _sincTokensIncr(sincTokens);

    if (parser->lexer.tokenClass == N_INTEGER || parser->lexer.tokenClass == N_REAL) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {  // change
        parser->errorCount++;
        printf("Parser error on line %d col %d: expected N_INTEGER or N_REAL but found %s\n", parser->lexer.currLine, lexerCurrColWithoutRetreat(&parser->lexer), lexerBuffer(&parser->lexer));
    }
    _sincTokensDecr(sincTokens);
}

/**
 * @brief Outputs a parser error given an expected token class
 *
 * @param parser initialized parser instance
 * @param expectedTokenClass expected token class
 */
void _error(Parser* parser, int expectedTokenClass, int sincTokens[]) {
    
    parser->errorCount++;

    String errorMsg;
    stringInit(&errorMsg);
    stringAppendCstr(&errorMsg, "Parser error on line ");
    stringAppendInt(&errorMsg, parser->lexer.currLine);
    stringAppendCstr(&errorMsg, " col ");
    stringAppendInt(&errorMsg, lexerCurrColWithoutRetreat(&parser->lexer));
    stringAppendCstr(&errorMsg, ": expected ");
    stringAppendCstr(&errorMsg, lexerTokenClassName(expectedTokenClass));
    stringAppendCstr(&errorMsg, " but found ");
    stringAppendCstr(&errorMsg, lexerBuffer(&parser->lexer));
    stringAppendChar(&errorMsg, '\n');
    printf("%s", errorMsg.str);
    fprintf(parser->output, "%s", errorMsg.str);

    stringDestroy(&errorMsg);

    while (sincTokens[parser->lexer.tokenClass] == -1)
        parser->errorCount += nextToken(&parser->lexer, parser->output);
}