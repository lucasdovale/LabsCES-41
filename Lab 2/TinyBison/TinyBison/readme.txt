 bison -d cminus.y
 flex cminus.l
 gcc -c main.c lex.yy.c cminus.tab.c util.c
 gcc -o cminus *.o -ll