all: prog_mpi_omp prog_mpi

prog_mpi_omp: prog.c funcs.h
	mpicc -g prog.c funcs.c func1.c func2.c transposta.c -lm -lnetpbm -openmp -o prog_mpi_omp

prog_mpi: prog.c funcs.h
	mpicc prog.c funcs.c func1.c func2.c transposta.c -lm -lnetpbm -o prog_mpi

clean:
	rm -f prog_mpi_omp prog_mpi

#mpirun −f cluster_debug xterm −e gdb −q −tui −x gdb.txt ./prog_mpi_omp