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

void fti(ImageF *in_re, ImageF *in_img, ImageF *out_re, ImageF *out_img, int inverse){

	//store size
	int rows = in_re->rows; // = M
	int cols = in_re->cols; // = N
	double Resultado_re = 0.0;
	double Resultado_im = 0.0;

	int thread_id;

	//Faz DFT	
	if(inverse == 0)
	{
		printf("DFT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		#pragma omp parallel
		{
			//#pragma omp for
			for(int k = 0;k < rows; k++)
			{ //percorre linhas
				
				//#pragma omp for
				for(int l = 0; l < cols; l++)//percorre colunas
				{
					//Somatório outside
					#pragma omp for //reduction (+:Resultado_re) reduction (+:Resultado_im)
					for(int m = 0; m <= rows-1; m++)
					{
					
						//int n1;
						//Somatório inside
							//#pragma omp for //reduction (+:Resultado_re) reduction (+:Resultado_im)
							for(int n = 0; n <= cols-1; n++)
							{
								Resultado_re += in_re->data[m*cols+n]*( cos(-2.0*PI*(l*n/cols)) - I*sin(2.0*PI*(l*n/cols)) );

								//long double seno = (long double) sin(2.0*PI*(l*n/cols));
								//printf ("seno = %Le | x = %lf \n", seno, 2.0*PI*(l*n/cols));
								double aux = in_re->data[m*cols+n];
								double trig = ( cos(-2.0*PI*(l*n/cols)) - I*sin(2.0*PI*(l*n/cols)) );
            					printf("aux = %lf | trig = %lf \n", aux,trig);

								Resultado_im += in_re->data[m*cols+n]*( cos(2.0*PI*(l*n/cols)) - I*sin(2.0*PI*(l*n/cols)) );

								//thread_id = omp_get_thread_num ( ) ;
								//printf("Iterações: %d - %d - %d - %d\n", k,l,m,n);
								//printf("Resultado_re = %lf \n", Resultado_re);
								//printf("n = %d & thread_id = %d\n", n, thread_id);
								//n1 = n;
							}

							Resultado_re += Resultado_re*( cos(2.0*PI*(k*m/rows)) - I*sin(2.0*PI*(k*m/rows)) ); 
							
							Resultado_im += Resultado_im*( cos(2.0*PI*(k*m/rows)) - I*sin(2.0*PI*(k*m/rows)) ); 
							//printf("Resultado = %lf + j*%lf \n",Resultado_re, Resultado_im);

							//printf("Resultado_re = %d | cos = %d \n", (int) Resultado_re/(rows*cols), (int) cos(PI*(l*n1/cols)) );

							//thread_id = omp_get_thread_num ( ) ;
							//printf("m = %d & thread_id = %d\n", m, thread_id);
					}
					out_re->data[k*cols+l] = Resultado_re;
					out_img->data[k*cols+l] = Resultado_im;

					Resultado_re = 0.0;
					Resultado_im = 0.0;

					//thread_id = omp_get_thread_num ( ) ;
					//printf("l = %d & thread_id = %d\n", l, thread_id);
					//printf("Iterações: %d - %d\n", k,l);
				}

				//thread_id = omp_get_thread_num ( ) ;
				//printf("k = %d & thread_id = %d\n", k, thread_id);
			}
		}
	}

	else if (inverse == 1)
	{ //Faz IDFT

		#pragma omp parallel
		{
			
			//#pragma omp for 
			for(int k = 0;k < rows; k++) //percorre linhas
			{
				//#pragma omp for
				for(int l = 0; l < cols; l++)//percorre colunas
				{
					//Somatório outside
					#pragma omp for reduction (+:Resultado_re) reduction (+:Resultado_im)
					for(int m = 0; m <= rows-1; m++)
					{
						//Somatório inside
						//#pragma omp for //reduction (+:Resultado_re) reduction (+:Resultado_im)
						for(int n = 0; n <= cols-1; n++)
						{
							Resultado_re += in_re->data[m*cols+n]*( cos(2.0*PI*(l*n/cols)) + I*sin(2.0*PI*(l*n/cols)) );
								Resultado_im += in_re->data[m*cols+n]*( cos(2.0*PI*(l*n/cols)) + I*sin(2.0*PI*(l*n/cols)) );
							//thread_id = omp_get_thread_num ( ) ;
								//printf("Iterações: %d - %d - %d - %d\n", k,l,m,n);
								//printf("n = %d & thread_id = %d\n", n, thread_id);
						}
						Resultado_re += Resultado_re*( cos(2.0*PI*(k*m/rows)) + I*sin(2.0*PI*(k*m/rows)) ); 
							
						Resultado_im += Resultado_im*( cos(2.0*PI*(k*m/rows)) + I*sin(2.0*PI*(k*m/rows)) ); 

						//thread_id = omp_get_thread_num ( ) ;
							//printf("m = %d & thread_id = %d\n", m, thread_id);
					}
					out_re->data[k*cols+l] = Resultado_re/(rows*cols);
					out_img->data[k*cols+l] = Resultado_im/(rows*cols);

					//double aux = out_re->data[k*out_re->cols+l];
            		//printf("out_re = %lf \n", aux);

					Resultado_re = 0.0;
					Resultado_im = 0.0;

					//thread_id = omp_get_thread_num ( ) ;
					//printf("l = %d & thread_id = %d\n", l, thread_id);
					//printf("Iterações: %d - %d\n", k,l);
				}
				//thread_id = omp_get_thread_num ( ) ;
			//printf("k = %d & thread_id = %d\n", k, thread_id);
			}
		}
	}
}
