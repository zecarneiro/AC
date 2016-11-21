#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcs.h"

/* coloque aqui o código das funções pedidas e que devem ser chamadas abaixo */





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
        {
            imginf->data[i*imginf->cols+j]=imgin->data[i*imgin->cols+j];
            //double aux = imgin->data[i*imgin->cols+j];
            double aux = imginf->data[i*imginf->cols+j];
            printf("imginf = %lf \n", aux);
        }
    }

    /** processamento */
    /** cria mascara **/
    /** calcula dft da imagem */
    /** multiplica pela mascara */
    /** calcula dft inversa da imagem filtrada */
    /** copia para imagem de saida imgout */
  
    /** A seguir é apenas um exemplo que deve ser retirado e substituido pela chamadas às funções a escrever */
    /* Só para testar. Comentar quando incluir o seu código*/
    teste(imginf, imgoutf);

    for (i=0;i<imgoutf->rows;i++){
	for(j=0;j<imgoutf->cols;j++){
	   double val;
	    val=imgoutf->data[i*imgoutf->cols+j];
	    
	    if (val<0)
		val=0.0;
	    else if (val>255)
		val=255.0;
	    imgout->data[i*imgout->cols+j]=(unsigned char)val;
	    
	}
    }
    savePBM(fnameout,imgout);
    return 0;
    }

