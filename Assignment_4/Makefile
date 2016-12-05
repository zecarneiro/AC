all: prog_mpi_omp prog_mpi

prog_mpi_omp: prog.c funcs.h
	mpicc prog.c funcs.c func1.c func2.c transposta.c -lm -lnetpbm -openmp -o prog_mpi_omp

prog_mpi: prog.c funcs.h
	mpicc prog.c funcs.c func1.c func2.c transposta.c -lm -lnetpbm -o prog_mpi

clean:
	rm -f prog_mpi_omp prog_mpi
