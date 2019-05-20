

all:
	@make clean
	@make syntactic
	@make lexical
	@make executable
	@make testing


syntactic:
	yacc --defines -v pcc.y -o pcc.c	


lexical: y
	flex -o pcc.lex.c pcc.l


executable: l
	gcc pcc.c pcc.lex.c symtab.c utils.c astree.c  -o x_pcc -lm

testing:
	echo "Testing done"


clean:
	rm -f pcc.h pcc.lex.c x_pcc pcc.output pcc.c
