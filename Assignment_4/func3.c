#include "funcs.h"

/* Função usada para se o bter o numero de linhas que cada
 * processo vai calcular */
int acha_n_linhas(int rows,int NUM_Proc){
    if((rows%NUM_Proc) == 0){
        rows = rows/NUM_Proc;
        
    }

    else{        
        while(1){
        	rows -= 1;
            if((rows%NUM_Proc) == 0){
                rows = rows/NUM_Proc;
                break;
            }
        }
    }
    return rows;
}

void Envia_Dados(ImageF *re, ImageF *im, ImageF *ou_re, ImageF *ou_im, int inv, int n_process, int n_rows_cols, MPI_Status *stat, MPI_Request *req){
    int linha = 0;

    ImageF *env_re, *env_im;
    env_re = (ImageF*)malloc(sizeof(ImageF));
    env_im = (ImageF*)malloc(sizeof(ImageF));

    env_re->rows = n_rows_cols;
    env_re->cols = re->cols;
    env_re->data = (double *)malloc(env_re->rows*env_re->cols*sizeof(double));
    env_im->rows = n_rows_cols;
    env_im->cols = im->cols;
    env_im->data = (double *)malloc(env_im->rows*env_im->cols*sizeof(double));

    for(int dest = 1; dest < n_process; ++dest){
        /* Envio o numero de linha da matriz */
        MPI_Isend(&env_re->rows, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, req);
        MPI_Wait(req, stat);

        /* Envio o numero de colunas da matriz.
         * Como a matriz imaginaria tem o mesmo numero de colunas, então não
         * precisamos de enviar */
        MPI_Isend(&env_re->cols, 1, MPI_INT, dest, 1, MPI_COMM_WORLD, req);
        MPI_Wait(req, stat);

        /* Envia a informação se é para fazer a DFT ou a IDFT */
        MPI_Isend(&inv, 1, MPI_INT, dest, 2, MPI_COMM_WORLD, req);
        MPI_Wait(req, stat);

        for(int i = 0; i < n_rows_cols; ++i, ++linha)
        {
            #pragma omp parallel for
            for (int j = 0; j < re->cols; ++j)
            {
                env_re->data[i*env_re->cols+j] = re->data[linha*re->cols+j];
                env_im->data[i*env_re->cols+j] = im->data[linha*im->cols+j];
            }
        }
        /* Envio a matriz real */
        MPI_Isend(env_re->data, env_re->cols*env_re->rows, MPI_DOUBLE, dest, 3, MPI_COMM_WORLD, req);
        MPI_Wait(req, stat);

        /* Envio a matriz imaginaria */
        MPI_Isend(env_im->data, env_im->cols*env_im->rows, MPI_DOUBLE, dest, 4, MPI_COMM_WORLD, req);
        MPI_Wait(req, stat);
    }

    int linha_fim;
    linha_fim = re->rows - linha;
    env_re->rows = linha_fim;
    env_im->rows = linha_fim;

    env_re->data = NULL;
    env_im->data = NULL;
    env_re->data = (double *)malloc(linha_fim*env_re->cols*sizeof(double));
    env_im->data = (double *)malloc(linha_fim*env_im->cols*sizeof(double));

    int auxiliar = linha; 
    for(int i = 0; i < linha_fim; ++i, ++linha)
    {
        #pragma omp parallel for
        for (int j = 0; j < re->cols; ++j)
        {
            env_re->data[i*env_re->cols+j] = re->data[linha*re->cols+j];
            env_im->data[i*env_im->cols+j] = im->data[linha*im->cols+j];
        }
    }

    calcula_e_junta(env_re, env_im, inv);

    linha = auxiliar; 
    for(int i = 0; i < linha_fim; ++i, ++linha)
    {
        #pragma omp parallel for
        for (int j = 0; j < re->cols; ++j)
        {
            ou_re->data[linha*re->cols+j] = env_re->data[i*env_re->cols+j];
            ou_im->data[linha*im->cols+j] = env_im->data[i*env_im->cols+j];
        }
    }

    free(env_re);
    free(env_im);
}

