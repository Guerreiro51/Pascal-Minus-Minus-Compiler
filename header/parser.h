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
void _sincTokensAdd(int sincTokens[], int toAdd[], unsigned long toAddSize);

// P-- grammar
void _programa(Parser* parser, int sincTokens[]);
void _corpo(Parser* parser);
void _dc(Parser* parser);
void _dc_c(Parser* parser);
void _dc_v(Parser* parser);
void _tipo_var(Parser* parser);
void _variaveis(Parser* parser);
void _mais_var(Parser* parser);
void _dc_p(Parser* parser);
void _parametros(Parser* parser);
void _lista_par(Parser* parser);
void _mais_par(Parser* parser);
void _corpo_p(Parser* parser);
void _dc_loc(Parser* parser);
void _lista_arg(Parser* parser);
void _argumentos(Parser* parser);
void _mais_ident(Parser* parser);
void _pfalsa(Parser* parser);
void _comandos(Parser* parser);
void _cmd(Parser* parser);
void _pos_ident(Parser* parser);
void _condicao(Parser* parser);
void _relacao(Parser* parser);
void _expressao(Parser* parser);
void _op_un(Parser* parser);
void _outros_termos(Parser* parser);
void _op_ad(Parser* parser);
void _termo(Parser* parser);
void _mais_fatores(Parser* parser);
void _op_mul(Parser* parser);
void _fator(Parser* parser);
void _numero(Parser* parser);

#endif  // PARSER_H