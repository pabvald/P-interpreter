

all: clean executable

syntactic:
	yacc --defines -v pcc.y -o pcc.c	


lexical: syntactic
	flex -o pcc.lex.c pcc.l


executable: lexical
	gcc pcc.c pcc.lex.c symtab.c utils.c astree.c  -o x_pcc -lm


clean:
	rm -f pcc.h pcc.lex.c x_pcc pcc.output pcc.c
