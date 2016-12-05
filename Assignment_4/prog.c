#include "funcs.h"

void aloca_matriz(ImageF *, ImageF *, int, int, int);
int acha_n_linhas(int, int);
void Envia_Dados(ImageF *, ImageF *, ImageF *, ImageF *, int, int, ImageF *, ImageF *);
void Recebe_Dados(ImageF *, ImageF *, ImageF *, ImageF *, int, MPI_Status *);

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
        }
    }

    /** processamento */
    /* Estamos a criar uma matriz com numeros imaginarios
     * Como a variavel imginf é que contém a informação original, então a vou usar */
    ImageF *imgin_img = NULL;
    imgin_img=(ImageF*)malloc(sizeof(ImageF));
    imgin_img->rows=imginf->rows;
    imgin_img->cols=imginf->cols;
    imgin_img->widthStep=imginf->cols*sizeof(double);
    imgin_img->data=(double *)malloc(imgin_img->rows*imgin_img->cols*sizeof(double));

    for (i=0;i<imgin_img->rows;i++){
        for(j=0;j<imgin_img->cols;j++){
            imgin_img->data[i*imgin_img->cols+j]=0.0;
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
    out_imag->widthStep=imginf->cols*sizeof(double);
    out_imag->data=(double *)malloc(out_imag->rows*out_imag->cols*sizeof(double));

    ImageF *out_real = NULL;
    out_real=(ImageF*)malloc(sizeof(ImageF));
    out_real->rows=imginf->rows;
    out_real->cols=imginf->cols;
    out_real->widthStep=imginf->cols*sizeof(double);
    out_real->data=(double *)malloc(out_real->rows*out_real->cols*sizeof(double));

    ImageF *auxiliar_real = NULL;
    auxiliar_real=(ImageF*)malloc(sizeof(ImageF));
    auxiliar_real->rows=imginf->rows;
    auxiliar_real->cols=imginf->cols;
    auxiliar_real->widthStep=imginf->cols*sizeof(double);
    auxiliar_real->data=(double *)malloc(out_real->rows*out_real->cols*sizeof(double));

    ImageF *auxiliar_im = NULL;
    auxiliar_im=(ImageF*)malloc(sizeof(ImageF));
    auxiliar_im->rows=imginf->rows;
    auxiliar_im->cols=imginf->cols;
    auxiliar_im->widthStep=imginf->cols*sizeof(double);
    auxiliar_im->data=(double *)malloc(out_real->rows*out_real->cols*sizeof(double));

    ImageF *out_mask = NULL;

    /* Variáveis usadas para se obter o numero de processos
     * como também o ID dos processos */
    int rank,nprocs;

    /* Inicio do MPI */
    MPI_Status status;
    ImageF *matriz_re;
    ImageF *matriz_im;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Se for o processo mãe */
    if(rank == 0){
    	printf("\n\n%d\n\n\n",nprocs);
    	int inverso;

        struct timespec begin, end, dif;
        clock_gettime(CLOCK_MONOTONIC, &begin);

        /** cria mascara **/
	    printf("\n\n========= MASCARA");
	    out_mask=genlpfmask(imginf->rows,imginf->cols);
        
        /**************************************************************
         *************************** DFT *****************************/

        /** calcula dft da imagem */
        printf("\n========= DFT");
        inverso = 0;
        aloca_matriz(matriz_re, matriz_im, imginf->rows, imginf->cols, nprocs);

        /* É feito o calculos para as linhas */
        Envia_Dados(imginf, imgin_img, matriz_re, matriz_im, inverso, nprocs, out_real, out_imag);
        Recebe_Dados(matriz_re, matriz_im, out_real, out_imag, nprocs, &status);

        /* Faço a transposta para de forma a se fazer os calculos para as colunas */
        transposta(out_real, out_imag);
        aloca_matriz(matriz_re, matriz_im, out_real->rows, out_real->cols, nprocs);

        /* É feito o calculos para as colunas */
        Envia_Dados(imginf, imgin_img, matriz_re, matriz_im, inverso, nprocs, out_real, out_imag);
        Recebe_Dados(matriz_re, matriz_im, out_real, out_imag, nprocs, &status);
        
        /* Faço outra vez a transposta de forma a que a matriz fique igual a original */
        transposta(out_real, out_imag);

        /**************************************************************
         *************************** FILTRAGEM ***********************/

        /** multiplica pela mascara */
        printf("\n========= FILTRAGEM");
        dofilt(out_real, out_imag, out_mask, auxiliar_real, auxiliar_im);

        /**************************************************************
         *************************** IDFT ****************************/

        /** calcula dft inversa da imagem filtrada */
        printf("\n========= IDFT\n");
        inverso = 1;
        aloca_matriz(matriz_re, matriz_im, out_real->rows, out_real->cols, nprocs);

        /* É feito o calculos para as linhas */
        Envia_Dados(imginf, imgin_img, matriz_re, matriz_im, inverso, nprocs, out_real, out_imag);
        Recebe_Dados(matriz_re, matriz_im, out_real, out_imag, nprocs, &status);

        /* Faço a transposta para de forma a se fazer os calculos para as colunas */
        transposta(out_real, out_imag);
        aloca_matriz(matriz_re, matriz_im, out_real->rows, out_real->cols, nprocs);

        /* É feito o calculos para as colunas */
        Envia_Dados(imginf, imgin_img, matriz_re, matriz_im, inverso, nprocs, out_real, out_imag);
        Recebe_Dados(matriz_re, matriz_im, out_real, out_imag, nprocs, &status);

        /* Faço outra vez a transposta de forma a que a matriz fique igual a original */
        transposta(out_real, out_imag);

        /**************************************************************
         *************************** OUTROS **************************/

        clock_gettime(CLOCK_MONOTONIC, &end);
        dif = SubtracaoTempo(begin,end);
        printf("TEMPO DE EXECUÇÃO = %lld.%.9ld\n", (long long) dif.tv_sec, dif.tv_nsec);

        /** copia para imagem de saida imgout */
        double val_re,val_img,val;
        for (i=0;i<out_real->rows;i++){
            for(j=0;j<out_real->cols;j++){     

                val = out_real->data[i*out_real->cols+j];

                if (val<0)
                    val=0.0;
                else if (val>255)
                    val=255.0;

                imgout->data[i*imgout->cols+j]=(unsigned char)val;
            }  
        }

        savePBM(fnameout,imgout);
    }

    /* Se for outros processos */
    if(rank != 0){
    	int inverso;

    	while(1){
    		/* Recebe os Dados do processo mãe */

	    	/* Recebe da matriz real */
	    	MPI_Recv(&matriz_re->rows, 1, MPI_INT, 0, 99, MPI_COMM_WORLD, &status);
	        MPI_Recv(&matriz_re->cols, 1, MPI_INT, 0, 98, MPI_COMM_WORLD, &status);

	        /* Nesta parte pretendo criar uma matriz bidimensional
		     * Reservo espaço para uma matriz Nx1, nesse caso as matrizes re e im */
		    matriz_re->data = (double*)malloc(matriz_re->rows*matriz_re->cols*sizeof(double));
	        MPI_Recv(matriz_re->data, matriz_re->rows*matriz_re->cols, MPI_DOUBLE, 0, 97, MPI_COMM_WORLD, &status);

	        /* Recebe da matriz imaginaria */
	        MPI_Recv(&matriz_im->rows, 1, MPI_INT, 0, 96, MPI_COMM_WORLD, &status);
	        MPI_Recv(&matriz_im->cols, 1, MPI_INT, 0, 95, MPI_COMM_WORLD, &status);

	        /* Nesta parte pretendo criar uma matriz bidimensional
		     * Reservo espaço para uma matriz Nx1, nesse caso as matrizes re e im */
		    matriz_im->data = (double*)malloc(matriz_im->rows*matriz_im->cols*sizeof(double));
	        MPI_Recv(matriz_im->data, matriz_im->rows*matriz_im->cols, MPI_DOUBLE, 0, 94, MPI_COMM_WORLD, &status);

	        /* Recebe a informação se é para fazer a DFT ou a IDFT */
	        MPI_Recv(&inverso, 1, MPI_INT, 0, 93, MPI_COMM_WORLD, &status);
	        
	        /* Calcula a DFT ou IDFT */
	        fti(matriz_re, matriz_im, matriz_re, matriz_im, inverso);

	        /* Envia os Dados do processo mãe */

	        /* Envia a matriz real */
	        MPI_Send(matriz_re->data, matriz_re->rows*matriz_re->cols, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD);
	        MPI_Send(&matriz_re->rows, 1, MPI_INT, 0, 98, MPI_COMM_WORLD);
	        MPI_Send(&matriz_re->cols, 1, MPI_INT, 0, 97, MPI_COMM_WORLD);

	        /* Envia a matriz imaginaria */
	        MPI_Send(matriz_im->data, matriz_im->rows*matriz_im->cols, MPI_DOUBLE, 0, 96, MPI_COMM_WORLD);
	        MPI_Send(&matriz_im->rows, 1, MPI_INT, 0, 95, MPI_COMM_WORLD);
	        MPI_Send(&matriz_im->cols, 1, MPI_INT, 0, 94, MPI_COMM_WORLD);

	        printf("\n%d\n",rank);

	        matriz_re->data = NULL;
	        matriz_im->data = NULL;

	        if(inverso == 1)
	        	break;
    	}
    }

    MPI_Finalize();    
    return 0;
}

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

void aloca_matriz(ImageF *in_re, ImageF *in_im, int rows, int cols, int NUM_Proc){
	in_re->data = NULL;
	in_im->data = NULL;

    in_re->rows = rows;
    in_re->cols = cols;
    in_re->rows = acha_n_linhas(in_re->rows,NUM_Proc);

    in_im->rows = in_re->rows;
    in_im->cols = cols;        

    /* Nesta parte pretendo criar uma matriz bidimensional
     * Reservo espaço para uma matriz Nx1, nesse caso as matrizes re e im */
    in_re->data = (double*)malloc(in_re->rows*in_re->cols*sizeof(double));
    in_im->data = (double*)malloc(in_im->rows*in_im->cols*sizeof(double));
}

void Envia_Dados(ImageF *in_re, ImageF *in_im, ImageF *env_re, ImageF *env_im, int inv, int n_process, ImageF *ou_re, ImageF *ou_im){
    int dest,i,j,linha,linhas_finais;

    linha = 0;
    for(dest = 1; dest < n_process; ++dest){
        /* Fazemos a DFT para as linhas */
        for(i = 0; i < env_re->rows; ++i){            
            /* Faço a copia das matrizes de entrada e armazeno nas matrizes
             * criadas por mim */
            #pragma omp parallel
            {
                #pragma omp for
                for(j = 0; j < in_re->cols; ++j){
                    env_re->data[i*env_re->cols+j] = in_re->data[linha*in_re->cols+j];
                    env_im->data[i*env_im->cols+j] = in_im->data[linha*in_im->cols+j];
                }
            }
            linha++;
        } 

        /* Envia a matriz real */
        MPI_Send(&env_re->rows, 1, MPI_INT, dest, 99, MPI_COMM_WORLD);
        MPI_Send(&env_re->cols, 1, MPI_INT, dest, 98, MPI_COMM_WORLD);
        MPI_Send(env_re->data, env_re->rows*env_re->cols, MPI_DOUBLE, dest, 97, MPI_COMM_WORLD);       

        /* Envia a matriz imaginaria */
        MPI_Send(&env_im->rows, 1, MPI_INT, dest, 96, MPI_COMM_WORLD);
        MPI_Send(&env_im->cols, 1, MPI_INT, dest, 95, MPI_COMM_WORLD);
        MPI_Send(env_im->data, env_im->rows*env_im->cols, MPI_DOUBLE, dest, 94, MPI_COMM_WORLD);

        /* Envia a informação se é para fazer a DFT ou a IDFT */
        MPI_Send(&inv, 1, MPI_INT, dest, 93, MPI_COMM_WORLD);
    }

    printf("passou\n");

    /* A Computador principal vai calcular a DFT ou a IDFT das linhas
     * que faltam */  
    linhas_finais = in_re->rows - linha;
    
    env_re->data = NULL;
    env_im->data = NULL;
    env_re->rows = linhas_finais;
    env_re->cols = in_re->cols;
    env_im->rows = env_re->rows;
    env_im->cols = in_im->cols;
    
    /* Nesta parte pretendo criar uma matriz bidimensional
     * Reservo espaço para uma matriz Nx1, nesse caso as matrizes re e im */
    env_re->data = (double*)malloc(in_re->rows*in_re->cols*sizeof(double));
    env_im->data = (double*)malloc(in_im->rows*in_im->cols*sizeof(double));

    linhas_finais = linha;

    /* Preenche a matriz que vai ser usada */
    for(i = 0; i < env_re->rows; ++i){

        /* Faço a copia das matrizes de entrada e armazeno nas matrizes
         * criadas por mim */
        #pragma omp parallel
        {
            #pragma omp for
            for(j = 0; j < in_re->cols; ++j){
                env_re->data[i*env_re->cols+j] = in_re->data[linha*in_re->cols+j];
                env_im->data[i*env_im->cols+j] = in_im->data[linha*in_im->cols+j];
            }
        }
        linha++;
    }

    /* Calcula a DFT ou a IDFT */
    fti(env_re, env_im, env_re, env_im, inv);

    linha = linhas_finais;
    /* Preenche a matriz de saida */
    for(i = 0; i < in_re->rows; ++i){

        /* Faço a copia das matrizes de entrada e armazeno nas matrizes
         * criadas por mim */
        #pragma omp parallel
        {
            #pragma omp for
            for(j = 0; j < ou_re->cols; ++j){
                ou_re->data[linha*ou_re->cols+j] = env_re->data[i*env_re->cols+j];
                ou_im->data[linha*ou_im->cols+j] = env_im->data[i*env_im->cols+j];
            }
        }
        linha++;
    }
}

void Recebe_Dados(ImageF *rec_re, ImageF *rec_im, ImageF *ou_re, ImageF *ou_im, int NUM_Proc, MPI_Status *status){
    int dest,i,j,linha;

    for(dest = 1; dest < NUM_Proc; ++dest){
    	/* Recebe a matriz real */
        MPI_Recv(rec_re->data, rec_re->rows*rec_re->cols, MPI_DOUBLE, dest, 99, MPI_COMM_WORLD, status);
        MPI_Recv(&rec_re->rows, 1, MPI_INT, dest, 98, MPI_COMM_WORLD, status);
        MPI_Recv(&rec_re->cols, 1, MPI_INT, dest, 97, MPI_COMM_WORLD, status);

        /* Recebe a matriz imaginaria */
        MPI_Recv(rec_im->data, rec_re->rows*rec_re->cols, MPI_DOUBLE, dest, 96, MPI_COMM_WORLD, status);
        MPI_Recv(&rec_im->rows, 1, MPI_INT, dest, 95, MPI_COMM_WORLD, status);
        MPI_Recv(&rec_im->cols, 1, MPI_INT, dest, 94, MPI_COMM_WORLD, status);

        if(dest == 1){
            linha = 0;
        }
        else{
            linha = dest*rec_re->rows + 1;
        }
        
        /* Fazemos a DFT para as linhas */
        for(i = 0; i < rec_re->rows; ++i){

            /* Faço a copia das matrizes de entrada e armazeno nas matrizes
             * criadas por mim */
            #pragma omp parallel
            {
                #pragma omp for
                for(j = 0; j < ou_re->cols; ++j){
                    ou_re->data[linha*ou_re->cols+j] = rec_re->data[i*rec_re->cols+j];
                    ou_im->data[linha*ou_im->cols+j] = rec_im->data[i*rec_im->cols+j];
                }
            }
            linha++;
        }
    }
}