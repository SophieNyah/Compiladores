#ifndef PARSER_H
#define PARSER_H

#include<stdbool.h>
#include<stdio.h>
#include"lex_analyser.h"

extern char* ll1_expected;
extern token_type s_token;

bool transition(token_type input_token);

bool S(token_type input_token);
bool E(token_type input_token);
bool E1(token_type input_token);
bool T(token_type input_token);
bool T1(token_type input_token);
bool F(token_type input_token);

bool error();
token_type advance();
bool eat(token_type t);

#endif