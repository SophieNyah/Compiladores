%{

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"ast_symbols.h"
#include"hashTable.h"
#include"commands.h"
extern int yylex();
extern char* yytext;
void yyerror(char *s);

void printLine(FILE* in, int line_number);

extern int colunas;
extern int linhas;
extern int id_linha;
extern int id_coluna;
int g_tipo = 0;
FILE* in_file = NULL;

%}

/* declare types */
%union{
	char* string;
	int integer;
	struct symbol* symbol_union;
	struct array* array_union;
	struct expression* expression_union;
}

%type <string> IDENTIFIER
%type <symbol_union> declaracao_var1
%type <array_union> array
%type <integer> pointer
%type <integer> tipo
%type <integer> number
%type <integer> NUM_INT
%type <integer> NUM_HEXA
%type <integer> NUM_OCTA
%type <expression_union> exp_prim

/* declare tokens */
%token VOID_T
%token INT_T
%token CHAR_T
%token RETURN_T
%token DO_T
%token WHILE_T
%token FOR_T
%token IF_T
%token ELSE_T
%token ADD
%token SUB
%token ASTERISK
%token DIV
%token MOD
%token INC
%token DEC
%token AMPERSAND
%token BIT_OR
%token BIT_NOT
%token BIT_XOR
%token LOG_AND
%token LOG_OR
%token LOG_NOT
%token EQUALS
%token NOT_EQUALS
%token LESS
%token GREAT
%token LEQ
%token GEQ
%token RSHIFT
%token LSHIFT
%token ASSIGN
%token ADD_ASSIGN
%token SUB_ASSIGN
%token SEMICOLON
%token COMMA
%token COLON
%token LPAREN
%token RPAREN
%token LCBRACK
%token RCBRACK
%token LBRACK
%token RBRACK
%token QUEST_MARK
%token NUMBER_SIGN
%token DEFINE
%token PRINTF_T
%token SCANF_T
%token EXIT_T
%token NUM_HEXA
%token NUM_OCTA
%token NUM_INT
%token CHARACTER
%token STRING
%token IDENTIFIER
%token LINE_COMMENT
%token BLOCK_COMMENT
%token UNTERMINATED_COMMENT
%token UNKNOWN_CHAR
%token END_OF_FILE

	/* unused tokens */
%token BREAK_T
%token SWITCH_T
%token CASE_T
%token DEFAULT_T
%token TYPEDEF_T
%token STRUCT_T
%token POINTER_DEFERENCE

%start programa

%%

programa: programa1 programa END_OF_FILE	{ printf("SUCCESSFUL COMPILATION."); return 0; }
		| programa1 END_OF_FILE				{ printf("SUCCESSFUL COMPILATION."); return 0; }
;
programa1: declaracoes	{}
		 | funcao		{}
;

declaracoes: NUMBER_SIGN DEFINE IDENTIFIER expressao	{}
		   | declaracao_var		{}
		   | declaracao_prot	{}
;

declaracao_var: tipo declaracao_var1 SEMICOLON	{}
;
declaracao_var1:
	pointer IDENTIFIER array ASSIGN exp_atr declaracao_var_fim	{
		Symbol *aux = malloc(sizeof(Symbol));

		aux->symbol_type = TIPOS_VARIAVEL;
		aux->type.pointers = $1;
		aux->id = $2;
		aux->type.type = g_tipo;
		aux->var.v.array = $3;
		aux->var.v.constant = false;
		aux->var.v.line = id_linha;
		aux->var.v.column = id_coluna;

		insereRegistro(Program_Table.Global_Symbol_Table, aux->id, aux);
	}
	| pointer IDENTIFIER array declaracao_var_fim				{
		Symbol *aux = malloc(sizeof(Symbol));

		aux->symbol_type = TIPOS_VARIAVEL;
		aux->type.pointers = $1;
		aux->id = $2;
		aux->type.type = g_tipo;
		aux->var.v.array = $3;
		aux->var.v.constant = false;
		aux->var.v.line = id_linha;
		aux->var.v.column = id_coluna;

		insereRegistro(Program_Table.Global_Symbol_Table, aux->id, aux);
	}
