prog_mpi_omp: prog.c funcs.h

	mpicc funcs.c func1.c func2.c transposta.c -lm -lnetpbm  -fopenmp −o prog_mpi_omp

clean:
	rm -f  prog_mpi_omp
