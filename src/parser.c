/**
 * @file parser.c
 * @brief Syntax analyser (parser) implementation
 *
 */
#include "../header/parser.h"

#include <stdio.h>
#include <stdlib.h>

#include "../header/vector.h"

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

void parserDestroy(Parser* parser) {
    fclose(parser->output);
    lexerDestroy(&parser->lexer);
}

/**
 * @brief Controls the compilation process.
 *
 * @param Parser initialized parser instance
 */
void compile(Parser* parser) {
    parser->errorCount += nextToken(&parser->lexer, parser->output);  // get next token

    // initial variable: programa
    _programa(parser);

    if (parser->lexer.tokenClass != EOF) {
        _error(parser, EOF);
    }
}

/**
 * @brief Implements rule 1 of the grammar:
 * <programa> ::= program ident ; <corpo> .
 * @param Parser initialized parser instance
 */
void _programa(Parser* parser) {
    if (parser->lexer.tokenClass == PROGRAM) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        if (parser->lexer.tokenClass == ID) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
            if (parser->lexer.tokenClass == SEMICOLON) {
                parser->errorCount += nextToken(&parser->lexer, parser->output);
                _corpo(parser);
                if (parser->lexer.tokenClass == DOT) {
                    parser->errorCount += nextToken(&parser->lexer, parser->output);
                } else {
                    _error(parser, DOT);
                }
            } else {
                _error(parser, SEMICOLON);
            }
        } else {
            _error(parser, ID);
        }
    } else {
        _error(parser, PROGRAM);
    }
}

/**
 * @brief Implements rule 2 of the grammar:
 * <corpo> ::= <dc> begin <comandos> end
 * @param Parser initialized parser instance
 */
void _corpo(Parser* parser) {
    _dc(parser);
    if (parser->lexer.tokenClass == BEGIN) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        _comandos(parser);
        if (parser->lexer.tokenClass == END) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
        } else {
            _error(parser, END);
        }
    } else {
        _error(parser, BEGIN);
    }
}

/**
 * @brief Implements rule 3 of the grammar:
 * <dc> ::= <dc_c> <dc_v> <dc_p>
 * @param Parser initialized parser instance
 */
void _dc(Parser* parser) {
    _dc_c(parser);
    _dc_v(parser);
    _dc_p(parser);
}

/**
 * @brief Implements rule 4 of the grammar:
 * <dc_c> ::= const ident = <numero>  ; <dc_c> | lambda
 * @param Parser initialized parser instance
 */
void _dc_c(Parser* parser) {
    if (parser->lexer.tokenClass == CONST) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        if (parser->lexer.tokenClass == ID) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
            if (parser->lexer.tokenClass == ASSIGN) {  // should be only '='
                parser->errorCount += nextToken(&parser->lexer, parser->output);
                _numero(parser);
                if (parser->lexer.tokenClass == SEMICOLON) {
                    parser->errorCount += nextToken(&parser->lexer, parser->output);
                    _dc_c(parser);
                } else {
                    _error(parser, SEMICOLON);
                }
            } else {
                _error(parser, ASSIGN);  // should be only '='
            }
        } else {
            _error(parser, ID);
        }
    }
}

/**
 * @brief Implements rule 5 of the grammar:
 * <dc_v> ::= var <variaveis> : <tipo_var> ; <dc_v> | lambda
 * @param Parser initialized parser instance
 */
void _dc_v(Parser* parser) {
    if (parser->lexer.tokenClass == VAR) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        _variaveis(parser);
        if (parser->lexer.tokenClass == DECLARE_TYPE) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
            _tipo_var(parser);
            if (parser->lexer.tokenClass == SEMICOLON) {
                parser->errorCount += nextToken(&parser->lexer, parser->output);
                _dc_v(parser);
            } else {
                _error(parser, SEMICOLON);
            }
        } else {
            _error(parser, DECLARE_TYPE);
        }
    }
}

/**
 * @brief Implements rule 6 of the grammar:
 * <tipo_var> ::= real | integer
 * @param Parser initialized parser instance
 */
void _tipo_var(Parser* parser) {
    if (parser->lexer.tokenClass == REAL || parser->lexer.tokenClass == INTEGER) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        parser->errorCount++;
        printf("Parser error on line %d col %d: expected real or integer but found %s\n", parser->lexer.currLine, lexerCurrColWithoutRetreat(&parser->lexer), parser->lexer.buffer.str);
    }
}

