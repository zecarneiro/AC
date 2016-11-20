#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#ifdef __APPLE__
#include <netpbm/pam.h>
#else
#include <pam.h>
#endif
#include "funcs.h"
#include <math.h>

#define PI 3.14159265359

void teste(ImageF * in, ImageF * out){
    int i,j;
 for (i=0;i<out->rows;i++){
      // ponteiros para linha i de cada imagem
      double * row1=(in->data)+i*in->cols;
      double * row2=(out->data)+i*out->cols;
      
      for(j=0;j<out->cols;j++){	  
	  row2[j]=row1[j]+sin(j/24.0*M_PI)*50.0;
      }      
  }
}


Image * loadPBM(char * fname){
  FILE * file;
  struct pam inpam;
  tuple * tuplerow;
  unsigned int row;
  Image * image;
  int aux;

  file=fopen(fname,"r");
  pnm_readpaminit(file, &inpam, /*PAM_STRUCT_SIZE(tuple_type)*/ sizeof(struct pam));
   
  printf("Reading image\n");
  printf("width=%d,height=%d,depth=%d\n",inpam.width,inpam.height,inpam.depth);
   
  /* allocating image*/
  image=(Image*)malloc(sizeof(Image));
  image->cols=inpam.width;
  image->rows=inpam.height;
  image->widthStep=image->cols;
  aux=image->cols & 0x3;
  if (aux!=0){
    image->widthStep+=4-aux;
  }
  image->data=(unsigned char *)malloc(image->widthStep*image->rows);
   
  tuplerow = pnm_allocpamrow(&inpam);
   
  for (row = 0; row < inpam.height; row++) {
    unsigned int column;
    pnm_readpamrow(&inpam, tuplerow);
    for (column = 0; column < inpam.width; ++column) {
      unsigned int plane;
      for (plane = 0; plane < inpam.depth; ++plane) {
	image->data[image->widthStep*row+column]= tuplerow[column][plane];
      }
    }
  } 
     
  pnm_freepamrow(tuplerow);
  fclose(file);
  return image;
   
}

void savePBM(char * fname, Image * image){
  FILE * file;
  struct pam outpam;
  tuple * tuplerow;
  unsigned int row;

  int aux;

  file=fopen(fname,"w");
  outpam.file=file;
  outpam.size=sizeof(struct pam);
  outpam.len=sizeof(struct pam);
  outpam.format=RPGM_FORMAT;
  outpam.plainformat=0;
  outpam.height=image->rows;
  outpam.width=image->cols;
  outpam.depth=1;
  outpam.maxval=255;
  strcpy(outpam.tuple_type,PAM_PGM_TUPLETYPE);
  /*  outpam.allocation_depth=0;
  outpam.comment_p="ficha 4 de Arquitecura de computadores 2010";
  */
  pnm_writepaminit( &outpam );
   
  printf("Writing image\n");

   
   
  tuplerow = pnm_allocpamrow(&outpam);
   
  for (row = 0; row < outpam.height; row++) {
    unsigned int column;
    for (column = 0; column < outpam.width; ++column) {
      unsigned int plane;
      for (plane = 0; plane < outpam.depth; ++plane) {
	tuplerow[column][plane]=image->data[image->widthStep*row+column];
      }
    }
    pnm_writepamrow(&outpam, tuplerow); 
  } 
     
  pnm_freepamrow(tuplerow);
  fclose(file);
}

//MALLOC REFERENCIA MATRIX 2D -> é 1D :(


//ImageF *imginf=NULL;

//imginf=(ImageF*)malloc(sizeof(ImageF));
//imginf->rows=imgin->rows;
//imginf->cols=imgin->cols;
//imginf->widthStep=imginf->cols*sizeof(double);
//imginf->data=(double *)malloc(imginf->rows*imginf->cols*sizeof(double));

// copiar imagem para uma nova em formato double
    //for (i=0;i<imgin->rows;i++){     
	//for(j=0;j<imgin->cols;j++)
	  //  imginf->data[i*imginf->cols+j]=imgin->data[i*imgin->cols+j];
   // }

/*   void teste(ImageF * in, ImageF * out){
    int i,j;
 for (i=0;i<out->rows;i++){
      // ponteiros para linha i de cada imagem
      double * row1=(in->data)+i*in->cols;
      double * row2=(out->data)+i*out->cols;
      
      for(j=0;j<out->cols;j++){	  
	  row2[j]=row1[j]+sin(j/24.0*M_PI)*50.0;
      }      
  }
}*/
