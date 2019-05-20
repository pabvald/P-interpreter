/**
 * @Author: Dušan Kolář
 * @Year:   2003-2018
 * Copyright (c) 2018
 * Licence: GLP 3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "utils.h"

static char fName[10240] = "";

static unsigned char wasE = 0;
static unsigned char exitOnE = 0;

/**
 * Set the file name.
 * @param str - file name
 */
void setFilename(const char *str) {
  strcpy(fName,str);
}



/*--------------------------------------------------- ERROR HANDLING ----------------------------------------------*/

/**
 * Return true if there's been an error; 
 */
int wasError(void) {
  return wasE; 
}

/**
 * Set the flag which indicates that the programm exited on error to 1.
 */
void exitOnError(void) {
   exitOnE = 1; 
}

/**
 * Print a error and  a number of line in the specified format.
 * @param lineno - number of line where the error ocurred
 * @param format - printing format
 */
void prError(unsigned short lineno, char *format, ...) {
  va_list ap;
  char *arg;

  wasE = 1;

  va_start(ap,format);
  fprintf(stderr,"%s (%u): ",fName,lineno);
  vfprintf(stderr,format,ap);
  fflush(stderr);
  va_end(ap);

  if (exitOnE) exit(1);
}


/**
 * Print a warning and a number of line in the specified format.
 * @param lineno - number of the line which caused the warning.
 * @param format - printing format
 */
void prWarning(unsigned short lineno, char *format, ...) {
  va_list ap;
  char *arg;

  va_start(ap,format);
  fprintf(stderr,"%s (%u): ",fName,lineno);
  vfprintf(stderr,format,ap);
  fflush(stderr);
  va_end(ap);

}

/*------------------------------------------------------------ MEMORY ---------------------------------------------------- */

/**
 * Allocate memory.
 * @param size - number of bytes
 * @return pointer.
 */
void *xmalloc(unsigned short size) {
  void *tmp;

  if ((tmp = malloc(size))==NULL) {
    fprintf(stderr,"Fatal error: Out of memory\n");
    fflush(stderr);
    exit(2);
  }

  return tmp;
}

/**
 * Free memory.
 * @param ptr - pointer
 */
void xfree(void *ptr) {
  free(ptr);
}

/* ---------------------------------------------------------- STRINGS ----------------------------------------------------- */

/**
 * Copy string in another memory address.
 * @param s - original string
 * @return copy of the string
 */
char *sdup(const char *s) {

  char *res = xmalloc(strlen(s)+1);

  strcpy(res,s);

  return res;
}


