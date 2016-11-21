all: prog progOMP

prog: prog_old.c funcs.c func1.c func2.c funcs.h
	gcc -g  prog_old.c funcs.c func1.c func2.c -lm  -lnetpbm -o prog

progOMP: prog_old.c funcs.c funcs.h
	gcc  prog_old.c funcs.c func1.c func2.c -lm -lnetpbm  -fopenmp -o progOMP


clean:
	rm -f prog progOMP
