#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include <fcntl.h>
#include <errno.h>
#include "mpi.h"

#include <time.h>

#ifdef __OPENMP
    #include <omp.h>
#else 
	#define omp_get_thread_num() 0
#endif


#define UM_SEC 1000000000L
#define PI 3.14159265359

struct Matrix{
  int rows;
  int cols;
  unsigned char * data;
  int widthStep;
};
typedef struct Matrix Image; 

struct MatrixF{
  int rows;
  int cols;
  double * data;
  int widthStep;
};
typedef struct MatrixF ImageF;

Image * loadPBM(char * fname);
void savePBM(char * fname, Image * image);

/* coloque aqui a seguir adeclaração das funções a desenvolver */
ImageF * genlpfmask(int , int);
void dofilt(ImageF * , ImageF * , ImageF * , ImageF * , ImageF * );
void fti(ImageF *, ImageF *, ImageF *, ImageF *, int);
struct timespec SubtracaoTempo(struct timespec Inicio, struct timespec Fim);

int devolve_metade_linha(int);
int acha_n_linhas(int, int);
void Envia_Dados(ImageF *, ImageF *, ImageF *, ImageF *, int, int, int, MPI_Status *, MPI_Request *);
void Recebe_Dados(ImageF *, ImageF *, int, int, MPI_Status *, MPI_Request *);
void transposta(ImageF *, ImageF *);