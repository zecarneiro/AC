#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcs.h"

int main(int argc, char**argv){
    
    char fnamein[255];
    char fnameout[255];
    
    
    Image * imgin=NULL;
    Image * imgout=NULL;
    ImageF *imginf=NULL;
    ImageF * imgoutf=NULL;
    
    if (argc!=3){
	printf("Uso: %s <imagem_in.bmp> <imagem_flt.bmp>\n",argv[0]);
	exit(1);
    }
  
    strcpy(fnamein,argv[1]);
    strcpy(fnameout,argv[2]);
    imgin=loadPBM(fnamein);
    imgout=(Image *)malloc(sizeof(Image));
    imgout->rows=imgin->rows;
    imgout->cols=imgin->cols;
    imgout->widthStep=imgin->widthStep;
    imgout->data=(unsigned char*)malloc(imgout->rows*imgout->cols);

    imginf=(ImageF*)malloc(sizeof(ImageF));
    imginf->rows=imgin->rows;
    imginf->cols=imgin->cols;
    imginf->widthStep=imginf->cols*sizeof(double);
    imginf->data=(double *)malloc(imginf->rows*imginf->cols*sizeof(double));

    imgoutf=(ImageF*)malloc(sizeof(ImageF));
    imgoutf->rows=imgin->rows;
    imgoutf->cols=imgin->cols;
    imgoutf->widthStep=imgoutf->cols*sizeof(double);
    imgoutf->data=(double *)malloc(imgoutf->rows*imgoutf->cols*sizeof(double));

    int i,j;

    // copiar imagem para uma nova em formato double
    for (i=0;i<imgin->rows;i++){     
	for(j=0;j<imgin->cols;j++)
	    imginf->data[i*imginf->cols+j]=imgin->data[i*imgin->cols+j];
    }

    /** processamento */
    /* Estamos a criar uma matriz com numeros imaginarios
     * Como a variavel imginf é que contém a informação original, então a vou usar */
    ImageF *imgin_img = NULL;
    imgin_img=(ImageF*)malloc(sizeof(ImageF));
    imgin_img->rows=imginf->rows;
    imgin_img->cols=imginf->cols;
    imgin_img->widthStep=imginf->widthStep;
    imgin_img->data=(double *)malloc(imgin_img->rows*imgin_img->cols*sizeof(double));

    for (i=0;i<imgin_img->rows;i++){
        for(j=0;j<imgin_img->cols;j++){
            imgin_img->data[i*imgin_img->cols+j]=0;
        }  
    }

    /* Estamos a criar as estruturas para as saidas necessárias para as funções
     * referidas no enunciado e feitas por nós, sendo
     * out_imag = matriz com valores imaginarios de saida
     * out_real = matriz com valores reais de saida
     * out_mask = Mascara */
    ImageF *out_imag = NULL;
    out_imag=(ImageF*)malloc(sizeof(ImageF));
    out_imag->rows=imginf->rows;
    out_imag->cols=imginf->cols;
    out_imag->widthStep=imginf->widthStep;
    out_imag->data=(double *)malloc(out_imag->rows*out_imag->cols*sizeof(double));

    ImageF *out_real = NULL;
    out_real=(ImageF*)malloc(sizeof(ImageF));
    out_real->rows=imginf->rows;
    out_real->cols=imginf->cols;
    out_real->widthStep=imginf->widthStep;
    out_real->data=(double *)malloc(out_real->rows*out_real->cols*sizeof(double));

    ImageF *auxiliar_real = NULL;
    auxiliar_real=(ImageF*)malloc(sizeof(ImageF));
    auxiliar_real->rows=imginf->rows;
    auxiliar_real->cols=imginf->cols;
    auxiliar_real->widthStep=imginf->widthStep;
    auxiliar_real->data=(double *)malloc(out_real->rows*out_real->cols*sizeof(double));

    ImageF *auxiliar_im = NULL;
    auxiliar_im=(ImageF*)malloc(sizeof(ImageF));
    auxiliar_im->rows=imginf->rows;
    auxiliar_im->cols=imginf->cols;
    auxiliar_im->widthStep=imginf->widthStep;
    auxiliar_im->data=(double *)malloc(out_real->rows*out_real->cols*sizeof(double));

    ImageF *out_mask = NULL;
    #pragma omp parallel
    {

        printf("\n\n=======MASCARA========\\n\n");
        /** cria mascara **/
        out_mask=genlpfmask(imginf->rows,imginf->cols);
        
        printf("\n\n=======DFT========\\n\n");
        /** calcula dft da imagem */
        fti(imginf, imgin_img, out_real, out_imag, 0);
    }

    printf("\n\n=======FILTRAGEM========\\n\n");
    /** multiplica pela mascara */
    dofilt(out_real, out_imag, out_mask, auxiliar_real, auxiliar_im);

    printf("\n\n=======IDFT========\\n\n");
    /** calcula dft inversa da imagem filtrada */
    fti(auxiliar_real, auxiliar_im, out_real, out_imag, 1);

    /** copia para imagem de saida imgout */
    double val;
    for (i=0;i<out_real->rows;i++){
        for(j=0;j<out_real->cols;j++){
            val=out_real->data[i*out_real->cols+j];
            imgout->data[i*imgout->cols+j]=(unsigned char)val;
        }  
    }
    savePBM(fnameout,imgout);
    
    return 0;
}