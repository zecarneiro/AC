#include "funcs.h"

/* Função que faz a FFT/DFT unidimensional, ou seja,
 * faz a transformada de um vector */
void fft(double *v_re , double *ve_im, int N, int inv){

	/* Vou alocar um espaço do tamanho do vector
	 * recebido onde vou armazenar o resultado da
	 * transformada */ 
	double *Re = (double*)malloc(N*sizeof(double));
	double *Im = (double*)malloc(N*sizeof(double));

	double dois_PI,arg_cs,re,im;
	int k,n;

	/* Através da expressão da transformada, o theta
	 * é negativo caso seja DFT ou positivo caso seja
	 * IDFT */
	if(inv == 0){
		dois_PI = -2.0*PI;
	}

	else{
		dois_PI = 2.0*PI;
	}

	/* É aplicado a expresśão da transformadas */
	for(k = 0; k < N; ++k){
		/* Como a expressão da transformada é um
		 * somatório, então vector tem que ser
		 * igual a zero no inicio */
		re = 0.0;
		im = 0.0;

		#pragma omp parallel for private(arg_cs) reduction(+:re,im)
		for(n = 0; n < N; ++n){
			arg_cs = (double)(k*n);
			arg_cs = (arg_cs*dois_PI)/N;

			re = re + v_re[n]*cos(arg_cs) - ve_im[n]*sin(arg_cs);
			im = im + v_re[n]*sin(arg_cs) + ve_im[n]*cos(arg_cs);
		}

		/* Os resultados são introduzidos no vector */
		Re[k] = re;
		Im[k] = im;

		/* Caso seja uma transformada inversa
		 * então tenho que dividir o resultado
		 * pelo numero de pontos */
		if(inv == 1){
			Re[k] = Re[k]/N;
			Im[k] = Im[k]/N;
		}
	}

	/* Depois de fazer a transformada, os resultados
	 * são devolvidos */
	#pragma omp parallel
	{
		#pragma omp for
		for(k = 0; k < N; ++k){
			v_re[k] = Re[k];
			ve_im[k] = Im[k];
		}
	}

	/* Liberto a memória */
	Re = NULL;
	Im = NULL;
}

/* Função que recebe uma matriz real e imaginaria de uma imagem e devolve a DFT
 * ou IDFT da mesma imagem/matriz */
void fti(ImageF *in_re, ImageF *in_img, ImageF *out_re, ImageF *out_img, int inverse, int n_linha, int linha_max)
{
	//store size
	int rows = in_re->rows; // = M
	int cols = in_re->cols; // = N
	int i,j;

	/* crio os vectores que vai receber os valores de cada linha e
	 * de cada coluna*/
	double *linha_re = (double*)malloc(cols*sizeof(double));
	double *linha_im = (double*)malloc(cols*sizeof(double));
	double *coluna_re = (double*)malloc(rows*sizeof(double));
	double *coluna_im = (double*)malloc(rows*sizeof(double));

	/* Nesta parte pretendo criar uma matriz bidimensional */
	/* Reservo espaço para uma matriz Nx1, nesse caso as matrizes re e im */
	double **matriz_re = (double**)malloc(rows*sizeof(double*));
	double **matriz_im = (double**)malloc(rows*sizeof(double*));

	/* Como criei uma matriz Nx1 e por ponteiro de ponteiro, então
	 * vou criar as colunas para cada matriz criada por anterior */
	#pragma omp parallel
	{
		#pragma omp for
		for(i = 0; i < rows; ++i){
			matriz_re[i] = (double*)malloc(cols*sizeof(double));
			matriz_im[i] = (double*)malloc(cols*sizeof(double));
		}
	}

	/* Faço a copia das matrizes de entrada e armazeno nas matrizes
	 * criadas por mim */
	for(i = n_linha; i < linha_max; ++i){
		#pragma omp parallel
		{
			#pragma omp for
			for(j = 0; j < cols; ++j){
				matriz_re[i][j] = in_re->data[i*cols+j];
				matriz_im[i][j] = in_img->data[i*cols+j];
			}
		}
	}

	// Calculos para as linhas
	for(i = n_linha; i < linha_max; ++i){

		/* As linhas da imagem/matriz são passados para os
		 * vectores linhas */
		#pragma omp parallel
		{
			#pragma omp for
			for(j = 0; j < cols; ++j){
				linha_re[j] = matriz_re[i][j];
				linha_im[j] = matriz_im[i][j];
			}
		}		

		// Vou fazer a fft de um vector, ou seja unidimensional
		fft(linha_re,linha_im,cols,inverse);

		/* Preecho as respectivas linha, ou seja, as matrizes
		 * recebem resultado da transformada */
		#pragma omp parallel
		{
			#pragma omp for
			for(j = 0; j < cols; ++j){
				matriz_re[i][j] = linha_re[j];
				matriz_im[i][j] = linha_im[j];
			}
		}
	}

	// Devolvo a DFT ou IDFT para a estrutura de saída
	for(i = n_linha; i < linha_max; ++i){
		#pragma omp parallel
		{
			#pragma omp for
			for(j = 0; j < cols; ++j){
				out_re->data[i*cols+j] = matriz_re[i][j];
				out_img->data[i*cols+j] = matriz_im[i][j];
			}
		}
	}

	/* Liberto a memória */
	free(matriz_im);
	free(matriz_re);
	free(linha_re);
	free(linha_im);
	free(coluna_im);
	free(coluna_re);
}