/**
 * @brief Implements rule 7 of the grammar:
 * <variaveis> ::= ident <mais_var>
 * @param Parser initialized parser instance
 */
void _variaveis(Parser* parser) {
    if (parser->lexer.tokenClass == ID) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        _mais_var(parser);
    } else {
        _error(parser, ID);
    }
}

/**
 * @brief Implements rule 8 of the grammar:
 * <mais_var> ::= , <variaveis> | lambda
 * @param Parser initialized parser instance
 */
void _mais_var(Parser* parser) {
    if (parser->lexer.tokenClass == COLON) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        _variaveis(parser);
    }
}

/**
 * @brief Implements rule 9 of the grammar:
 * <dc_p> ::= procedure ident <parametros> ; <corpo_p> <dc_p> | lambda
 * @param Parser initialized parser instance
 */
void _dc_p(Parser* parser) {
    if (parser->lexer.tokenClass == PROCEDURE) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        if (parser->lexer.tokenClass == ID) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
            _parametros(parser);
            if (parser->lexer.tokenClass == SEMICOLON) {
                parser->errorCount += nextToken(&parser->lexer, parser->output);
                _corpo_p(parser);
                _dc_p(parser);
            } else {
                _error(parser, SEMICOLON);
            }
        } else {
            _error(parser, ID);
        }
    }
}

/**
 * @brief Implements rule 10 of the grammar:
 * <parametros> ::= ( <lista_par> ) | λ
 * @param Parser initialized parser instance
 */
void _parametros(Parser* parser) {
    if (parser->lexer.tokenClass == OPEN_PAR) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        _lista_par(parser);
        if (parser->lexer.tokenClass == CLOSE_PAR) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
        } else {
            _error(parser, CLOSE_PAR);
        }
    }
}

/**
 * @brief Implements rule 11 of the grammar:
 * <lista_par> ::= <variaveis> : <tipo_var> <mais_par>
 * @param Parser initialized parser instance
 */
void _lista_par(Parser* parser) {
    _variaveis(parser);
    if (parser->lexer.tokenClass == DECLARE_TYPE) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        _tipo_var(parser);
        _mais_par(parser);
    } else {
        _error(parser, DECLARE_TYPE);
    }
}

/**
 * @brief Implements rule 12 of the grammar:
 * <mais_par> ::= ; <lista_par> | lambda
 * @param Parser initialized parser instance
 */
void _mais_par(Parser* parser) {
    if (parser->lexer.tokenClass == SEMICOLON) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        _lista_par(parser);
    }
}

/**
 * @brief Implements rule 13 of the grammar:
 * <corpo_p> ::= <dc_loc> begin <comandos> end ;
 * @param Parser initialized parser instance
 */
void _corpo_p(Parser* parser) {
    _dc_loc(parser);
    if (parser->lexer.tokenClass == BEGIN) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        _comandos(parser);
        if (parser->lexer.tokenClass == END) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
            if (parser->lexer.tokenClass == SEMICOLON) {
                parser->errorCount += nextToken(&parser->lexer, parser->output);
            } else {
                _error(parser, SEMICOLON);
            }
        } else {
            _error(parser, END);
        }
    } else {
        _error(parser, BEGIN);
    }
}

/**
 * @brief Implements rule 14 of the grammar:
 * <dc_loc> ::= <dc_v>
 * @param Parser initialized parser instance
 */
void _dc_loc(Parser* parser) {
    _dc_v(parser);
}

/**
 * @brief Implements rule 15 of the grammar:
 * <lista_arg> ::= ( <argumentos> ) | lambda
 * @param Parser initialized parser instance
 */
void _lista_arg(Parser* parser) {
    if (parser->lexer.tokenClass == OPEN_PAR) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        _argumentos(parser);
        if (parser->lexer.tokenClass == CLOSE_PAR) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
        } else {
            _error(parser, CLOSE_PAR);
        }
    }
}

/**
 * @brief Implements rule 16 of the grammar:
 * <argumentos> ::= ident <mais_ident>
 * @param Parser initialized parser instance
 */
void _argumentos(Parser* parser) {
    if (parser->lexer.tokenClass == ID) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        _mais_ident(parser);
    } else {
        _error(parser, ID);
    }
}

/**
 * @brief Implements rule 17 of the grammar:
 * <mais_ident> ::= ; <argumentos> | λ
 * @param Parser initialized parser instance
 */
