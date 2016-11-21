#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include "funcs.h"
#include "complex.h"


#ifdef __OPENMP
    #include <omp.h>
#else 
	#define omp_get_thread_num() 0
#endif

#define PI 3.14159265359

void fti(ImageF *in_re, ImageF *in_img, ImageF *out_re, ImageF *out_img, int inverse)
{

	//store size
	int rows = in_re->rows; // = M
	int cols = in_re->cols; // = N
	double Resultado_linha_re = 0.0;
	double Resultado_linha_im = 0.0;

	double Resultado_coluna_re = 0.0;
	double Resultado_coluna_im = 0.0;

	int thread_id;

	//Faz DFT	
	if(inverse == 0)
	{
		for(int k = 0;k < rows; k++)
		{ //percorre linhas
			
			#pragma omp for
			for(int l = 0; l < cols; l++)//percorre colunas
			{
				//Somatório outside
				//#pragma omp for //reduction (+:Resultado_re) reduction (+:Resultado_im)
				for(int m = 0; m <= rows-1; m++)
				{
				
					for(int n = 0; n <= cols-1; n++)
					{
						Resultado_linha_re += in_re->data[m*cols+n]*cos(2.0*PI*(l*n/cols)) + in_img->data[m*cols+n]*sin(2.0*PI*(l*n/cols));
						Resultado_linha_im += in_img->data[m*cols+n]*cos(2.0*PI*(l*n/cols)) - in_re->data[m*cols+n]*sin(2.0*PI*(l*n/cols));
					}
					
					Resultado_coluna_re += Resultado_linha_re*cos(2.0*PI*(k*m/rows)) + Resultado_linha_im*sin(2.0*PI*(k*m/rows)); 
					
					Resultado_coluna_im += Resultado_linha_im*cos(2.0*PI*(k*m/rows)) - Resultado_linha_re*sin(2.0*PI*(k*m/rows));

					Resultado_linha_re = 0.0;
					Resultado_linha_im = 0.0;
				}

				out_re->data[k*cols+l] = Resultado_coluna_re;
				out_img->data[k*cols+l] = Resultado_coluna_im;

				Resultado_coluna_re = 0.0;
				Resultado_coluna_im = 0.0;
			}
		}
	}

	else if (inverse == 1)
	{ //Faz IDFT
		for(int k = 0;k < rows; k++) //percorre linhas
		{
			#pragma omp for
			for(int l = 0; l < cols; l++)//percorre colunas
			{
				//Somatório outside
				//#pragma omp for reduction (+:Resultado_coluna_re) reduction (+:Resultado_coluna_im)
				for(int m = 0; m <= rows-1; m++)
				{
					//Somatório inside
					//#pragma omp for //reduction (+:Resultado_re) reduction (+:Resultado_im)
					for(int n = 0; n <= cols-1; n++)
					{
						Resultado_linha_re += in_re->data[m*cols+n]*cos(2.0*PI*(l*n/cols)) - in_img->data[m*cols+n]*sin(2.0*PI*(l*n/cols));
						Resultado_linha_im += in_re->data[m*cols+n]*sin(2.0*PI*(l*n/cols)) + in_img->data[m*cols+n]*cos(2.0*PI*(l*n/cols));
					}

					Resultado_linha_re = Resultado_linha_re/cols;
					Resultado_linha_im = Resultado_linha_im/cols;
					
					Resultado_coluna_re += Resultado_linha_re*cos(2.0*PI*(k*m/rows)) - Resultado_linha_im*sin(2.0*PI*(k*m/rows)); 
					Resultado_coluna_im += Resultado_linha_re*sin(2.0*PI*(k*m/rows)) + Resultado_linha_im*cos(2.0*PI*(k*m/rows));

					Resultado_linha_re = 0.0;
					Resultado_linha_im = 0.0; 
				}

				out_re->data[k*cols+l] = Resultado_coluna_re/rows;
				out_img->data[k*cols+l] = Resultado_coluna_im/rows;

				Resultado_coluna_re = 0.0;
				Resultado_coluna_im = 0.0;
			}
		}
	}
}

