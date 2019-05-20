
%{
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "pcc.h"
//#include "token.h"
//#include "astree.h"

#define KWLEN 14

/* Keywords: variable declaration, input/ouput functions names and 
 * predefined functions names */
char *keywords[KWLEN] = {
    "write",
    "read",
    "var",
    "cos",
    "sin",
    "tan",
    "asin",
    "acos",
    "atan",
    "log",
    "log10",
    "exp",
    "ceil",
    "floor",
};

/* Codes used in Yacc to refer to the keywords */
unsigned keycodes[KWLEN] = {
    WRITE,
    READ,
    VAR,
    COS,
    SIN,
    TAN,
    ASIN,
    ACOS,
    ATAN,
    LOG,
    LOG10,
    EXPOP,
    CEIL,
    FLOOR,
}

/* Line counter */
int	nlin = 1;

/* Functions declarations */
static void toLowerCase(char *s);
static char *readStr(void);
static void addStr(char **, unsigned long, char);
static char *readChar(void);


int yywrap(void) {
    return 1;
}

%}

LETTER ([a-zA-Z])

DIGIT   ([0-9])
DIGITS  ({DIGIT}+)

EXP     ([eE][-+]?{DIGITS})

FLOAT1  ({DIGITS)"."{DIGITS})
FLOAT2  ({DIGITS}{EXP})
FLOAT3  ({DIGITS}"."{DIGITS}{EXP})

FLOAT   ({FLOAT1}|{FLOAT2}|{FLOAT3})

ID      ((_|{LETTER})(_|{LETTER}|{DIGIT})*)

NL  ([\n])

SPC     ([ \t\f\r])
SPCS    ({SPC}+)

OP1     ([-+*/=<>%!^()])
OP2     ("=="|"!="|">="|"<="|"&&"|"||")

CHARSTART       (['])
STRSTART        (["])
SLCOMMENTSTART  ("//")
MLCOMMENTSTART  ("/*")

%s  MLCOMMENT
%%

 /* White space, nothing to do */
{SPC}                   ;           


 /* Single line comment - ignore line */
{SLCOMMENTSTART}.*\n    { ++nlin; } 


 /* Multiline comment - ignore input until the comment is closed with */
{MLCOMMENTSTART}        { BEGIN MLCOMMENT; } 
<MLCOMMENT>"*/"         { BEGIN 0;}
<MLCOMMENT>[\n]         { ++nlin; }
<MLCOMMENT>.            ; 


 /* Identifiers and keywords - Check if it is a keyword and return it */
{ID}                    {
                            unsigned i = 0;
                            int r=-1;
                            char *res;
                            
                            toLowerCase(yytext);

                            while (i<KWLEN && r<0) {
                                if ((r=strcmp(keywords[i],yytext))==0){
                                    return keycodes[i];
                                } 
                                ++i;
                            }

                            yyStr(yylval)= strdup(yytext);
                            yyFlag(yylval)=fID;

                            return ID;
                        }

 /* Newline - increase the line counter */
{NL}                {
                        ++nlin;
                        return yytext[0];
                    }

 /* Numerical contants - Convert it into float and return it */
{DIGITS}            {
                        long int li;
                        sscanf(yytext,"%ld",&li);
                        yyFloat(yylval) = (double)li;
                        yyFlag(yylval) = fFLOAT;
                        
                        return FLOAT;
                    }       

{FLOAT}             {
                        sscanf(yytext,"%lf",&( yyFloat(yylval) ));
                        yyFlag(yylval) = fFLOAT;
                        
                        return FLOAT;
                    }

 /* String - Read string and return it */
{STRSTART}          {
                      yyStr(yylval) = readStr();
                      yyFlag(yylval) = fSTR;
                      
                      return STR;
                    }

 /* Char - Read char and return it */
{CHARSTART}         {
                      
                      yyStr(yylval) = readChar();
                      yyFlag(yylval) = fCHAR
                      return CHAR;
                    }

 /* One-symbol operators - Return it */
{OP1}               return yytext[0];

 /* Two-symbol operators - Check which one of them is and return it */
{OP2}               {
                        switch (yytext[0]) {
                            case '=':
                                return EQ;
                            case '!':
                                return NE;
                            case '<':
                                return LE;
                            case '>':
                                return GE;
                            case '&':
                                return AND;
                            case '|':
                                return OR;
                        }
                    }


 /* Other - Error */
.                   {
                        printf("Unexpected character in input: %c [Line %d]\n",yytext[0], nlin);
                        exit(1);
                    }

%%

/**
* Modify a string, converting it to uppercase using the function 
* 'int tolower(int *c)'.
* @param str - string to be converted.
*
* @author Pablo Valdunciel Sánchez
*/
static void toLowerCase(char *str) {
    for(int i = 0; str[i]; i++){
        str[i] = tolower(str[i]);
    }
}

/**
 * Read next character of the input. 
 * @return string cotaining one single character.
 */
static char *readChar(void) {
    int c, d;
    char *buff;
    unsigned long len = 1;

    buff = malloc(2);
    buff[0] = '\0';
    
       c = input();
       
       if (c == "'") return buff; //End of char 

       d = input();
       if (d != "'") {
           printf("Invalid length for char literal [Line %d]\n", nlin);
           exit(1);
       }

       addStr(&buff,&len,c);
}

/**
 * Read the input until the character " (end of string) is found or until 
 * an unexpected symbol is found.
 * @return string literal 
 */
static char *readStr(void) {
  int c;
  char *buff;
  unsigned long len = 256;

  buff = malloc(257);
  buff[0]='\0';

  do {
    c = input();

    if (c < ' ') {
        printf("Unexpected symbol in string literal [Line %d]\n", nlin);
        exit(1);
    }
    if (c == '"') break; // End of string 

    if (c=='\\') {
      c == input();
      if (c!='\\' && c !='"') {
        unput(c);
        c = '\\';
      }
    }

    addStr(&buff,&len,c);
    
  } while (1);

  return buff;
}

/**
 * Allocate string dynamically
 * @param s - part of the string already allocated.
 * @param len - length of the current allocated memory
 * @para c - next character of the string 
 *
 */
static void addStr(char **s, unsigned long *len, char c) {
  char buf[2];
  buf[0] = c;
  buf[1] = '\0';

  if (strlen(*s) >= *len) {
    char *ss;
    ss = malloc(*len+1025);
    strcpy(ss,*s);
    free(*s);
    *s = ss;
    *len = *len+1024;
  }
  
  strcat(*s,buf);
}