void Recebe_Dados(ImageF *ou_re, ImageF *ou_im, int n_process, int n_rows_cols, MPI_Status *stat, MPI_Request *req){
    ImageF *rec_re, *rec_im;
    rec_re = (ImageF*)malloc(sizeof(ImageF));
    rec_im = (ImageF*)malloc(sizeof(ImageF));

    rec_re->rows = n_rows_cols;
    rec_re->cols = ou_re->cols;
    rec_re->data = (double *)malloc(rec_re->rows*rec_re->cols*sizeof(double));

    rec_im->rows = n_rows_cols;
    rec_im->cols = ou_im->cols;
    rec_im->data = (double *)malloc(rec_im->rows*rec_im->cols*sizeof(double));

    int linha;
    for(int source = 1; source < n_process; ++source){
        linha = (source-1)*n_rows_cols;

        /* Recebe a matriz real */
        MPI_Irecv(rec_re->data, rec_re->rows*rec_re->cols, MPI_DOUBLE, source, 6, MPI_COMM_WORLD, req);
        MPI_Wait(req, stat);

        /* Recebe a matriz imaginaria */
        MPI_Irecv(rec_im->data, rec_im->rows*rec_im->cols, MPI_DOUBLE, source, 7, MPI_COMM_WORLD, req);
        MPI_Wait(req, stat);
        
        /* Fazemos a DFT para as linhas */
        for(int i = 0; i < n_rows_cols; ++i, ++linha){
            /* Faço a copia das matrizes de entrada e armazeno nas matrizes
             * criadas por mim */
            #pragma omp parallel for
            for(int j = 0; j < ou_re->cols; ++j){
                ou_re->data[linha*ou_re->cols+j] = rec_re->data[i*rec_re->cols+j];
                ou_im->data[linha*ou_im->cols+j] = rec_im->data[i*rec_im->cols+j];
            }
        }
    }

    free(rec_re);
    free(rec_im);
}

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
    for(int i = 0; i < cols; ++i)
    {
        #pragma omp parallel
        {
            #pragma omp for
            for(int j = 0; j < rows; ++j)
            {
                in_re->data[i*rows+j] = buffer_re[i][j];
                in_img->data[i*rows+j] = buffer_img[i][j];
            }
        }
    }

    //muda tamanhos
    in_re->rows = cols;
    in_re->cols = rows;
    in_img->rows = cols;
    in_img->cols = rows;

    /* Liberto a memória */
    buffer_re = NULL;
    buffer_img = NULL;
}

void calcula_e_junta(ImageF *in_re, ImageF *in_im, int inv){
    int linha_max = MAX;
    int linha_copia, linha_preenche, exit;

    ImageF *re, *im;
    re = (ImageF*)malloc(sizeof(ImageF));
    im = (ImageF*)malloc(sizeof(ImageF));

    re->rows = linha_max;
    re->cols = in_re->cols;
    re->data = (double *)malloc(re->rows*re->cols*sizeof(double));
    im->rows = linha_max;
    im->cols = in_im->cols;
    im->data = (double *)malloc(im->rows*im->cols*sizeof(double));

    linha_preenche = 0;
    linha_copia = 0;
    exit = -1;
    while(exit == -1){    	
    	if((in_re->rows - linha_copia) <= linha_max){
    		re->rows = in_re->rows - linha_copia;
    		im->rows = in_im->rows - linha_copia;

    		re->data = NULL;
    		im->data = NULL;
    		re->data = (double *)malloc(re->rows*re->cols*sizeof(double));
    		im->data = (double *)malloc(im->rows*im->cols*sizeof(double));

    		exit = 0;
    	}

    	#pragma omp parallel for
    	for(int i = 0; i < re->rows; ++i, ++linha_copia){
	        #pragma omp parallel for
	        for(int j = 0; j < re->cols; ++j){
	            re->data[i*re->cols+j] = in_re->data[linha_copia*in_re->cols+j];
	            im->data[i*im->cols+j] = in_im->data[linha_copia*in_im->cols+j];
	        }
	    }

	    /* Calcula a DFT ou IDFT */
	    fti(re, im, re, im, inv);

	    #pragma omp parallel for
	    for(int i = 0; i < re->rows; ++i, ++linha_preenche){
	        #pragma omp parallel for
	        for(int j = 0; j < re->cols; ++j){
	            in_re->data[linha_preenche*in_re->cols+j] = re->data[i*re->cols+j];
	            in_im->data[linha_preenche*in_im->cols+j] = im->data[i*im->cols+j];
	        }
	    }
    }
    
    free(re);
    free(im);
}