void _mais_ident(Parser* parser) {
    if (parser->lexer.tokenClass == SEMICOLON) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        _argumentos(parser);
    }
}

/**
 * @brief Implements rule 18 of the grammar:
 * <pfalsa> ::= else <cmd> | lambda
 * @param Parser initialized parser instance
 */
void _pfalsa(Parser* parser) {
    if (parser->lexer.tokenClass == ELSE) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        _cmd(parser);
    }
}

/**
 * @brief Implements rule 19 of the grammar:
 * <comandos> ::= <cmd> ; <comandos> | λ
 * @param Parser initialized parser instance
 */
void _comandos(Parser* parser) {
    if (parser->lexer.tokenClass == READ ||
        parser->lexer.tokenClass == WRITE ||
        parser->lexer.tokenClass == WHILE ||
        parser->lexer.tokenClass == IF ||
        parser->lexer.tokenClass == FOR ||
        parser->lexer.tokenClass == ID ||
        parser->lexer.tokenClass == BEGIN) {
        _cmd(parser);
        if (parser->lexer.tokenClass == SEMICOLON) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
            _comandos(parser);
        } else {
            _error(parser, SEMICOLON);
        }
    }
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
 * @param Parser initialized parser instance
 */
void _cmd(Parser* parser) {
    if (parser->lexer.tokenClass == READ) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        if (parser->lexer.tokenClass == OPEN_PAR) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
            _variaveis(parser);
            if (parser->lexer.tokenClass == CLOSE_PAR) {
                parser->errorCount += nextToken(&parser->lexer, parser->output);
            }
        }
    } else if (parser->lexer.tokenClass == WRITE) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        if (parser->lexer.tokenClass == OPEN_PAR) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
            _variaveis(parser);
            if (parser->lexer.tokenClass == CLOSE_PAR) {
                parser->errorCount += nextToken(&parser->lexer, parser->output);
            }
        }
    } else if (parser->lexer.tokenClass == WHILE) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        if (parser->lexer.tokenClass == OPEN_PAR) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
            _condicao(parser);
            if (parser->lexer.tokenClass == CLOSE_PAR) {
                parser->errorCount += nextToken(&parser->lexer, parser->output);
                if (parser->lexer.tokenClass == DO) {
                    parser->errorCount += nextToken(&parser->lexer, parser->output);
                    _cmd(parser);
                }
            }
        }
    } else if (parser->lexer.tokenClass == IF) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        _condicao(parser);
        if (parser->lexer.tokenClass == THEN) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
            _cmd(parser);
            _pfalsa(parser);
        }
    } else if (parser->lexer.tokenClass == FOR) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        if (parser->lexer.tokenClass == ID) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
            if (parser->lexer.tokenClass == ASSIGN) {
                parser->errorCount += nextToken(&parser->lexer, parser->output);
                _expressao(parser);
                if (parser->lexer.tokenClass == TO) {
                    parser->errorCount += nextToken(&parser->lexer, parser->output);
                    _expressao(parser);
                    if (parser->lexer.tokenClass == DO) {
                        parser->errorCount += nextToken(&parser->lexer, parser->output);
                        _cmd(parser);
                    }
                }
            }
        }
    } else if (parser->lexer.tokenClass == ID) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        _pos_ident(parser);
    } else if (parser->lexer.tokenClass == BEGIN) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        _comandos(parser);
        if (parser->lexer.tokenClass == END) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
        }
    } else {
        _error(parser, SEMICOLON);
    }
}

/**
 * @brief Implements rule 20.1 of the grammar:
 * <pos_ident> ::= := <expressao> | <lista_arg>
 * @param Parser initialized parser instance
 */
void _pos_ident(Parser* parser) {
    if (parser->lexer.tokenClass == ASSIGN) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        _expressao(parser);
    } else {
        _lista_arg(parser);
    }
}

/**
 * @brief Implements rule 21 of the grammar:
 * <condicao> ::= <expressao> <relacao> <expressao>
 * @param Parser initialized parser instance
 */
void _condicao(Parser* parser) {
    _expressao(parser);
    _relacao(parser);
    _expressao(parser);
}

/**
 * @brief Implements rule 22 of the grammar:
 * <relacao> ::= = | <> | >= | <= | > | <
 * @param Parser initialized parser instance
 */
