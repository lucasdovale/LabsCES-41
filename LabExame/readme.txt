 flex cminus.l
 bison -d cminus.y
 gcc -c *.c
 gcc -o cminus *.o -ll