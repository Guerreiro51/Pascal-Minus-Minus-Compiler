/**
 * @file parser.c
 * @brief Syntax analyser (parser) implementation
 *
 */
#include "../header/parser.h"

#include <stdio.h>
#include <stdlib.h>

#include "../header/vector.h"

/**
 * @brief Controls the compilation process.
 *
 * @param sourceCode pointer to P-- source code file
 * @return int compiler termination status
 */
CompileRet compile(FILE* sourceCode) {
    // initializes a lexical analyser (lexer)
    Lexer lexer;
    lexerInit(&lexer, sourceCode);

    // stores the last lexem read by the lexer
    CompileRet compileRet;
    compileRet.errorCount = 0;

    // open output file
    FILE* output = fopen( "output.txt", "w" );
    if( output == NULL ){
        printf("Error: couldn't open output file\n");
        compileRet.errorCount = -1;
        return compileRet;
    }

    compileRet.errorCount += nextToken(&lexer, output);  // get next token

    // initial variable: programa
    compileRet.errorCount += _programa(&lexer, output);

    if( lexer.tokenClass != EOF ){
        compileRet.errorCount++;
        printf("Parser error on line %d col %d: expected EOF but found %s\n", lexer.currLine, lexer.currCol, lexer.buffer.str);
    }
    
    stringDestroy(&lexer.buffer);
    fclose(output);
    return compileRet;
}

/**
 * @brief 
 * 
 *
 * @param tokenClass token class number
 * @return char* token class name
 */
char* _getTokenClassName(int tokenClass) {
    // indexes token class names for user-friendly printing
    static char* tokenClassName[] = {"", "N_REAL", "N_INTEGER", "OP_UN", "OP_ADD", "OP_MULT", "RELATION",
                                     "ASSIGN", "DECLARE_TYPE", "SEMICOLON", "COLON",
                                     "OPEN_PAR", "CLOSE_PAR", "DOT", "ID", "BEGIN", "CONST",
                                     "DO", "END", "ELSE", "IF", "INTEGER", "FOR", "PROGRAM", "PROCEDURE",
                                     "REAL", "READ", "THEN", "TO", "VAR", "WRITE", "WHILE", "ERROR"};
    return tokenClassName[tokenClass];
}

