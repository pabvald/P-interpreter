
%{
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "pcc.h"
#include "token.h"
#include "astree.h"

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
};

static void lower(char *s);
static char *readStr(void);
static char *readChar(void);
static void addStr(char **s, unsigned long *len, char c);

int yywrap(void) { 
    return 1; 
}

%}

LETTER    ([a-zA-Z])

DIGIT     ([0-9])
DIGITS    ({DIGIT}+)

EXP       ([eE][-+]?{DIGITS})

FLOAT1    ({DIGITS}"."{DIGITS})
FLOAT2    ({DIGITS}{EXP})
FLOAT3    ({DIGITS}"."{DIGITS}{EXP})

FLOAT     ({FLOAT1}|{FLOAT2}|{FLOAT3})

IDENT     ((_|{LETTER})(_|{LETTER}|{DIGIT})*)

COMSEP    ([\n])

WSPC      ([ \t\f\r])
WSPCS     ({WSPC}+)


OP1       ([-+*/=<>()%!^])
OP2       ("=="|"!="|"<="|">="|"&&"|"||")

CHARSTART       (['])
STRSTART        (["])
SLCOMMENTSTART  ("//")
MLCOMMENTSTART  ("/*")

%s  MLCOMMENT
%%

 /* White space, nothing to do */
{WSPC}              ; 


 /* Single line comment - ignore line */
{SLCOMMENTSTART}.*\n    { ++yylineno; } 


 /* Multiline comment - ignore input until the comment is closed. Increase 
  * the line counter when a newline is found */
{MLCOMMENTSTART}        { BEGIN MLCOMMENT; } 
<MLCOMMENT>"*/"         { BEGIN 0;}
<MLCOMMENT>[\n]         { ++yylineno; }
<MLCOMMENT>.            ; 


 /* Identifiers and keywords - Check if it is a keyword and return it */
{IDENT}             {
                        unsigned i = 0;
                        int r=-1;
                        char *res;
                        
                        lower(yytext);

                        while (i<KWLEN && r<0) {
                            if ((r=strcmp(keywords[i],yytext))==0) {
                                return keycodes[i];
                            }
                            ++i;
                        }

                        yyStr(yylval)= sdup(yytext);
                        yyFlag(yylval)= fIDENT;

                        return IDENT;
                    }

 /* Newline - increase the line counter */
{COMSEP}            {
                        ++yylineno;
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
  /* Char - Read char and return it. Chars are saved as strings */
{CHARSTART}         {
                      yyStr(yylval) = readChar();
                      yyFlag(yylval) = fSTR;
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

 /* Other - Lexical error */                 
.                   {
                        prError(yylineno,"Unexpected character in input: %c [%d]\n",yytext[0],yytext[0],NULL);
                    }

%%

/**
* Convert an string to lowercase.
* @param s - string to be converted.
*
*/
static void lower(char *s) {
    unsigned l = strlen(s);
    while (l>0) {
        --l;
        if (s[l]>='A' && s[l]<='Z') s[l] = s[l]+'a'-'A';
    }
}

/**
 * Append a charecter to a stored string. Allocate more memory if needed.
 * @param s - part of the string already allocated.
 * @param len - length of the current allocated memory
 * @para c - next character of the string 
 *
 */
static void addStr(char **s, unsigned long *len, char c) {
  char buf[2];
  buf[0] = c;
  buf[1] = '\0';

  if (strlen(*s) >= *len) { // Not enough allocated memory 
    char *ss;
    ss = xmalloc(*len + 1025);
    strcpy(ss,*s);
    xfree(*s);
    *s = ss;
    *len = *len+1024;
  }
  
  strcat(*s,buf);
}

/**
 * Read the input until the character " (end of string) is found or until 
 * an unexpected symbol is found.
 * @return string literal 
 */
static char *readStr(void) {
  int c;
  char *str;
  unsigned long len = 256;

  str = xmalloc(257); // Allocate memory 

  str[0]='\0';  //The string is iniatially empty

  do {
    c = input();

    if (c < ' ') {
      prError(yylineno,"Unexpected symbol in string literal [%d]\n",c,NULL);
    } 

    if (c == '"') break; // End of string 

    if (c=='\\') { // Possible escaped character
      c == input();
      if (c!='\\' && c !='"') {
        unput(c);
        c = '\\';
      }
    }

    addStr(&str,&len,c);
    
  } while (1);

  return str;
}

/*
 * Read the character constant.
 */
static char *readChar(void) {
  int c, d;
  char *str;
  unsigned long len = 1;

  str = xmalloc(2);
  
  str[0] = '\0';

  c = input();

  if (c < ' ') {
      prError(yylineno,"Unexpected symbol in char literal [%d]\n",c,NULL);
  }

  if (c == '\'') return str;

  if (c == '\\') {
    d = input();
    if (d !='\\' ) {
        addStr(&str,&len,c);
        addStr(&str,&len,d);
    }
  }

  return str;
}