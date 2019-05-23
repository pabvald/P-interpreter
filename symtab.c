/**
 * @Author: Dušan Kolář
 * @Year:   2003-2018
 * Copyright (c) 2018
 * Licence: GLP 3.0
 */

#include "symtab.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern int yylineno;


typedef struct symtab {
    char   *name;    
    double  dVal;
    struct symtab *left, *right;
} symtab_t;

static symtab_t *root = NULL;

static char *wName;
static double wDvalue;

/**
 * Insert a new variable in the syntactic table.
 * @param nd - node to be examined
 */
static void inmod(symtab_t *nd) {
    int res = strcmp(wName,nd->name);
    if (res == 0) { // the same string       
       
        nd->dVal = wDvalue;      
        
        return;
    }
    if (res < 0) {
        if (nd->left != NULL) {
            inmod(nd->left);
        } else {
            symtab_t *newr = xmalloc(sizeof(symtab_t));            
            newr->dVal = wDvalue;                     
            newr->name  = sdup(wName);
            newr->left  = NULL;
            newr->right = NULL;
            nd->left    = newr;
        }
    } else {
        if (nd->right != NULL) {
            inmod(nd->right);
        } else {
            symtab_t *newr = xmalloc(sizeof(symtab_t));           
            newr->dVal = wDvalue;            
            newr->name  = sdup(wName);
            newr->left  = NULL;
            newr->right = NULL;
            nd->right   = newr;
        }
    }
}

/**
 * Insert a new numerical variable or modify the value of an
 * existing one.
 * @param s - variable's name 
 * @param val - numerical value
 */
void insertModifyD(char *s, double val) {
    if (root == NULL) {
        root = xmalloc(sizeof(symtab_t));
        root->dVal = val;
        root->name  = sdup(s);
        root->left = NULL;
        root->right = NULL;
        return;
    }
    wName = s;
    wDvalue = val;
    inmod(root);
}


/**
 * Get the the value of a numerical variable.
 * @param nd - node that contains the variable.
 */
static double getD(symtab_t *nd) {
    if (nd == NULL) {
        prError(yylineno,"Undefined variable %s!\n",wName);
    }
    
    int res = strcmp(wName,nd->name);
    if (res == 0) { // the same string
        return nd->dVal;
    }
    
    if (res < 0) {
        return getD(nd->left);
    } else {
        return getD(nd->right);
    }
}


/**
 * Get the value of a numerical variable.
 * @param s - variable's name. 
 */
double readD(char *s) {
    wName = s;
    return getD(root);
}


