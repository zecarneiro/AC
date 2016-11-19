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

ImageF * genlpfmask(int rows, int cols)
{
    double matriz[rows,cols]; // matriz de filtragem

    //Array com posições de início de fim dos espaços brancos
    int positions_rows[4] = {0, rows/4, rows-(rows/4), rows}
    int positions_cols[4] = {0, cols/4, cols-(cols/4), cols}

    //Preenche Branco
    for(int r = 0; r < rows, r++)
    {
        for(int c = 0; c < cols; c++)
        {
            // Verifica se está dentro da zona branca
            if(r >= position_rows[0] && c >= position_cols[0] && r <= position_rows[1] && c <= position_cols[1] || //zona superior esquerda
            r >= position_rows[0] && c >= position_cols[2] && r <= position_rows[1] && c <= position_cols[3] || //zona superior direita
            r >= position_rows[2] && c >= position_cols[0] && r <= position_rows[3] && c <= position_cols[1] || //zona inferior esquerda
            r >= position_rows[2] && c >= position_cols[2] && r <= position_rows[3] && c <= position_cols[3]) //zona inferior direita
            {
                matriz[r,c] = 1; //preenche branco 
            }
            else
            {
                matriz[r,c] = 0; //preenche preto 
            }
                
        }   
    }
    return (matriz);
}


void fti(ImageF *in_re, ImageF *in_img, ImageF *out_re, ImageF *out_img, int inverse){

	//store size
	int rows = in_re.rows; // = M
	int cols = in_re.cols; // = N
	int Resultado_re, Resultado_im;

	//Faz DFT	
	if(inverse == 0)
	{
		for(int k = 0;k < rows; k++) //percorre linhas
		{
			for(int l = 0; l < rows; l++)//percorre colunas
			{
				//Somatório outside
				for(int m = 0; m <= rows-1; m++)
				{
					//Somatório inside
					for(int n = 0; n <= cols-1; n++)
					{
						Resultado_re += in_re.data(m,n)*cos(-2*PI*(l*n/cols));
						Resultado_im += in_im.data(m,n)*cos(-2*PI*(l*n/cols));
					}
					Resultado_re += Resultado_re*cos(-2*PI*(k*m/rows));
					Resultado_im += Resultado_re*cos(-2*PI*(k*m/rows));
				}
				out_re.data(k,l) = Resultado_re;
				out_im.data(k,l) = Resultado_im;
			}
		}
	}
	else if (inverse == 1) //Faz IDFT
	{
		//Parte Real
		for(int k = 0;k < rows; k++) //percorre linhas
		{
			for(int l = 0; l < rows; l++)//percorre colunas
			{
				//Somatório outside
				for(int m = 0; m <= rows-1; m++)
				{
					//Somatório inside
					for(int n = 0; n <= cols-1; n++)
					{
						Resultado_re += in_re.data(m,n)*cos(2*PI*(l*n/cols));
						Resultado_im += in_im.data(m,n)*cos(2*PI*(l*n/cols));
					}
					Resultado_re += Resultado_re*cos(2*PI*(k*m/rows));
					Resultado_im += Resultado_re*cos(2*PI*(k*m/rows));
				}
				out_re.data(k,l) = Resultado_re;
				out_im.data(k,l) = Resultado_im;
			}
		}
	}
}

void dofilt(ImageF * in_re, ImageF * in_im, ImageF * mask, ImageF * out_re, ImageF * out_im)
{
    int rows = mask.rows;
    int cols = mask.cols;

    for(int r = 0; r < rows, r++)
    {
        for(int c = 0; c < cols; c++)
        {
            out_re[r,c] = in_re.data[r,c]*mask[r,c];
            out_im[r,c] = in_im.data[r,c]*mask[r,c];
        }   
    }
}