#include "funcs.h"

void transposta(ImageF *in_re, ImageF *in_img)
{
    int rows = Real->rows; // = M
	int cols = Real->cols; // = N
    
    double *buffer_re = (double*)malloc(rows*cols*sizeof(double));
    double *buffer_img = (double*)malloc(cols*rows*sizeof(double));

	/* Faço a copia das matrizes de entrada e armazeno nas matrizes
	 * criadas por mim */
	for(i = 0; i < rows; ++i)
    {
        //põe no buffer
        #pragma omp parallel
		{
			#pragma omp for
			for(j = 0; j < cols; ++j)
            {
				buffer_re->data[i*cols+j] = in_re->data[i*cols+j];
				buffer_img->data[i*cols+j] = in_img->data[i*cols+j];
			}
		}

        //transpõe
		#pragma omp parallel
		{
			#pragma omp for
			for(j = 0; j < cols; ++j){
                
				in_re->data[j*cols+i] = buffer_re->data[i*cols+j];
				in_img->data[j*cols+i] = buffer_img->data[i*cols+j];
			}
		}
	}

}