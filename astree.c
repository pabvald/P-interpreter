/**
 * @Author: Dušan Kolář
 * @Year:   2003-2018
 * Copyright (c) 2018
 * Licence: GLP 3.0
 */

#include "astree.h"
#include "utils.h"
#include "pcc.h"
#include "symtab.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

extern int yylineno;

/*-------------------------------------------------------- BUILDING ---------------------------------------------------------*/
/**
 * Creates a leaf with string value and the given tag.
 * @param tag 
 * @pram str - string value of the leaf
 */
ast_t *mkSlf(unsigned tag, char *str) {
    ast_t *res = xmalloc(sizeof(ast_t));
    lnum(res) = (unsigned)yylineno;
    tag(res) = tag;
    sv(res) = str;
    return res;
}

/**
 * Creates a leaf with double value and the given tag.
 * @param tag
 * @param dval - double value
 */
ast_t *mkDlf(unsigned tag, double dval) {
    ast_t *res = xmalloc(sizeof(ast_t));
    lnum(res) = (unsigned)yylineno;
    tag(res) = tag;
    dv(res) = dval;
    return res;
}

/**
 * Creates an inner node with left and right sub-tree and the given tag.
 * @param tag
 * @param l - left subtree
 * @param r - right subtree
 */
ast_t *mkNd(unsigned tag, ast_t *l, ast_t *r) {
    ast_t *res = xmalloc(sizeof(ast_t));
    lnum(res) = (unsigned)yylineno;
    tag(res) = tag;
    left(res) = l;
    right(res) = r;
    return res;
}


/**
 * Appends node to the right (end) of the list of nodes.
 * @param tag
 * @param lst - list of nodes
 * @param nd - node 
 */
ast_t *appR(unsigned tag, ast_t *lst, ast_t *nd) {
    if (lst == NULL) {
        if (nd == NULL) {
            return NULL;
        }
        return mkNd(tag,nd,NULL);
    }
    if (nd == NULL) {
        return lst;
    }

    ast_t *tmp = lst;
    while (right(tmp) != NULL) {
        tmp = right(tmp);
    }
    right(tmp) = mkNd(tag,nd,NULL);

    return lst;
}

/*--------------------------------------------------------- EVALUATION ---------------------------------------------------*/

/**
 * Evaluate an expression.
 * @return value of the expression
 */
static double expr(ast_t *root) {
    switch (tag(root)) {
        case OR:
            if (expr(left(root)) != 0.0 || expr(right(root)) != 0.0) {
                return 1.0;
            } else {
                return 0.0;
            }
        case AND:
            if (expr(left(root)) != 0.0 && expr(right(root)) != 0.0) {
                return 1.0;
            } else {
                return 0.0;
            }
        case EQ:
            if (expr(left(root)) == expr(right(root))) {
                return 1.0;
            } else {
                return 0.0;
            }
        case NE:
            if (expr(left(root)) != expr(right(root))) {
                return 1.0;
            } else {
                return 0.0;
            }
        case LE:
            if (expr(left(root)) <= expr(right(root))) {
                return 1.0;
            } else {
                return 0.0;
            }
        case GE:
            if (expr(left(root)) >= expr(right(root))) {
                return 1.0;
            } else {
                return 0.0;
            }
        case '<':
            if (expr(left(root)) < expr(right(root))) {
                return 1.0;
            } else {
                return 0.0;
            }
        case '>':
            if (expr(left(root)) > expr(right(root))) {
                return 1.0;
            } else {
                return 0.0;
            }
        case '+':
            return expr(left(root)) + expr(right(root));
        case '-':
            if (left(root) == NULL) {
                return - expr(right(root));
            } else {
                return expr(left(root)) - expr(right(root));
            }
        case '*':
            return expr(left(root)) * expr(right(root));
        case '/':
            return expr(left(root)) / expr(right(root));
        case '^':
            return pow( expr(left(root)), expr(right(root)) );
        case '%':
            return fmod( expr(left(root)), expr(right(root)) );
        case '!':
            if (expr( right(root) ) == 0.0) {
                return 1.0;
            } else {
                return 0.0;
            }
        case FLOAT:
            return dv(root);
        case IDENT:
            return readD( sv(root) );
        case SIN:
            return sin( expr(left(root)) );
        case ASIN:
            return asin( expr(left(root)) );
        case COS:
            return cos( expr(left(root)) );
        case ACOS:
            return acos( expr(left(root)) );
        case TAN:
            return tan( expr(left(root)) );
        case ATAN:
            return atan( expr(left(root)) );
        case EXP:
            return exp( expr(left(root)) );
        case LOG:
            return log( expr(left(root)) );
        case LOG10:
            return log10( expr(left(root)) );
        case CEIL:
            return ceil( expr(left(root)) );
        case FLOOR:
            return floor( expr(left(root)) );
        default:
            prError((unsigned short)lnum(root),"Unknown tag in expr AST %u\n",tag(root),NULL);
            break;
    }
}


/**
 * Process an AST given its root node.
 * @param root - root node
 */
static void proc(ast_t *root) {
    switch (tag(root)) {
        case '=':            
            insertModifyD( sv(left(root)), expr(right(root)) );     
            break;
        case WRITE:
            if (left(root) == NULL) { // write(IDENT)
                printf("%g", expr(right(root)) );

            } else if (right(root) == NULL) { //write(STR) 
                printf("%s", sv(left(root)) );

            } else { // write(STR, IDENT )
                printf("%s%g", sv(left(root)), expr(right(root)) );
            }
            break;
        case READ:
            if (left(root) == NULL) { //read(IDENT)
                double rval;
                scanf("%lf",&rval);
                insertModifyD(sv(right(root)), rval);
            } else { //read(STRING, IDENT)
                double rval; 
                printf("%s", sv(left(root)));
                scanf("%lf", &rval);
                insertModifyD( sv(right(root)), rval);
            }
            break;
        case WHILE:
            {
                double control = expr(left(root));
                while (control) {
                    if (right(root) != NULL) {
                        evaluate(right(root));
                    }                    
                    control = expr(left(root));
                }
            }
            break;
        case IF:
            {
                double control = expr(left(root));
                if (control ) {
                    if(right(root) != NULL) {
                        evaluate(right(root));
                    }
                }
            }
            break;
        case ELSE:
            {
                double control = expr(left(root));
                if (control) {
                    if(left(right(root)) != NULL) {
                        evaluate( left(right(root)));
                    }
                } else {
                    if(right(right(root)) != NULL) {
                        evaluate( right( right(root)) );
                    } 
                }
            }
            break;
        default:
            prError((unsigned short)lnum(root),"Unknown tag in statement AST %u\n",tag(root),NULL);
            break;
    }
}

/**
 * Evaluate the program.
 * @param root - root of the Abstract Syntax Tree.
 */
void evaluate(ast_t *root) {
    while (root != NULL) {
        proc(left(root));
        root = right(root);
    }
}