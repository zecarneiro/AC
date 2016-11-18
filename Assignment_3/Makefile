all: prog progOMP

prog: prog.c funcs.c funcs.h
	gcc -g  prog.c funcs.c -lm  -lnetpbm -o prog

progOMP: prog.c funcs.c funcs.h
	gcc  prog.c funcs.c  -lm -lnetpbm  -fopenmp -o progOMP


clean:
	rm -f prog progOMP
