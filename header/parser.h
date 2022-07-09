/**
 * @file parser.h
 * @brief Syntax analyser (parser)
 */
#ifndef PARSER_H
#define PARSER_H

#include "../header/lexer.h"

// struct returned by the compiler
typedef struct {
    Lexer lexer;
    FILE* output;

    int errorCount;
} Parser;

bool parserInit(Parser* parser, const char* sourceCodePath);
void parserDestroy(Parser* parser);
void compile(Parser* parser);  // the syntax analyser controls the compilation process

void _error(Parser* parser, int expectedTokenClass, int sincTokens[]);

// synchronization token vector management routines
void _sincTokensInit(int sincTokens[]);
void _sincTokensCopy(int sincTokens[], int copySincTokens[]);
void _sincTokensIncr(int sincTokens[]);
void _sincTokensAdd(int sincTokens[], const int toAdd[], unsigned long toAddSize);

// P-- grammar
void _programa(Parser* parser, int sincTokens[]);
void _corpo(Parser* parser, int sincTokens[]);
void _dc(Parser* parser, int sincTokens[]);
void _dc_c(Parser* parser, int sincTokens[]);
void _dc_v(Parser* parser, int sincTokens[]);
void _tipo_var(Parser* parser, int sincTokens[]);
void _variaveis(Parser* parser, int sincTokens[]);
void _mais_var(Parser* parser, int sincTokens[]);
void _dc_p(Parser* parser, int sincTokens[]);
void _parametros(Parser* parser, int sincTokens[]);
void _lista_par(Parser* parser, int sincTokens[]);
void _mais_par(Parser* parser, int sincTokens[]);
void _corpo_p(Parser* parser, int sincTokens[]);
void _dc_loc(Parser* parser, int sincTokens[]);
void _lista_arg(Parser* parser, int sincTokens[]);
void _argumentos(Parser* parser, int sincTokens[]);
void _mais_ident(Parser* parser, int sincTokens[]);
void _pfalsa(Parser* parser, int sincTokens[]);
void _comandos(Parser* parser, int sincTokens[]);
void _cmd(Parser* parser, int sincTokens[]);
void _pos_ident(Parser* parser, int sincTokens[]);
void _condicao(Parser* parser, int sincTokens[]);
void _relacao(Parser* parser, int sincTokens[]);
void _expressao(Parser* parser, int sincTokens[]);
void _op_un(Parser* parser, int sincTokens[]);
void _outros_termos(Parser* parser, int sincTokens[]);
void _op_ad(Parser* parser, int sincTokens[]);
void _termo(Parser* parser, int sincTokens[]);
void _mais_fatores(Parser* parser, int sincTokens[]);
void _op_mul(Parser* parser, int sincTokens[]);
void _fator(Parser* parser, int sincTokens[]);
void _numero(Parser* parser, int sincTokens[]);

#endif  // PARSER_H