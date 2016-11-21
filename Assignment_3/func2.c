#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include "funcs.h"


#ifdef __OPENMP
    #include <omp.h>
#endif

#define PI 3.14159265359

void fti(ImageF *in_re, ImageF *in_img, ImageF *out_re, ImageF *out_img, int inverse){

	//store size
	int rows = in_re->rows; // = M
	int cols = in_re->cols; // = N
	double Resultado_re, Resultado_im;

	//Faz DFT	
	if(inverse == 0)
	{
		#pragma omp parallel default(shared)
    	{
			#pragma omp for
			for(int k = 0;k < rows; k++) //percorre linhas
			{
				for(int l = 0; l < rows; l++)//percorre colunas
				{
					#pragma omp parallel shared(k,l)
					{
						#pragma omp for
						//Somatório outside
						for(int m = 0; m <= rows-1; m++)
						{
							//Somatório inside
							#pragma omp parallel shared(k,l,m)
							{
								#pragma omp for
								for(int n = 0; n <= cols-1; n++)
								{
									Resultado_re += in_re->data[m*cols+n]*cos(-2*PI*(l*n/cols));
									Resultado_im += in_img->data[m*cols+n]*sin(-2*PI*(l*n/cols));

									printf("Iterações: %d - %d - %d - %d\n", k,l,m,n);

								}
								Resultado_re += Resultado_re*cos(-2*PI*(k*m/rows));
								Resultado_im += Resultado_re*sin(-2*PI*(k*m/rows));
							}
						}
						out_re->data[k*cols+l] = Resultado_re;
						out_img->data[k*cols+l] = Resultado_im;
					}
				}
			}
		}
	}
	else if (inverse == 1) //Faz IDFT
	{
		#pragma omp parallel default(shared)
    	{
			//Parte Real
			#pragma omp for
			for(int k = 0;k < rows; k++) //percorre linhas
			{
				for(int l = 0; l < rows; l++)//percorre colunas
				{
					#pragma omp parallel shared(k,l)
					{
						#pragma omp for
						//Somatório outside
						for(int m = 0; m <= rows-1; m++)
						{
							//Somatório inside
							#pragma omp parallel shared(k,l,m)
							{
								#pragma omp for
								for(int n = 0; n <= cols-1; n++)
								{
									Resultado_re += in_re->data[m*cols+n]*cos(2*PI*(l*n/cols));
									Resultado_im += in_img->data[m*cols+n]*sin(2*PI*(l*n/cols));
									printf("Iterações: %d - %d - %d - %d\n", k,l,m,n);
								}
								Resultado_re += Resultado_re*cos(2*PI*(k*m/rows));
								Resultado_im += Resultado_re*sin(2*PI*(k*m/rows));
							}
						}
						out_re->data[k*cols+l] = Resultado_re;
						out_img->data[k*cols+l] = Resultado_im;
					}
				}
			}
		}
	}
}
