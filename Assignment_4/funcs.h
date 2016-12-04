#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include <fcntl.h>
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

struct MatrixEnv{
  int rows;
  int cols;
  int inverso;
  double **matriz_re;
  double **matriz_im;
}; 
typedef struct MatrixEnv MEnv;

Image * loadPBM(char * fname);
void savePBM(char * fname, Image * image);
void teste(ImageF * in, ImageF * out);

/* coloque aqui a seguir adeclaração das funções a desenvolver */
ImageF * genlpfmask(int , int);
void dofilt(ImageF * , ImageF * , ImageF * , ImageF * , ImageF * );
void fti(ImageF *, ImageF *, ImageF *, ImageF *, int );

void transposta(ImageF *in_re, ImageF *in_img);

struct timespec SubtracaoTempo(struct timespec Inicio, struct timespec Fim);