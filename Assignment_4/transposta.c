#include "funcs.h"

void transposta(ImageF *in_re, ImageF *in_img)
{
    int rows = in_re->rows; // = M
	int cols = in_re->cols; // = N
    
    double **buffer_re = (double**)malloc(cols*sizeof(double*));
    double **buffer_img = (double**)malloc(cols*sizeof(double*));

    /* Como criei uma matriz Nx1 e por ponteiro de ponteiro, então
	 * vou criar as colunas para cada matriz criada por anterior */
	#pragma omp parallel
	{
		#pragma omp for
		for(int i = 0; i < cols; ++i){
			buffer_re[i] = (double*)malloc(rows*sizeof(double));
			buffer_img[i] = (double*)malloc(rows*sizeof(double));
		}
	}

	/* Faço a copia das matrizes de entrada e armazeno nas matrizes
	 * criadas por mim */
	for(int i = 0; i < rows; ++i)
    {
        //põe no buffer
        #pragma omp parallel
		{
			#pragma omp for
			for(int j = 0; j < cols; ++j)
            {
				buffer_re[j][i] = in_re->data[i*cols+j];
				buffer_img[j][i] = in_img->data[i*cols+j];
			}
		}
	}
    
    //transpõe
	for(int i = 0; i < rows; ++i)
	{
		#pragma omp parallel
		{
			#pragma omp for
			for(int j = 0; j < cols; ++j)
            {
				in_re->data[i*cols+j] = buffer_re[j][i];
				in_img->data[i*cols+j] = buffer_img[j][i];
			}
		}
	}

	//muda tamanhos
    in_re->rows = cols;
    in_re->cols = rows;
    in_img->rows = cols;
    in_img->cols = rows;

    /* Liberto a memória */
    free(buffer_re);
    free(buffer_img);
}