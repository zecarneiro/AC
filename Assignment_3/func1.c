#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "funcs.h"

ImageF * genlpfmask(int rows, int cols)
{
    //Definição da matriz de filtragem

    ImageF *matriz = NULL;

    matriz = malloc(sizeof(ImageF));
    
    matriz->rows = rows;
    matriz->cols = cols;
    matriz->widthStep = cols*sizeof(double);

    //Allocation cycle for filtering matrix

    matriz->data = (double *)malloc(sizeof(double)*rows*cols); //Aloca linhas

    //Array com posições de início de fim dos espaços brancos
    int position_rows[4] = {0, round(rows/4), rows-round((rows/4)), rows};
    int position_cols[4] = {0, round(cols/4), cols-round((cols/4)), cols};

    //Preenche Branco
    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            // Verifica se está dentro da zona branca
            if(r >= position_rows[0] && c >= position_cols[0] && r <= position_rows[1] && c <= position_cols[1] || //zona superior esquerda
            r >= position_rows[0] && c >= position_cols[2] && r <= position_rows[1] && c <= position_cols[3] || //zona superior direita
            r >= position_rows[2] && c >= position_cols[0] && r <= position_rows[3] && c <= position_cols[1] || //zona inferior esquerda
            r >= position_rows[2] && c >= position_cols[2] && r <= position_rows[3] && c <= position_cols[3]) //zona inferior direita
            {
                //  imginf->data[i*imginf->cols+j]=imgin->data[i*imgin->cols+j];
                matriz->data[r*cols+c] = 1; //preenche branco 
            }
            else
            {
                 matriz->data[r*cols+c] = 0; //preenche preto 
            }
                
        }   
    }
    return (matriz);
}

void dofilt(ImageF * in_re, ImageF * in_im, ImageF * mask, ImageF * out_re, ImageF * out_im)
{
    int rows = mask->rows;
    int cols = mask->cols;

    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            out_re->data[r*cols+c] = in_re->data[r*cols+c]*mask->data[r*cols+c];
            out_im->data[r*cols+c] = in_im->data[r*cols+c]*mask->data[r*cols+c];
        }   
    }
}   