;
declaracao_var_fim: COMMA declaracao_var1	{}
				  | /* epsilon */			{}
;

declaracao_prot: tipo pointer IDENTIFIER parametros SEMICOLON	{}
;

funcao: tipo pointer IDENTIFIER parametros LCBRACK func_dec_var comandos RCBRACK	{}
;
func_dec_var: declaracao_var func_dec_var	{}
			| /* epsilon */					{}
;

parametros: LPAREN parametros1 RPAREN	{}
;
parametros1: tipo pointer IDENTIFIER array parametros_fim	{}
		   | /* epsilon */		{}
;
parametros_fim: COMMA parametros1	{}
			  | /* epsilon */		{}
;

bloco: LCBRACK comandos RCBRACK	{}
;

comandos: lista_comandos comandos	{}
		| lista_comandos			{}
;

lista_comandos: DO_T bloco WHILE_T LPAREN expressao RPAREN SEMICOLON	{}
			  | IF_T LPAREN expressao RPAREN bloco else_exp				{}
			  | WHILE_T LPAREN expressao RPAREN bloco					{}
			  | FOR_T LPAREN opt_exp SEMICOLON opt_exp SEMICOLON opt_exp RPAREN bloco	{}
			  | PRINTF_T LPAREN STRING printf_exp RPAREN SEMICOLON		{}
			  | SCANF_T LPAREN STRING COMMA AMPERSAND IDENTIFIER RPAREN SEMICOLON		{}
			  | EXIT_T LPAREN expressao RPAREN SEMICOLON				{}
			  | RETURN_T opt_exp SEMICOLON								{}
			  | expressao SEMICOLON										{}
			  | SEMICOLON												{}
			  | bloco													{}
;
printf_exp: COMMA expressao	{}
		  | /* epsilon */	{}
;
else_exp: ELSE_T bloco		{}
		| /* epsilon */		{}
;
opt_exp: expressao		{}
	   | /* epsilon */	{}
;

expressao: exp_atr					{}
		 | exp_atr COMMA expressao	{}
;

exp_atr: exp_cond						{}
	   | exp_unary ASSIGN exp_atr		{}
	   | exp_unary ADD_ASSIGN exp_atr	{}
	   | exp_unary SUB_ASSIGN exp_atr	{}
;

exp_cond: exp_log_or										{}
		| exp_log_or QUEST_MARK expressao COLON exp_cond	{}
;

exp_log_or: exp_log_and						{}
		  | exp_log_and LOG_OR exp_log_or	{}
;

exp_log_and: exp_or						{}
		   | exp_or LOG_AND exp_log_and	{}
;

exp_or: exp_xor					{}
	  | exp_xor BIT_OR exp_or	{}
;

exp_xor: exp_and					{}
	   | exp_and BIT_XOR exp_xor	{}
;

exp_and: exp_equal					{}
	   | exp_equal AMPERSAND exp_and	{}
;

exp_equal: exp_relat						{}
		 | exp_relat EQUALS exp_equal		{}
		 | exp_relat NOT_EQUALS exp_equal	{}
;

exp_relat: exp_shift					{}
		 | exp_shift LESS exp_relat		{}
		 | exp_shift LEQ exp_relat		{}
		 | exp_shift GEQ exp_relat		{}
		 | exp_shift GREAT exp_relat	{}
;

exp_shift: exp_add					{}
		 | exp_add LSHIFT exp_shift	{}
		 | exp_add RSHIFT exp_shift	{}
;

exp_add: exp_mult				{}
	   | exp_mult ADD exp_add	{}
	   | exp_mult SUB exp_add	{}
;

exp_mult: exp_cast						{}
		| exp_cast ASTERISK exp_mult	{}
		| exp_cast DIV exp_mult			{}
		| exp_cast MOD exp_mult			{}
;

exp_cast: exp_unary								{}
		| LPAREN tipo pointer RPAREN exp_cast	{}
;

