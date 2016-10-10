#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "passagens.h"

static const Instrucao instrucoes[] = {
                                        {"ADD", "01", 1},
                                        {"SUB", "02", 1},
                                        {"MUL", "03", 1},
                                        {"DIV", "04", 1},
                                        {"JMP", "05", 1},
                                        {"JMPN", "06", 1},
                                        {"JMPP", "07", 1},
                                        {"JMPZ", "08", 1},
                                        {"COPY", "09", 2},
                                        {"LOAD", "10", 1},
                                        {"STORE", "11", 1},
                                        {"INPUT", "12", 1},
                                        {"OUTPUT", "13", 1},
                                        {"STOP", "14", 0}
                                      };

static const Diretiva diretivas[] = {
                                      {"IF", 1, 0},
                                      {"EQU", 1, 0},
                                      {"SPACE", 1, 1},
                                      {"SPACE", 2, 1},
                                      {"CONST", 2, 1}
                                    };
