#include "funcs.h"

int main(int argc, char**argv){
	/* Variáveis usadas para se obter o numero de processos
     * como também o ID dos processos */
    int rank,nprocs;
    MPI_Status status;
    MPI_Request request;

    /* Inicio do MPI */
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

    /* Se for o processo mãe */
    if(rank == 0){
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

	    /********************************************************************************/
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

	    /********************************************************************************/
	    /* Estamos a criar as estruturas para as saidas necessárias para as funções
	     * referidas no enunciado e feitas por nós, sendo
	     * out_imag = matriz com valores imaginarios de saida
	     * out_real = matriz com valores reais de saida
	     * out_mask = Mascar
	     * auxiliar_real = matriz com os valores reis da filtragem
	     * auxiliar_im = matriz com os valores imaginario da filtragem */
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
	    /********************************************************************************/

	    struct timespec begin, end, dif;
        clock_gettime(CLOCK_MONOTONIC, &begin);

	    int inverso;

	    /* Minhas Variáveis */
	    int num_linha, num_coluna;
	    num_linha = acha_n_linhas(imginf->rows,nprocs);
	    num_coluna = acha_n_linhas(imginf->cols, nprocs);

        /** cria mascara **/
	    printf("\n\n========= MASCARA");
	    out_mask=genlpfmask(imginf->rows,imginf->cols);
        
        /**************************************************************
         *************************** DFT *****************************/
        printf("\n========= DFT");
        inverso = 0;

        /* É feito o calculos para as linhas */
        printf("\n-> Para as linhas");
        Envia_Dados(imginf, imgin_img, out_real, out_imag, inverso, nprocs, num_linha, &status, &request);
        Recebe_Dados(out_real, out_imag, nprocs, num_linha, &status, &request);

        printf("\n-> Transposta");
        transposta(out_real, out_imag);

        /* É feito o calculos para as colunas */
        printf("\n-> Para as colunas");
		Envia_Dados(out_real, out_imag, out_real, out_imag, inverso, nprocs, num_coluna, &status, &request);
		Recebe_Dados(out_real, out_imag, nprocs, num_coluna, &status, &request);

		printf("\n-> Transposta");
		transposta(out_real, out_imag);

        /**************************************************************
         *************************** FILTRAGEM ***********************/
        printf("\n========= FILTRAGEM");
        dofilt(out_real, out_imag, out_mask, auxiliar_real, auxiliar_im);

        /**************************************************************
         *************************** IDFT ****************************/
        printf("\n========= IDFT");
        inverso = 1;

        /* É feito o calculos para as linhas */
        printf("\n-> Para as linhas");
		Envia_Dados(auxiliar_real, auxiliar_im, out_real, out_imag, inverso, nprocs, num_linha, &status, &request);
		Recebe_Dados(out_real, out_imag, nprocs, num_linha, &status, &request);

		printf("\n-> Transposta");
		transposta(out_real, out_imag);

        /* É feito o calculos para as colunas */
        printf("\n-> Para as colunas");
		Envia_Dados(out_real, out_imag, out_real, out_imag, inverso, nprocs, num_coluna, &status, &request);
		Recebe_Dados(out_real, out_imag, nprocs, num_coluna, &status, &request);

		printf("\n-> Transposta");
		transposta(out_real, out_imag);

        /**************************************************************
         *************************** OUTROS **************************/

        clock_gettime(CLOCK_MONOTONIC, &end);
        dif = SubtracaoTempo(begin,end);
        printf("\nTEMPO DE EXECUÇÃO = %lld.%.9ld\n", (long long) dif.tv_sec, dif.tv_nsec);

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
        return 0;
    }

    /* Se for outros processos */
    if(rank != 0){
    	int inverso;

    	ImageF *matriz_real;
    	ImageF *matriz_imag;
    	matriz_real = (ImageF*)malloc(sizeof(ImageF));
    	matriz_imag = (ImageF*)malloc(sizeof(ImageF));
    	matriz_real->data = NULL;
    	matriz_imag->data = NULL;

	    int i = 0;
    	while(1){
    		/* Recebe os Dados do processo mãe */
	    	MPI_Irecv(&matriz_real->rows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
	    	MPI_Wait(&request, &status);
	    	MPI_Irecv(&matriz_real->cols, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &request);
	    	MPI_Wait(&request, &status);

	    	matriz_imag->rows = matriz_real->rows;
    		matriz_imag->cols = matriz_real->cols;

    		matriz_real->data = (double *)malloc(matriz_real->rows*matriz_real->cols*sizeof(double));
	    	matriz_imag->data = (double *)malloc(matriz_imag->rows*matriz_imag->cols*sizeof(double));

	    	/* Recebe a informação se é para fazer a DFT ou a IDFT */
	    	MPI_Irecv(&inverso, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &request);
	    	MPI_Wait(&request, &status);

	        /* Nesta parte pretendo criar uma matriz bidimensional
		     * Reservo espaço para uma matriz Nx1, nesse caso as matrizes re e im */
	    	MPI_Irecv(matriz_real->data, matriz_real->rows*matriz_real->cols, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD, &request);
	    	MPI_Wait(&request, &status);

	        MPI_Irecv(matriz_imag->data, matriz_imag->rows*matriz_imag->cols, MPI_DOUBLE, 0, 4, MPI_COMM_WORLD, &request);
	        MPI_Wait(&request, &status);	     
	        
	        /* Calcula a DFT ou IDFT */
	        fti(matriz_real, matriz_imag, matriz_real, matriz_imag, inverso);

	        /* Envia a matriz real */
	        MPI_Isend(matriz_real->data, matriz_real->rows*matriz_real->cols, MPI_DOUBLE, 0, 6, MPI_COMM_WORLD, &request);
	        MPI_Wait(&request, &status);

	        MPI_Isend(matriz_imag->data, matriz_imag->rows*matriz_imag->cols, MPI_DOUBLE, 0, 7, MPI_COMM_WORLD, &request);
	        MPI_Wait(&request, &status);

	        matriz_real->data = NULL;
	        matriz_imag->data = NULL;
    	}
    }
    MPI_Finalize();   
}