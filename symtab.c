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
    char   *sVal;
   
    int numerical;
    struct symtab *left, *right;
} symtab_t;

static symtab_t *root = NULL;

static char *wName;
static int wNumerical;
static double wDvalue;
static char* wSvalue;

/**
 * Insert a new variable in the syntactic table.
 * @param nd - node to be examined
 */
static void inmod(symtab_t *nd) {
    int res = strcmp(wName,nd->name);
    if (res == 0) { // the same string
       
        if (wNumerical) {
            nd->dVal = wDvalue;
        } else {
            nd->sVal = wSvalue;
        }
        nd->numerical = wNumerical;
        
        return;
    }
    if (res < 0) {
        if (nd->left != NULL) {
            inmod(nd->left);
        } else {
            symtab_t *newr = xmalloc(sizeof(symtab_t));
            newr->numerical = wNumerical;
            if (wNumerical) {
                newr->dVal = wDvalue;
            } else {
                newr->sVal = sdup(wSvalue);
            }            
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
            newr->numerical = wNumerical;
            if (wNumerical) {
                newr->dVal = wDvalue;
            } else {
                newr->sVal = wSvalue;
            }  
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
        root->numerical = 1;
        root->dVal = val;
        root->name  = sdup(s);
        root->left = NULL;
        root->right = NULL;
        return;
    }
    wName = s;
    wNumerical = 1;
    wDvalue = val;
    inmod(root);
}

/**
 * Insert a new string variable or modify the value of an existing one. 
 * @param s - variables's name
 * @param string 
 */
void insertModifyS(char *s, char *string) {
    if (root == NULL) {
        root = xmalloc(sizeof(symtab_t));
        root->numerical = 0;
        root->sVal = sdup(string);
        root->name  = sdup(s);
        root->left = NULL;
        root->right = NULL;
        return;
    }
    wName = s;
    wNumerical = 0;
    wSvalue = string;
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
        if (nd->numerical != 1) {
            prError(yylineno,"%s is a string variable!\n",wName);
        }
        return nd->dVal;
    }
    
    if (res < 0) {
        return getD(nd->left);
    } else {
        return getD(nd->right);
    }
}

/**
 * Get the the value of a variable as a string.
 * @param nd - node that contains the variable.
 */
static char *getS(symtab_t *nd) {
    if (nd == NULL) {
        prError(yylineno,"Undefined variable %s!\n",wName);
    }
    
    int res = strcmp(wName,nd->name);
    if (res == 0) { // the same string
        if (nd->numerical == 1) {
            char dToString[50];
            snprintf(dToString, 50, "%f", nd->dVal);
            return dToString;
        } else {
            return nd->sVal;
        }
        
    }
    
    if (res < 0) {
        return getS(nd->left);
    } else {
        return getS(nd->right);
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

/*
 * Get the value of an string variable.
 * @param s - variable's name 
 */
char *readS(char *s) {
    wName = s;
    return getS(root);
}

