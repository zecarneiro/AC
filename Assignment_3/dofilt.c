#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcs.h"


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