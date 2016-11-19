#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcs.h"

ImageF * fmask genlpfmask(int rows, int cols)
{
    double matriz[rows,cols]; // matriz de filtragem

    //Array com posições de início de fim dos espaços brancos
    int positions_rows[4] = {0, rows/4, rows-(rows/4), rows}
    int positions_cols[4] = {0, cols/4, cols-(cols/4), cols}

    //Preenche Branco
    for(int r = 0; r < rows, r++)
    {
        for(int c = 0; c < cols; c++)
        {
            // Verifica se está dentro da zona branca
            if(r >= position_rows[0] && c >= position_cols[0] && r <= position_rows[1] && c <= position_cols[1] || //zona superior esquerda
            r >= position_rows[0] && c >= position_cols[2] && r <= position_rows[1] && c <= position_cols[3] || //zona superior direita
            r >= position_rows[2] && c >= position_cols[0] && r <= position_rows[3] && c <= position_cols[1] || //zona inferior esquerda
            r >= position_rows[2] && c >= position_cols[2] && r <= position_rows[3] && c <= position_cols[3]) //zona inferior direita
            {
                matriz[r,c] = 1; //preenche branco 
            }
            else
            {
                matriz[r,c] = 0; //preenche preto 
            }
                
        }   
    }
    return (matriz);
}