/**
 * @brief Implements rule 1 of the grammar:
 * <programa> ::= program ident ; <corpo> .
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _programa(Lexer* lexer, FILE* output){
    int errorCount = 0;
    if( lexer->tokenClass == PROGRAM ){
        errorCount += nextToken(lexer, output); 
        if( lexer->tokenClass == ID ){
            errorCount += nextToken(lexer, output);   
            if( lexer->tokenClass == SEMICOLON ){
                errorCount += nextToken(lexer, output);
                errorCount += _corpo(lexer, output);
                if( lexer->tokenClass == DOT ){
                    errorCount += nextToken(lexer, output);
                    return errorCount;
                }
                else{
                    printf("Parser error on line %d col %d: expected '.' but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
                }
            }
            else {
                printf("Parser error on line %d col %d: expected ';' but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
            }
        }
        else{
            printf("Parser error on line %d col %d: expected ID but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
        }
    }
    else{
        printf("Parser error on line %d col %d: expected 'program' keyword but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
    }
    return ++errorCount;
}

/**
 * @brief Implements rule 2 of the grammar:
 * <corpo> ::= <dc> begin <comandos> end
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _corpo(Lexer* lexer, FILE* output){
    
    int errorCount = 0;
    
    errorCount += _dc(lexer, output);
    if( lexer->tokenClass == BEGIN ){
        errorCount += nextToken(lexer, output);
        errorCount += _comandos(lexer, output);
        if( lexer->tokenClass == END ){
            errorCount += nextToken(lexer, output);
            return errorCount;
        }else{
            printf("Parser error on line %d col %d: expected 'end' keyword but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
        }
    }
    else{
        printf("Parser error on line %d col %d: expected 'begin' keyword but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
    }
    return ++errorCount;
}

/**
 * @brief Implements rule 3 of the grammar:
 * <dc> ::= <dc_c> <dc_v> <dc_p>
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _dc(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    errorCount += _dc_c(lexer, output);
    errorCount += _dc_v(lexer, output);
    errorCount += _dc_p(lexer, output);

    return errorCount;
}

/**
 * @brief Implements rule 4 of the grammar:
 * <dc_c> ::= const ident = <numero>  ; <dc_c> | lambda
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _dc_c(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    if(lexer->tokenClass == CONST) {
        errorCount += nextToken(lexer, output);
        if(lexer->tokenClass == ID) {
            errorCount += nextToken(lexer, output);
            if(lexer->tokenClass == ASSIGN) {
                errorCount += nextToken(lexer, output);
                errorCount += _numero(lexer, output);
                if(lexer->tokenClass == SEMICOLON) {
                    errorCount += nextToken(lexer, output);
                    errorCount += _dc_c(lexer, output);
                    return errorCount;
                }
                else {
                    printf("Parser error on line %d col %d: expected ';' but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
                }
            }
            else {
                printf("Parser error on line %d col %d: expected ':=' but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
            }
        }
        else {
            printf("Parser error on line %d col %d: expected ID but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
        }
    }
    else{
        return errorCount;
    }
    return ++errorCount;
}

/**
 * @brief Implements rule 5 of the grammar:
 * <dc_v> ::= var <variaveis> : <tipo_var> ; <dc_v> | lambda
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _dc_v(Lexer* lexer, FILE* output){
    int errorCount = 0;
    if( lexer->tokenClass == VAR ){
        errorCount += nextToken(lexer, output);
        errorCount += _variaveis(lexer, output);
        if( lexer->tokenClass == DECLARE_TYPE ){
            errorCount += nextToken(lexer, output);
            errorCount += _tipo_var(lexer, output);
            if( lexer->tokenClass == SEMICOLON ){
                errorCount += nextToken(lexer, output);
                errorCount += _dc_v(lexer, output);
                return errorCount;
            }
            else{
                printf("Parser error on line %d col %d: expected ';' but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
            }
        }
        else{
            printf("Parser error on line %d col %d: expected ':' but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
        }
    }
    else{
        return errorCount;
    }
    return ++errorCount;
}


/**
 * @brief Implements rule 6 of the grammar:
 * <tipo_var> ::= real | integer
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _tipo_var(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    if(lexer->tokenClass == REAL || lexer->tokenClass == INTEGER) {
        errorCount += nextToken(lexer, output);
        return errorCount;
    }

    return ++errorCount;
}

/**
 * @brief Implements rule 7 of the grammar:
 * <variaveis> ::= ident <mais_var>
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _variaveis(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    if(lexer->tokenClass == ID) {
        errorCount += nextToken(lexer, output);
        errorCount += _mais_var(lexer, output);
        return errorCount;
    }
    else {
        printf("Parser error on line %d col %d: expected ID but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
    }

    return ++errorCount;
}

/**
 * @brief Implements rule 8 of the grammar:
 * <mais_var> ::= , <variaveis> | lambda
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _mais_var(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    if(lexer->tokenClass == COLON) {
        errorCount += nextToken(lexer, output);
        errorCount += _variaveis(lexer, output);
    }
    return errorCount;
}

/**
 * @brief Implements rule 9 of the grammar:
 * <dc_p> ::= procedure ident <parametros> ; <corpo_p> <dc_p> | lambda
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _dc_p(Lexer* lexer, FILE* output){
    int errorCount = 0;
    if( lexer->tokenClass == PROCEDURE ){
        errorCount += nextToken(lexer, output);
        if( lexer->tokenClass == ID ){
            errorCount += nextToken(lexer, output);
            errorCount += _parametros(lexer, output);
            if( lexer->tokenClass == SEMICOLON ){
                errorCount += nextToken(lexer, output);
                errorCount += _dc_p(lexer, output);
                return errorCount;
            }
            else{
                printf("Parser error on line %d col %d: expected ';' but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
            }
        }
        else{
            printf("Parser error on line %d col %d: expected ID but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
        }
    }
    else{
        return errorCount;
    }
    return ++errorCount;
}

/**
 * @brief Implements rule 10 of the grammar:
 * <parametros> ::= ( <lista_par> ) | λ
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _parametros(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    if(lexer->tokenClass == OPEN_PAR) {
        errorCount += nextToken(lexer, output);
        errorCount += _lista_par(lexer, output);
        if(lexer->tokenClass == CLOSE_PAR) {
            errorCount += nextToken(lexer, output);
            return errorCount;
        }
        else {
            printf("Parser error on line %d col %d: expected ')' keyword but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
        }
    }
    else {
        return errorCount;
    }
    return ++errorCount;
}

/**
 * @brief Implements rule 11 of the grammar:
 * <lista_par> ::= <variaveis> : <tipo_var> <mais_par>
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _lista_par(Lexer* lexer, FILE* output){
    int errorCount = 0;
    errorCount += _variaveis(lexer, output);
    if( lexer->tokenClass == DECLARE_TYPE ){
        errorCount += nextToken(lexer, output);
        errorCount += _tipo_var(lexer, output);
        errorCount += _mais_par(lexer, output);
        return errorCount;
    }
    else {
        printf("Parser error on line %d col %d: expected ':' keyword but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
    }
    return ++errorCount;
}

/**
 * @brief Implements rule 12 of the grammar:
 * <mais_par> ::= ; <lista_par> | lambda
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _mais_par(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    if(lexer->tokenClass == SEMICOLON) {
        errorCount += nextToken(lexer, output);
        errorCount += _lista_par(lexer, output);
        return errorCount;
    }
    else {
        return errorCount;
    }

    return ++errorCount;
}

/**
 * @brief Implements rule 13 of the grammar:
 * <corpo_p> ::= <dc_loc> begin <comandos> end ;
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _corpo_p(Lexer* lexer, FILE* output){
    int errorCount = 0;
    errorCount += _dc_loc(lexer, output);
    if( lexer->tokenClass == BEGIN ){
        errorCount += nextToken(lexer, output);
        errorCount += _comandos(lexer, output);
        if( lexer->tokenClass == END ){
            errorCount += nextToken(lexer, output);
            if( lexer->tokenClass == SEMICOLON ){
                errorCount += nextToken(lexer, output);
                return errorCount;
            }
            else {
                printf("Parser error on line %d col %d: expected ';' but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
            }
        }
        else{
            printf("Parser error on line %d col %d: expected 'end' keyword but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
        }
    }
    else {
        printf("Parser error on line %d col %d: expected 'begin' keyword but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
    }
}

/**
 * @brief Implements rule 14 of the grammar:
 * <dc_loc> ::= <dc_v>
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _dc_loc(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    errorCount += _dc_v(lexer, output);

    return errorCount;
}

/**
 * @brief Implements rule 15 of the grammar:
 * <lista_arg> ::= ( <argumentos> ) | lambda
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _lista_arg(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    if(lexer->tokenClass == OPEN_PAR) {
        errorCount += nextToken(lexer, output);
        errorCount += _argumentos(lexer, output);
        if(lexer->tokenClass == CLOSE_PAR) {
            errorCount += nextToken(lexer, output);
            return errorCount;
        }
        else {
            printf("Parser error on line %d col %d: expected ')' keyword but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
        }
    }
    else {
        return errorCount;
    }
    return ++errorCount;
}


/**
 * @brief Implements rule 16 of the grammar:
 * <argumentos> ::= ident <mais_ident>
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _argumentos(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    if(lexer->tokenClass == ID) {
        errorCount += _mais_ident(lexer, output);
        return errorCount;
    }
    else {
        printf("Parser error on line %d col %d: expected ID but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
    }

    return ++errorCount;
}

/**
 * @brief Implements rule 17 of the grammar:
 * <mais_ident> ::= ; <argumentos> | λ
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _mais_ident(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    if(lexer->tokenClass == SEMICOLON) {
        errorCount += nextToken(lexer, output);
        errorCount += _argumentos(lexer, output);
    }
    return errorCount;
}


/**
 * @brief Implements rule 18 of the grammar:
 * <pfalsa> ::= else <cmd> | lambda
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _pfalsa(Lexer* lexer, FILE* output){
    int errorCount = 0;
    if( lexer->tokenClass == ELSE ){
        errorCount += nextToken(lexer, output);
        errorCount += _cmd(lexer, output);
    }
    return errorCount;
}

/**
 * @brief Implements rule 19 of the grammar:
 * <comandos> ::= <cmd> ; <comandos> | λ
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _comandos(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    if (lexer->tokenClass == READ   ||
        lexer->tokenClass == WRITE  ||
        lexer->tokenClass == WHILE  ||
        lexer->tokenClass == IF     ||
        lexer->tokenClass == FOR    ||
        lexer->tokenClass == ID     ||
        lexer->tokenClass == BEGIN) {
        errorCount += _cmd(lexer, output);
        if(lexer->tokenClass == SEMICOLON) {
            errorCount += nextToken(lexer, output);
            errorCount += _comandos(lexer, output);
            return errorCount;
        } else {
            printf("Parser error on line %d col %d: expected ';' keyword but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
        }
    }
    else
        return errorCount;
    
    return ++errorCount;
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
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _cmd(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    if(lexer->tokenClass == READ) {
        errorCount += nextToken(lexer, output);
        if(lexer->tokenClass == OPEN_PAR) {
            errorCount += nextToken(lexer, output);
            errorCount += _variaveis(lexer, output);
            if(lexer->tokenClass == CLOSE_PAR) {
                errorCount += nextToken(lexer, output);
                return errorCount;
            }
        }
    }
    else if(lexer->tokenClass == WRITE) {
        errorCount += nextToken(lexer, output);
        if(lexer->tokenClass == OPEN_PAR) {
            errorCount += nextToken(lexer, output);
            errorCount += _variaveis(lexer, output);
            if(lexer->tokenClass == CLOSE_PAR) {
                errorCount += nextToken(lexer, output);
                return errorCount;
            }
        }
    }
    else if(lexer->tokenClass == WHILE) {
        errorCount += nextToken(lexer, output);
        if(lexer->tokenClass == OPEN_PAR) {
            errorCount += nextToken(lexer, output);
            errorCount += _condicao(lexer, output);
            if(lexer->tokenClass == CLOSE_PAR) {
                errorCount += nextToken(lexer, output);
                if(lexer->tokenClass == DO) {
                    errorCount += nextToken(lexer, output);
                    errorCount += _cmd(lexer, output);
                    return errorCount;
                }
            }
        }
    }
    else if(lexer->tokenClass == IF) {
        errorCount += nextToken(lexer, output);
        errorCount += _condicao(lexer, output);
        if(lexer->tokenClass == THEN) {
            errorCount += nextToken(lexer, output);
            errorCount += _cmd(lexer, output);
            errorCount += _pfalsa(lexer, output);
            return errorCount;
        }
    }
    else if(lexer->tokenClass == FOR) {
        errorCount += nextToken(lexer, output);
        if(lexer->tokenClass ==  ID) {
            errorCount += nextToken(lexer, output);
            if(lexer->tokenClass == ASSIGN) {
                errorCount += nextToken(lexer, output);
                errorCount += _expressao(lexer, output);
                if(lexer->tokenClass == TO) {
                    errorCount += nextToken(lexer, output);
                    errorCount += _expressao(lexer, output);
                    if(lexer->tokenClass == DO) {
                        errorCount += nextToken(lexer, output);
                        errorCount += _cmd(lexer, output);
                        return errorCount;
                    }
                }
            }
        }
    }
    else if(lexer->tokenClass == ID) {
        errorCount += nextToken(lexer, output);
        errorCount += _pos_ident(lexer, output);
        return errorCount;
    }
    else if(lexer->tokenClass == BEGIN) {
        errorCount += nextToken(lexer, output);
        errorCount += _comandos(lexer, output);
        if(lexer->tokenClass == END) {
            errorCount += nextToken(lexer, output);
            return errorCount;
        }
    }
    else {
        printf("Parser error on line %d col %d: invalid command\n", lexer->currLine, lexer->currCol);
    }

    return ++errorCount;
}

/**
 * @brief Implements rule 20.1 of the grammar:
 * <pos_ident> ::= := <expressao> | <lista_arg>
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _pos_ident(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    if(lexer->tokenClass == ASSIGN){
        errorCount += nextToken(lexer, output);
        errorCount += _expressao(lexer, output);
    }
    else{
        errorCount += _lista_arg(lexer, output);
    }
    
    return errorCount;
}

/**
 * @brief Implements rule 21 of the grammar:
 * <condicao> ::= <expressao> <relacao> <expressao>
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _condicao(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    errorCount += _expressao(lexer, output);
    errorCount += _relacao(lexer, output);
    errorCount += _expressao(lexer, output);
    
    return errorCount;
}

/**
 * @brief Implements rule 22 of the grammar:
 * <relacao> ::= = | <> | >= | <= | > | <
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _relacao(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    if( lexer->tokenClass == RELATION ){
        errorCount += nextToken(lexer, output);
        return errorCount;
    }
    else{
        printf("Parser error on line %d col %d: expected '=', '<>', '>=', '<=', '>', '<' but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
    }

    return ++errorCount;
}

/**
 * @brief Implements rule 23 of the grammar:
 * <expressao> ::= <termo> <outros_termos>
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _expressao(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    errorCount += _termo(lexer, output);
    errorCount += _outros_termos(lexer, output);
    return errorCount;
}

/**
 * @brief Implements rule 24 of the grammar:
 * <op_un> ::= + | - | lambda
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _op_un(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    if(lexer->tokenClass == OP_UN) {
        errorCount += nextToken(lexer, output);
    }
    return errorCount;
}

/**
 * @brief Implements rule 25 of the grammar:
 * <outros_termos> ::= <op_ad> <termo> <outros_termos> | lambda
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _outros_termos(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    if(lexer->tokenClass == OP_ADD) {
        errorCount += _op_ad(lexer, output);
        errorCount += _termo(lexer, output);
        errorCount += _outros_termos(lexer, output);
    }
    return errorCount;
}

/**
 * @brief Implements rule 26 of the grammar:
 * <op_ad> ::= + | -
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _op_ad(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    if(lexer->tokenClass == OP_ADD) {
        errorCount += nextToken(lexer, output);
        return errorCount;
    }
    else {
        printf("Parser error on line %d col %d: expected '+' or '-' but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
    }
    return ++errorCount;
}

/**
 * @brief Implements rule 27 of the grammar:
 * <termo> ::= <op_un> <fator> <mais_fatores>
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _termo(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    errorCount += _op_un(lexer, output);
    errorCount += _fator(lexer, output);
    errorCount += _mais_fatores(lexer, output);
    return errorCount;
}

/**
 * @brief Implements rule 28 of the grammar:
 * <mais_fatores> ::= <op_mul> <fator> <mais_fatores> | λ
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _mais_fatores(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    if(lexer->tokenClass == OP_MULT) {
        errorCount += _op_mul(lexer, output);
        errorCount += _fator(lexer, output);
        errorCount += _mais_fatores(lexer, output);
    }
    return errorCount;
}
    

/**
 * @brief Implements rule 29 of the grammar:
 * <op_mul> ::= *|/
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _op_mul(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    if(lexer->tokenClass == OP_MULT) {
        errorCount += nextToken(lexer, output);
        return errorCount;
    }
    else {
        printf("Parser error on line %d col %d: expected '*' or '/' but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
    }
    return ++errorCount;
}

/**
 * @brief Implements rule 30 of the grammar:
 * <fator> ::= ident | <numero> | (<expressao>)
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _fator(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    if(lexer->tokenClass == ID){
        errorCount += nextToken(lexer, output);
        return errorCount;
    }
    else if(lexer->tokenClass == OPEN_PAR){
        errorCount += nextToken(lexer, output);
        errorCount += _expressao(lexer, output);
        if(lexer->tokenClass == CLOSE_PAR){
            errorCount += nextToken(lexer, output);
            return errorCount;
        }
        else {
            printf("Parser error on line %d col %d: expected ')' keyword but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
        }
    }
    else {
        errorCount += _numero(lexer, output);
    }
    return errorCount;
}

/**
 * @brief Implements rule 31 of the grammar:
 * <numero> ::= numero_int | numero_real
 * @param lexer lexer instance
 * @param output output file pointer
 */
int _numero(Lexer* lexer, FILE* output) {
    int errorCount = 0;
    if(lexer->tokenClass == N_INTEGER || lexer->tokenClass == N_REAL) {
        errorCount += nextToken(lexer, output);
        return errorCount;
    }
    else {
        printf("Parser error on line %d col %d: expected N_INTEGER or N_REAL keyword but found %s\n", lexer->currLine, lexer->currCol, lexer->buffer.str);
    }
    return ++errorCount;
}