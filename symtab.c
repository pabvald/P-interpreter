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

extern int yylineno;


typedef struct symtab {
    char   *name;
    double  value;
    struct symtab *left, *right;
} symtab_t;

static symtab_t *root = NULL;

static char *wName;
static double wValue;

// ----------------------------------------------------------------------------

static void inmod(symtab_t *nd) {
    int res = strcmp(wName,nd->name);
    if (res == 0) { // the same string
        nd->value = wValue;
        return;
    }
    if (res < 0) {
        if (nd->left != NULL) {
            inmod(nd->left);
        } else {
            symtab_t *newr = xmalloc(sizeof(symtab_t));
            newr->value = wValue;
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
            newr->value = wValue;
            newr->name  = sdup(wName);
            newr->left  = NULL;
            newr->right = NULL;
            nd->right   = newr;
        }
    }
}

// ----------------------------------------------------------------------------

void insertModify(char *s, double val) {
    if (root == NULL) {
        root = xmalloc(sizeof(symtab_t));
        root->value = val;
        root->name  = sdup(s);
        root->left = NULL;
        root->right = NULL;
        return;
    }
    wName = s;
    wValue = val;
    inmod(root);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

static double get(symtab_t *nd) {
    if (nd == NULL) {
        prError(yylineno,"Undefined variable %s!\n",wName);
    }
    
    int res = strcmp(wName,nd->name);
    if (res == 0) { // the same string
        return nd->value;
    }
    
    if (res < 0) {
        return get(nd->left);
    } else {
        return get(nd->right);
    }
}

// ----------------------------------------------------------------------------

double read(char *s) {
    wName = s;
    return get(root);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// EOF
