/**
 * @file parser.h
 * @brief Syntax analyser (parser)
 */
#ifndef PARSER_H
#define PARSER_H

#include "../header/lexer.h"
#include "../header/linkedList.h"

// struct returned by the compiler
typedef struct {
    Lexer lexer;
    FILE* output;

    int errorCount;
} Parser;

bool parserInit(Parser* parser, const char* sourceCodePath);
void parserDestroy(Parser* parser);
void compile(Parser* parser);  // the syntax analyser controls the compilation process

void _error(Parser* parser, int expectedTokenClass, Node*  sincTokens[]);

// synchronization token vector management routines
void _sincTokensInit(Node*  sincTokens[]);
void _sincTokensIncr(Node*  sincTokens[]);
void _sincTokensDecr(Node*  sincTokens[]);
void _sincTokensAdd(Node*  sincTokens[], const int toAdd[], unsigned long toAddSize);

// P-- grammar
void _programa(Parser* parser, Node*  sincTokens[]);
void _corpo(Parser* parser, Node*  sincTokens[]);
void _dc(Parser* parser, Node*  sincTokens[]);
void _dc_c(Parser* parser, Node*  sincTokens[]);
void _dc_v(Parser* parser, Node*  sincTokens[]);
void _tipo_var(Parser* parser, Node*  sincTokens[]);
void _variaveis(Parser* parser, Node*  sincTokens[]);
void _mais_var(Parser* parser, Node*  sincTokens[]);
void _dc_p(Parser* parser, Node*  sincTokens[]);
void _parametros(Parser* parser, Node*  sincTokens[]);
void _lista_par(Parser* parser, Node*  sincTokens[]);
void _mais_par(Parser* parser, Node*  sincTokens[]);
void _corpo_p(Parser* parser, Node*  sincTokens[]);
void _dc_loc(Parser* parser, Node*  sincTokens[]);
void _lista_arg(Parser* parser, Node*  sincTokens[]);
void _argumentos(Parser* parser, Node*  sincTokens[]);
void _mais_ident(Parser* parser, Node*  sincTokens[]);
void _pfalsa(Parser* parser, Node*  sincTokens[]);
void _comandos(Parser* parser, Node*  sincTokens[]);
void _cmd(Parser* parser, Node*  sincTokens[]);
void _pos_ident(Parser* parser, Node*  sincTokens[]);
void _condicao(Parser* parser, Node*  sincTokens[]);
void _relacao(Parser* parser, Node*  sincTokens[]);
void _expressao(Parser* parser, Node*  sincTokens[]);
void _op_un(Parser* parser, Node*  sincTokens[]);
void _outros_termos(Parser* parser, Node*  sincTokens[]);
void _op_ad(Parser* parser, Node*  sincTokens[]);
void _termo(Parser* parser, Node*  sincTokens[]);
void _mais_fatores(Parser* parser, Node*  sincTokens[]);
void _op_mul(Parser* parser, Node*  sincTokens[]);
void _fator(Parser* parser, Node*  sincTokens[]);
void _numero(Parser* parser, Node*  sincTokens[]);

#endif  // PARSER_H