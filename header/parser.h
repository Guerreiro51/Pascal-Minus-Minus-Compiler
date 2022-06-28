/**
 * @file parser.h
 * @brief Syntax analyser (parser)
 */
#ifndef PARSER_H
#define PARSER_H

#include "../header/lexer.h"

// struct returned by the compiler
typedef struct {
    int errorCount;
} CompileRet;

CompileRet compile(FILE* sourceCode);        // the syntax analyser controls the compilation process
char* _getTokenClassName(int token_class);   // returns token class name given token class number

// P-- grammar
int _programa(Lexer* lexer, FILE* output);
int _corpo(Lexer* lexer, FILE* output);
int _dc(Lexer* lexer, FILE* output);
int _dc_c(Lexer* lexer, FILE* output);
int _dc_v(Lexer* lexer, FILE* output);
int _tipo_var(Lexer* lexer, FILE* output);
int _variaveis(Lexer* lexer, FILE* output);
int _mais_var(Lexer* lexer, FILE* output);
int _dc_p(Lexer* lexer, FILE* output);
int _parametros(Lexer* lexer, FILE* output);
int _lista_par(Lexer* lexer, FILE* output);
int _mais_par(Lexer* lexer, FILE* output);
int _corpo_p(Lexer* lexer, FILE* output);
int _dc_loc(Lexer* lexer, FILE* output) ;
int _lista_arg(Lexer* lexer, FILE* output) ;
int _argumentos(Lexer* lexer, FILE* output) ;
int _mais_ident(Lexer* lexer, FILE* output) ;
int _pfalsa(Lexer* lexer, FILE* output);
int _comandos(Lexer* lexer, FILE* output);
int _cmd(Lexer* lexer, FILE* output);
int _pos_ident(Lexer* lexer, FILE* output);
int _condicao(Lexer* lexer, FILE* output);
int _relacao(Lexer* lexer, FILE* output);
int _expressao(Lexer* lexer, FILE* output);
int _op_un(Lexer* lexer, FILE* output);
int _outros_termos(Lexer* lexer, FILE* output);
int _op_ad(Lexer* lexer, FILE* output);
int _termo(Lexer* lexer, FILE* output);
int _mais_fatores(Lexer* lexer, FILE* output);
int _op_mul(Lexer* lexer, FILE* output);
int _fator(Lexer* lexer, FILE* output);
int _numero(Lexer* lexer, FILE* output);




#endif  // PARSER_H