void _relacao(Parser* parser) {
    if (parser->lexer.tokenClass == RELATION) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        _error(parser, RELATION);
    }
}

/**
 * @brief Implements rule 23 of the grammar:
 * <expressao> ::= <termo> <outros_termos>
 * @param Parser initialized parser instance
 */
void _expressao(Parser* parser) {
    _termo(parser);
    _outros_termos(parser);
}

/**
 * @brief Implements rule 24 of the grammar:
 * <op_un> ::= + | - | lambda
 * @param Parser initialized parser instance
 */
void _op_un(Parser* parser) {
    if (parser->lexer.tokenClass == OP_UN) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    }
}

/**
 * @brief Implements rule 25 of the grammar:
 * <outros_termos> ::= <op_ad> <termo> <outros_termos> | lambda
 * @param Parser initialized parser instance
 */
void _outros_termos(Parser* parser) {
    if (parser->lexer.tokenClass == OP_ADD) {
        _op_ad(parser);
        _termo(parser);
        _outros_termos(parser);
    }
}

/**
 * @brief Implements rule 26 of the grammar:
 * <op_ad> ::= + | -
 * @param Parser initialized parser instance
 */
void _op_ad(Parser* parser) {
    if (parser->lexer.tokenClass == OP_ADD) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        _error(parser, OP_ADD);
    }
}

/**
 * @brief Implements rule 27 of the grammar:
 * <termo> ::= <op_un> <fator> <mais_fatores>
 * @param Parser initialized parser instance
 */
void _termo(Parser* parser) {
    _op_un(parser);
    _fator(parser);
    _mais_fatores(parser);
}

/**
 * @brief Implements rule 28 of the grammar:
 * <mais_fatores> ::= <op_mul> <fator> <mais_fatores> | λ
 * @param Parser initialized parser instance
 */
void _mais_fatores(Parser* parser) {
    if (parser->lexer.tokenClass == OP_MULT) {
        _op_mul(parser);
        _fator(parser);
        _mais_fatores(parser);
    }
}

/**
 * @brief Implements rule 29 of the grammar:
 * <op_mul> ::= *|/
 * @param Parser initialized parser instance
 */
void _op_mul(Parser* parser) {
    if (parser->lexer.tokenClass == OP_MULT) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        _error(parser, OP_MULT);
    }
}

/**
 * @brief Implements rule 30 of the grammar:
 * <fator> ::= ident | <numero> | (<expressao>)
 * @param Parser initialized parser instance
 */
void _fator(Parser* parser) {
    if (parser->lexer.tokenClass == ID) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else if (parser->lexer.tokenClass == OPEN_PAR) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
        _expressao(parser);
        if (parser->lexer.tokenClass == CLOSE_PAR) {
            parser->errorCount += nextToken(&parser->lexer, parser->output);
        } else {
            _error(parser, CLOSE_PAR);
        }
    } else {
        _numero(parser);
    }
}

/**
 * @brief Implements rule 31 of the grammar:
 * <numero> ::= numero_int | numero_real
 * @param Parser initialized parser instance
 */
void _numero(Parser* parser) {
    if (parser->lexer.tokenClass == N_INTEGER || parser->lexer.tokenClass == N_REAL) {
        parser->errorCount += nextToken(&parser->lexer, parser->output);
    } else {
        parser->errorCount++;
        printf("Parser error on line %d col %d: expected N_INTEGER or N_REAL but found %s\n", parser->lexer.currLine, lexerCurrColWithoutRetreat(&parser->lexer), parser->lexer.buffer.str);
    }
}

void _error(Parser* parser, int expectedTokenClass) {
    parser->errorCount++;

    String errorMsg;
    stringInit(&errorMsg);
    appendStr(&errorMsg, "Parser error on line ");
    appendInt(&errorMsg, parser->lexer.currLine);
    appendStr(&errorMsg, " col ");
    appendInt(&errorMsg, lexerCurrColWithoutRetreat(&parser->lexer));
    appendStr(&errorMsg, ": expected ");
    appendStr(&errorMsg, lexerTokenClassName(expectedTokenClass));
    appendStr(&errorMsg, " but found ");
    appendStr(&errorMsg, parser->lexer.buffer.str);
    append(&errorMsg, '\n');
    printf("%s", errorMsg.str);
    fprintf(parser->output, "%s", errorMsg.str);

    stringDestroy(&errorMsg);
}