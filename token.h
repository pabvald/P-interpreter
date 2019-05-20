/**
 * @Author: Dušan Kolář
 * @Year:   2003-2018
 * Copyright (c) 2018
 * Licence: GLP 3.0
 */

#ifndef ___TOKEN_H___
#define ___TOKEN_H___

#define yyFlag(x)    x.s.flag
#define yyFloat(x)   x.s.u.vFloat
#define yyStr(x)     x.s.u.vStr
#define yyAST(x)     x.s.u.ast

#define fIDENT  1
#define fFLOAT  2
#define fSTR    3
#define fAST    4

#endif
