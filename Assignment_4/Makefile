# NOME: José Manuel C. Noronha
# NOME: Luís Marques

CC = mpicc
FLAGS = -lm -lnetpbm
FLAGS_OMP = -openmp
FUNC = funcs.c func1.c func2.c func3.c
PROG = prog.c
TARGET = prog_mpi
RM = rm -f
CLEAR = clear
RUN = mpiexec -f
CLUSTER = cluster_debug
FILEIN = Example_lena_denoise_noisy.pgm
FILEOUT = imagem.pgm

subsystem:
	$(CLEAR)
	$(CLEAR)
	$(RM) $(TARGET)_omp $(TARGET)
	$(CC) $(PROG) $(FUNC) $(FLAGS) $(FLAGS_OMP) -o $(TARGET)_omp
	$(CC) $(PROG) $(FUNC) $(FLAGS) -o $(TARGET)

run:
	$(RUN) $(CLUSTER) ./$(TARGET)_omp $(FILEIN) $(FILEOUT)

run1:
	$(RUN) $(CLUSTER) ./$(TARGET) $(FILEIN) $(FILEOUT)
	