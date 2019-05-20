
/**
 * @author Pablo Valdunciel Sánchez 
 * 
 * Based in Dušan Kolář's code.
 *
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "symtab.h"
#include "astree.h"
#include "token.h"

#define IN_PARSER
#include "pcc.h"


int yylex(void);
extern int yylineno;  // Line counter 

FILE *fIn;

int yyerror(char *str);

static ast_t *astRoot = NULL;

%}

%union {
    struct sStackType {
    unsigned char   flag;
        union {
        double         vFloat;
        char          *vStr;
        struct ast_s  *ast;
        }   u;
    }   s;
}


 /* Nonassociative operators: ==, !=, >, >=, <, <= */
%nonassoc EQ NE
%nonassoc LE GE '<' '>'

 /* Left associative operators: logic OR, logic AND, sum, difference, product 
  * division */
%left OR
%left AND

%left '+' '-'
%left '*' '/' '%'

 /* Right associative operators: pow, logic NOT */
%right '^'

%right '!'

%right UNOP


%token <s> IDENT
%term LET PRINT SIN COS TAN ASIN ACOS ATAN LOG LOG10 EXPOP CEIL FLOOR READ WRITE VAR 
%token <s> FLOAT
%token <s> STR

%type <s> expr ternary statement  progelem prog

%%

prog
  : progelem
    {
      astRoot = appR(';', astRoot, $1.u.ast);
    }
  | prog progelem
    {
      astRoot = appR(';', astRoot, $2.u.ast);
    }
  ;

progelem
  : statement '\n'
    {
      $$ = $1;
    }
  | '\n'
    {
      $$.flag = fAST;
      $$.u.ast = NULL;
    }
  ;


statement
  : LET IDENT '=' ternary
    {
      $$.flag = fAST;
      $$.u.ast = mkNd('=', mkSlf(IDENT,$2.u.vStr), $4.u.ast);
    }
  | PRINT ternary
    {
      $$.flag = fAST;
      $$.u.ast = mkNd(PRINT, NULL, $2.u.ast);
    }
  | PRINT STR
    {
      $$.flag = fAST;
      $$.u.ast = mkNd(PRINT, mkSlf(STR,$2.u.vStr), NULL);
    }
  | PRINT STR ternary
    {
      $$.flag = fAST;
      $$.u.ast = mkNd(PRINT, mkSlf(STR,$2.u.vStr), $3.u.ast);
    }
  | READ IDENT
    {
      $$.flag = fAST;
      $$.u.ast = mkNd(READ, NULL, mkSlf(IDENT,$2.u.vStr));
    }
  | READ STR IDENT
    {
      $$.flag = fAST;
      $$.u.ast = mkNd(READ, mkSlf(STR,$2.u.vStr), mkSlf(IDENT,$3.u.vStr));
    }
  ;

ternary
  : ternary '?' ternary ':' ternary
    {
      $$.flag = fAST;
      $$.u.ast = mkNd('?', $1.u.ast, mkNd(':', $3.u.ast, $5.u.ast) );
    }
  | expr
    /* $$ = $1 */
  ;

expr
  : expr OR expr
    {
      $$.flag = fAST;
      $$.u.ast = mkNd(OR, $1.u.ast, $3.u.ast);
    }
  | expr AND expr
    {
      $$.flag = fAST;
      $$.u.ast = mkNd(AND, $1.u.ast, $3.u.ast);
    }
  | expr EQ expr
    {
      $$.flag = fAST;
      $$.u.ast = mkNd(EQ, $1.u.ast, $3.u.ast);
    }
  | expr NE expr
    {
      $$.flag = fAST;
      $$.u.ast = mkNd(NE, $1.u.ast, $3.u.ast);
    }
  | expr LE expr
    {
      $$.flag = fAST;
      $$.u.ast = mkNd(LE, $1.u.ast, $3.u.ast);
    }
  | expr GE expr
    {
      $$.flag = fAST;
      $$.u.ast = mkNd(GE, $1.u.ast, $3.u.ast);
    }
  | expr '<' expr
    {
      $$.flag = fAST;
      $$.u.ast = mkNd('<', $1.u.ast, $3.u.ast);
    }
  | expr '>' expr
    {
      $$.flag = fAST;
      $$.u.ast = mkNd('>', $1.u.ast, $3.u.ast);
    }
  | expr '+' expr
    {
      $$.flag = fAST;
      $$.u.ast = mkNd('+', $1.u.ast, $3.u.ast);
    }
  | expr '-' expr
    {
      $$.flag = fAST;
      $$.u.ast = mkNd('-', $1.u.ast, $3.u.ast);
    }
  | expr '*' expr
    {
      $$.flag = fAST;
      $$.u.ast = mkNd('*', $1.u.ast, $3.u.ast);
    }
  | expr '/' expr
    {
      $$.flag = fAST;
      $$.u.ast = mkNd('/', $1.u.ast, $3.u.ast);
    }
  | expr '^' expr
    {
      $$.flag = fAST;
      $$.u.ast = mkNd('^', $1.u.ast, $3.u.ast);
    }
  | '!' expr
    {
      $$.flag = fAST;
      $$.u.ast = mkNd('!',NULL,$2.u.ast);
    }
  | '+' expr %prec UNOP
    {
      $$ = $2;
    }
  | '-' expr %prec UNOP
    {
      $$.flag = fAST;
      $$.u.ast = mkNd('-',NULL,$2.u.ast);
    }
  | '(' ternary ')'
    {
      $$ = $2;
    }
  | FLOAT
    {
      $$.flag = fAST;
      $$.u.ast = mkDlf(FLOAT,$1.u.vFloat);
    }
  | IDENT
    {
      $$.flag = fAST;
      $$.u.ast = mkSlf(IDENT,$1.u.vStr);
    }
  | SIN '(' ternary ')'
    {
      $$.flag = fAST;
      $$.u.ast = mkNd(SIN,$3.u.ast,NULL);
    }
  | COS '(' ternary ')'
    {
      $$.flag = fAST;
      $$.u.ast = mkNd(COS,$3.u.ast,NULL);
    }
  | TAN '(' ternary ')'
    {
      $$.flag = fAST;
      $$.u.ast = mkNd(TAN,$3.u.ast,NULL);
    }
  ;


%%

/**
 * Print the error and the number of line where it is.
 * @param str - error description
 */
int yyerror(char *str) {
  prError(yylineno,"%s\n",str,NULL);
  return 1;
}

extern FILE *yyin;

/**
 * MAIN PROGRAMM
 */
int main(int argc, char *argv[]) {
  exitOnError();

  // If a filename wasnt' provided, show usage and end programm.
  if (argc!=2) {
    puts("\nUsage: demo <filename>\n");
    fflush(stdout);
    return 1;
  }

  // If the provided file cannot be opened, inform the user and end programm.
  if ((fIn = fopen(argv[1],"rb"))==NULL) {
    fprintf(stderr,"\nCannot open file: %s\n\n",argv[1]);
    fflush(stderr);
    return 1;
  }
  
  yyin = fIn;

  setFilename( argv[1] );

  if (yyparse() != 0) {
    fclose(fIn);
    prError(yylineno,"Parsing aborted due to errors in input\n",NULL);
  }

  fclose(fIn);

  if (astRoot != NULL) {
    evaluate(astRoot);
  } else {
    prError(yylineno,"No parse output provided, aborting evaluation\n",NULL);
  }

  return 0;
}


