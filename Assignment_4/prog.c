#include "funcs.h"

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

    struct timespec begin, end, dif;
    int rank,nprocs,dest;

    clock_gettime(CLOCK_MONOTONIC, &begin);

    /** cria mascara **/
    printf("\n\n========= MASCARA");
    out_mask=genlpfmask(imginf->rows,imginf->cols);
        
    /** calcula dft da imagem */
    printf("\n========= DFT");

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

    /* Testa para verificar se a divisão por numero de processos
     * é par ou impar */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == 0){
        MEnv->rows = imginf->rows;

        if((MEnv->rows%nprocs) == 0){
            MEnv->rows = MEnv->rows/nprocs;
            MEnv->cols = imginf->cols;
        }
        else{
            MEnv->rows -= 1;
            
            while(1){
                if((imginf->rows%nprocs) == 0){
                    MEnv->rows = MEnv->rows/nprocs;
                    MEnv->cols = imginf->cols;
                    break;
                }

                MEnv->rows -= 1;
            }
        }

        /* Nesta parte pretendo criar uma matriz bidimensional */
        /* Reservo espaço para uma matriz Nx1, nesse caso as matrizes re e im */
        MEnv->matriz_re = (double**)malloc(MEnv->rows*sizeof(double*));
        MEnv->matriz_im = (double**)malloc(MEnv->rows*sizeof(double*));

        #pragma omp parallel
        {
            #pragma omp for
            for(i = 0; i < imginf->rows; ++i){
                MEnv->matriz_re[i] = (double*)malloc(MEnv->cols*sizeof(double));
                MEnv->matriz_im[i] = (double*)malloc(MEnv->cols*sizeof(double));
            }
        }

        for(dest = 1; dest < nprocs; ++ dest){
            /* Fazemos a DFT para as linhas */
            for(i = 0; i < MEnv->rows; ++i){
                /* Faço a copia das matrizes de entrada e armazeno nas matrizes
                 * criadas por mim */
                for(j = 0; j < imginf->cols; ++j){
                    MEnv->matriz_re[i][j] = imginf->data[i*imginf->cols+j];
                    MEnv->matriz_im[i][j] = imgin_img->data[i*imginf->cols+j];
                }
            }

            MPI_Send(MEnv->matriz_re, MEnv->rows*MEnv->cols, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
            MPI_Send(MEnv->matriz_im, MEnv->rows*MEnv->cols, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
            MPI_Send(&MEnv->rows, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            MPI_Send(&MEnv->cols, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            MPI_Send(&MEnv->inverso, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }

        for(dest = 1; dest < nprocs; ++dest){
            MPI_Recv(MEnv->matriz_re, MEnv->rows*MEnv->cols, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(MEnv->matriz_im, MEnv->rows*MEnv->cols, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&MEnv->rows, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&MEnv->rows, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&MEnv->inverso, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, &status);
        }

        if((MEnv->rows - imginf->rows) > 0){
            for()
        }

        transposta(out_real, out_imag);
    }

    if(rank != 0){
        MPI_Recv(MEnv->matriz_re, MEnv->rows*MEnv->cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(MEnv->matriz_im, MEnv->rows*MEnv->cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&MEnv->rows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&MEnv->rows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&MEnv->inverso, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        
        /* Calcula a DFT */
        fti(MEnv->matriz_re, MEnv->matriz_im, MEnv->matriz_re, MEnv->matriz_im, MEnv->inverso);

        MPI_Send(MEnv->matriz_re, MEnv->rows*MEnv->cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        MPI_Send(MEnv->matriz_im, MEnv->rows*MEnv->cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&MEnv->rows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&MEnv->cols, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&MEnv->inverso, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    fti(out_real, out_imag, out_real, out_imag, 0);
    transposta(out_real, out_imag);

    /** multiplica pela mascara */
    printf("\n========= FILTRAGEM");
    dofilt(out_real, out_imag, out_mask, auxiliar_real, auxiliar_im);

    /** calcula dft inversa da imagem filtrada */
    printf("\n========= IDFT\n");
    fti(auxiliar_real, auxiliar_im, out_real, out_imag, 1);
    transposta(out_real, out_imag);
    fti(out_real, out_imag, out_real, out_imag, 1);
    transposta(out_real, out_imag);

    MPI_Finalize();

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
    
    return 0;
}

void Envia_Dados(){

}