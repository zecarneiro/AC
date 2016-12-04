#include "funcs.h"

int acha_n_linhas(int, int);
void Envia_Dados(MEnv *, ImageF *, ImageF *, int , ImageF *, ImageF *);
void Recebe_Dados(MEnv *, ImageF *, ImageF *, int );

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
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Se for o processo mãe */
    if(rank == 0){
        struct timespec begin, end, dif;
        clock_gettime(CLOCK_MONOTONIC, &begin);

        /** cria mascara **/
        printf("\n\n========= MASCARA");
        out_mask=genlpfmask(imginf->rows,imginf->cols);
            
        /** calcula dft da imagem */
        printf("\n========= DFT");        
        MEnv->rows = imginf->rows;
        MEnv->cols = imginf->cols;
        MEnv->rows = acha_n_linhas(MEnv->rows,nprocs);

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

        /* É feito o calculos para as linhas */
        Envia_Dados(imginf, imgin, MEnv, nprocs, out_real, out_imag);
        Recebe_Dados(MEnv, out_real, out_imag, nprocs);

        /* Faço a transposta para de forma a se fazer os calculos para as colunas */
        transposta(out_real, out_imag);
        MEnv->rows = out_real->rows;
        MEnv->cols = out_real->cols;
        MEnv->rows = acha_n_linhas(MEnv->rows,nprocs);

        /* É feito o calculos para as colunas */
        Envia_Dados(imginf, imgin, MEnv, nprocs, out_real, out_imag);
        Recebe_Dados(MEnv, out_real, out_imag, nprocs);

        /* Faço outra vez a transposta de forma a que a matriz fique igual a original */
        transposta(out_real, out_imag);

        /** multiplica pela mascara */
        printf("\n========= FILTRAGEM");
        dofilt(out_real, out_imag, out_mask, auxiliar_real, auxiliar_im);

        /** calcula dft inversa da imagem filtrada */
        printf("\n========= IDFT\n");
        MEnv->rows = out_real->rows;
        MEnv->cols = out_real->cols;
        MEnv->rows = acha_n_linhas(MEnv->rows,nprocs);

        /* É feito o calculos para as linhas */
        Envia_Dados(imginf, imgin, MEnv, nprocs, out_real, out_imag);
        Recebe_Dados(MEnv, out_real, out_imag, nprocs);

        /* Faço a transposta para de forma a se fazer os calculos para as colunas */
        transposta(out_real, out_imag);
        MEnv->rows = out_real->rows;
        MEnv->cols = out_real->cols;
        MEnv->rows = acha_n_linhas(MEnv->rows,nprocs);

        /* É feito o calculos para as colunas */
        Envia_Dados(imginf, imgin, MEnv, nprocs, out_real, out_imag);
        Recebe_Dados(MEnv, out_real, out_imag, nprocs);

        /* Faço outra vez a transposta de forma a que a matriz fique igual a original */
        transposta(out_real, out_imag);

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
        /* Recebe os Dados do processo mãe */
        MPI_Recv(MEnv->matriz_re, MEnv->rows*MEnv->cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(MEnv->matriz_im, MEnv->rows*MEnv->cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&MEnv->rows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&MEnv->rows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&MEnv->inverso, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        
        /* Calcula a DFT ou IDFT */
        fti(MEnv->matriz_re, MEnv->matriz_im, MEnv->matriz_re, MEnv->matriz_im, MEnv->inverso);

        /* Envia os Dados do processo mãe */
        MPI_Send(MEnv->matriz_re, MEnv->rows*MEnv->cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        MPI_Send(MEnv->matriz_im, MEnv->rows*MEnv->cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&MEnv->rows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&MEnv->cols, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&MEnv->inverso, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();    
    return 0;
}

/* Função usada para se o bter o numero de linhas que cada
 * processo vai calcular */
void acha_n_linhas(int rows,int NUM_Proc){
    NUM_Proc -= 1;
    if((rows%NUM_Proc) == 0){
        rows = rows/NUM_Proc;
        
    }

    else{
        rows -= 1;
        
        while(1){
            if((rows%nprocs) == 0){
                rows = rows/nprocs;
                break;
            }

            rows -= 1;
        }
    }
    return rows;
}

void Envia_Dados(MEnv *m_env, ImageF *in_re, ImageF *in_im, int n_process, ImageF *ou_re, ImageF *ou_im){
    int dest,i,j,linha;

    linha = 0;
    for(dest = 1; dest < n_process; ++ dest){
        /* Fazemos a DFT para as linhas */
        for(i = 0; i < m_env->rows; ++i){
            
            /* Faço a copia das matrizes de entrada e armazeno nas matrizes
             * criadas por mim */
            #pragma omp parallel
            {
                #pragma omp for
                for(j = 0; j < in_re->cols; ++j){
                    m_env->matriz_re[i][j] = in_re->data[linha*in_re->cols+j];
                    m_env->matriz_im[i][j] = in_im->data[linha*in_im->cols+j];
                }
            }
            ++linha;
        }

        MPI_Send(m_env->matriz_re, m_env->rows*m_env->cols, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
        MPI_Send(m_env->matriz_im, m_env->rows*m_env->cols, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
        MPI_Send(&m_env->rows, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        MPI_Send(&m_env->cols, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        MPI_Send(&m_env->inverso, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
    }

    /* Caso ainda faltam linhas para fazer, então a matriz mãe
     * faz os calculos */
    if((in_re->rows - m_env->rows) > 0){
        int linhas_finais = ((n_process - 1)*m_env->rows) + 1;

        /* Preenche a matriz que vai ser usada */
        for(i = linhas_finais; i < in_re->rows; ++i){

            /* Faço a copia das matrizes de entrada e armazeno nas matrizes
             * criadas por mim */
            #pragma omp parallel
            {
                #pragma omp for
                for(j = 0; j < in_re->cols; ++j){
                    m_env->matriz_re[i][j] = in_re->data[i*in_re->cols+j];
                    m_env->matriz_im[i][j] = in_im->data[i*in_im->cols+j];
                }
            }
        }
        m_env->rows = in_re->rows - m_env->rows;

        /* Calcula a DFT */
        fti(m_env->matriz_re, m_env->matriz_im, m_env->matriz_re, m_env->matriz_im, m_env->inverso);

        /* Preenche a matriz de saida */
        for(i = linhas_finais; i < in_re->rows; ++i){

            /* Faço a copia das matrizes de entrada e armazeno nas matrizes
             * criadas por mim */
            #pragma omp parallel
            {
                #pragma omp for
                for(j = 0; j < in_re->cols; ++j){
                    ou_re->data[i*ou_re->cols+j] = m_env->matriz_re[i][j];
                    ou_im->data[i*ou_im->cols+j] = m_env->matriz_im[i][j];
                }
            }
        }
    }
}

void Recebe_Dados(MEnv *m_rec, ImageF *ou_re, ImageF *ou_im, int NUM_Proc){
    int dest,i,j,linha;

    for(dest = 1; dest < NUM_Proc; ++dest){
        MPI_Recv(m_rec->matriz_re, m_rec->rows*m_rec->cols, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(m_rec->matriz_im, m_rec->rows*m_rec->cols, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&m_rec->rows, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&m_rec->rows, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&m_rec->inverso, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, &status);

        if(dest == 1){
            linha = 0;
        }
        else{
            linha = dest*m_rec->rows + 1;
        }
        
        /* Fazemos a DFT para as linhas */
        for(i = 0; i < m_rec->rows; ++i){

            /* Faço a copia das matrizes de entrada e armazeno nas matrizes
             * criadas por mim */
            #pragma omp parallel
            {
                #pragma omp for
                for(j = 0; j < ou_re->cols; ++j){
                    ou_re->data[linha*ou_re->cols+j] = m_rec->matriz_re[i][j];
                    ou_im->data[linha*ou_im->cols+j] = m_rec->matriz_im[i][j];
                }
            }
            ++linha;
        }
    }
}