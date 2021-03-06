
#define UM_SEC 1000000000L

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
void teste(ImageF * in, ImageF * out);

/* coloque aqui a seguir adeclaração das funções a desenvolver */
ImageF * genlpfmask(int , int);
void dofilt(ImageF * , ImageF * , ImageF * , ImageF * , ImageF * );
void fti(ImageF *, ImageF *, ImageF *, ImageF *, int );

struct timespec SubtracaoTempo(struct timespec Inicio, struct timespec Fim);