exp_unary: exp_postfix			{}
		 | INC exp_unary		{}
		 | DEC exp_unary		{}
		 | AMPERSAND exp_cast	{}
		 | ASTERISK exp_cast	{}
		 | ADD exp_cast			{}
		 | SUB exp_cast			{}
		 | BIT_NOT exp_cast		{}
		 | LOG_NOT exp_cast		{}
;

exp_postfix: exp_prim				{}
		   | exp_postfix INC		{}
		   | exp_postfix DEC		{}
		   | exp_postfix LBRACK expressao RBRACK			{}
		   | exp_postfix LPAREN RPAREN						{}
		   | exp_postfix LPAREN exp_atr exp_postfix1 RPAREN	{}
;
exp_postfix1: COMMA exp_atr exp_postfix1	{}
			| /* epsilon */					{}
;

exp_prim: IDENTIFIER	{ $$ = expressionNew(IDENTIFIER, 0, NULL, NULL); }
		| number		{}
		| CHARACTER		{}
		| STRING		{}
		| LPAREN expressao RPAREN	{}
;

number: NUM_INT		{ $$ = $1; }
	  | NUM_HEXA	{ $$ = $1; }
	  | NUM_OCTA	{ $$ = $1; }
;

tipo: INT_T		{ g_tipo = TIPOS_INT; }
	| CHAR_T	{ g_tipo = TIPOS_CHAR; }
	| VOID_T	{ g_tipo = TIPOS_VOID; }
;

pointer: ASTERISK pointer	{ $$ = $2 + 1; }
	   | /* epsilon */		{ $$ = 0; }
;
array: LBRACK expressao RBRACK array	{
			struct array *aux = malloc(sizeof(struct array));
			// aux->length = $2; // aux->length = solveConstantExpression($2);
			aux->next = $4;

			$$ = aux;
		}
	 | /* epsilon */					{ $$ = NULL; }
;

%%

void yyerror(char *s)
{
	switch(yychar){
		case UNTERMINATED_COMMENT:
			printf("error:lexical:%d:%d: unterminated comment", linhas, colunas);
			break;
		case UNKNOWN_CHAR:
			colunas -= strlen(yytext);
			printf("error:lexical:%d:%d: %s", linhas, colunas, yytext);
			break;
		case END_OF_FILE:
			printf("error:syntax:%d:%d: expected declaration or statement at end of input\n", linhas, colunas);
			printLine(in_file, linhas);
			for(int i=1;i<colunas;i++)
				{ printf(" "); }
			printf("^");
			break;
		default:
			colunas -= strlen(yytext);
			printf("error:syntax:%d:%d: %s\n", linhas, colunas, yytext);
			printLine(in_file, linhas);
			for(int i=1;i<colunas;i++)
				{ printf(" "); }
			printf("^");
			break;
	}
}

int main(int argc, char **argv)
{
	Program_Table.Global_Symbol_Table = criaTabela(211);
	Symbol *aux = malloc(sizeof(Symbol));
	aux->symbol_type = TIPOS_VARIAVEL;
	aux->id = malloc(7 * sizeof(char));
	strcpy(aux->id, "number");
	aux->type.type = TIPOS_INT;
	aux->type.pointers = 0;
	aux->var.v.array = NULL;
	aux->var.v.constant = true;
	aux->var.v.value.i = 15;
	aux->var.v.line = 5;
	aux->var.v.column = 15;

	insereRegistro(Program_Table.Global_Symbol_Table, aux->id, aux);

	in_file = stdin;
	yyparse();

	aux = getPrimeiroRegistro(Program_Table.Global_Symbol_Table, "nuuuum");
	printSymbol(*aux);
	aux = getPrimeiroRegistro(Program_Table.Global_Symbol_Table, "a");
	printSymbol(*aux);
	hashtableFinalizar(Program_Table.Global_Symbol_Table);

    return 0;
}

void printLine(FILE* in, int n){
	int i=1;
	char c;
	fseek(in, 0, SEEK_SET);

	while(i < n){
		c = fgetc(in);
		if(c == '\n')
			{ i++; }
		if(c == EOF)
			{ break; }
	}

	c = fgetc(in);
	while(c!='\n' && c!=EOF){
		printf("%c", c);
		c = fgetc(in);
	}
	printf("\n");
}
