#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "defs.h"
#include "string.h"

#define PINWHEEL(dest_idx, src_idx) dest[dest_idx].red = dest[dest_idx].green = dest[dest_idx].blue = \
  ((int)src[src_idx].red + src[src_idx].green + src[src_idx].blue) / 3

char naive_pinwheel_descr[] = "naive_pinwheel: baseline implementation";
void naive_pinwheel(pixel *src, pixel *dest)
{
  int i, j;

  for (i = 0; i < src->dim; i++)
    for (j = 0; j < src->dim; j++) {
      if ((fabs(i + 0.5 - src->dim/2) + fabs(j + 0.5 - src->dim/2)) < src->dim/2) {
        int s_idx = RIDX(i, j, src->dim);
        int d_idx = RIDX(src->dim - j - 1, i, src->dim);
        dest[d_idx].red = ((int)src[s_idx].red + src[s_idx].green + src[s_idx].blue) / 3;
        dest[d_idx].green = ((int)src[s_idx].red + src[s_idx].green + src[s_idx].blue) / 3;
        dest[d_idx].blue = ((int)src[s_idx].red + src[s_idx].green + src[s_idx].blue) / 3;
      } else {
        int s_idx = RIDX(i, j, src->dim);
        int d_idx = RIDX(i, j, src->dim);
        dest[d_idx] = src[s_idx];
      }
    }
}


char pinwheel_descr[] = "pinwheel: Current working version";
inline void pinwheel(pixel *src, pixel *dest) {
  int i, j,k, dim, half_dim, index;
  dim = src->dim;
  half_dim = dim >> 1;
  //top left
  for (i = 0; i < half_dim; i++)
    for (j = 0,k=1; j < half_dim; j+=2,k+=2) {
      index = RIDX(i, j, dim);
      if(i + j + 1 > half_dim) PINWHEEL(RIDX(dim - j - 1, i, dim),index);
      else dest[index] = src[index];
      if(i + k + 1 > half_dim) PINWHEEL(RIDX(dim - k - 1, i, dim),index+1);
      else dest[index+1] = src[index+1];
    }
  //bottom left
  for (i = half_dim; i < dim; i++)
    for (j = 0,k=1; j < half_dim; j+=2, k+=2) {
      index = RIDX(i, j, dim);
      if (i-j < half_dim) PINWHEEL(RIDX(dim - j - 1, i, dim),index);
      else dest[index] = src[index];
      if (i-k < half_dim) PINWHEEL(RIDX(dim - k - 1, i, dim),index+1);
      else dest[index+1] = src[index+1];
    }
  //top right
  for (i = 0; i < half_dim; i++)
    for (j = half_dim,k= half_dim + 1; j < dim; j+=2, k+=2) {
      index = RIDX(i, j, dim);
        if (j-i < half_dim) PINWHEEL(RIDX(dim - j - 1, i, dim),index);
        else dest[index] = src[index];
        if (k-i < half_dim) PINWHEEL(RIDX(dim - k - 1, i, dim),index+1);
        else dest[index+1] = src[index+1];
    }
  //bottom right
  for (i = half_dim; i < dim; i++)
    for (j = half_dim,k=half_dim + 1; j < dim; j+=2, k+=2) {
      index = RIDX(i, j, dim);
      if (i+j + 1< dim + half_dim) PINWHEEL(RIDX(dim - j - 1, i, dim),index);
      else dest[index] = src[index];
      if (i+k + 1< dim + half_dim) PINWHEEL(RIDX(dim - k - 1, i, dim),index+1);
      else dest[index+1] = src[index+1];
    }
}

void register_pinwheel_functions() {
  add_pinwheel_function(&pinwheel, pinwheel_descr);
  add_pinwheel_function(&naive_pinwheel, naive_pinwheel_descr);
}


/***************************************************************
 * GLOW KERNEL
 **************************************************************/

typedef struct {
  int red;
  int green;
  int blue;
} pixel_sum;
static void initialize_pixel_sum(pixel_sum *sum)
{
  sum->red = sum->green = sum->blue = 0;
}
static void accumulate_weighted_sum(pixel_sum *sum, pixel p, double weight)
{
  sum->red += (int) p.red * weight;
  sum->green += (int) p.green * weight;
  sum->blue += (int) p.blue * weight;
}
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum)
{
  current_pixel->red = (unsigned short)sum.red;
  current_pixel->green = (unsigned short)sum.green;
  current_pixel->blue = (unsigned short)sum.blue;
}
static pixel weighted_combo(int dim, int i, int j, pixel *src)
{
  int ii, jj;
  pixel_sum sum;
  pixel current_pixel;
  double weights[3][3] = { { 0.16, 0.00, 0.16 },
                           { 0.00, 0.30, 0.00 },
                           { 0.16, 0.00, 0.16 } };

  initialize_pixel_sum(&sum);
  for (ii=-1; ii < 2; ii++)
    for (jj=-1; jj < 2; jj++)
      if ((i + ii >= 0) && (j + jj >= 0) && (i + ii < dim) && (j + jj < dim))
        accumulate_weighted_sum(&sum,
                                src[RIDX(i+ii,j+jj,dim)],
                                weights[ii+1][jj+1]);

  assign_sum_to_pixel(&current_pixel, sum);

  return current_pixel;
}

/******************************************************
 * Your different versions of the glow kernel go here
 ******************************************************/

char naive_glow_descr[] = "naive_glow: baseline implementation";
void naive_glow(pixel *src, pixel *dst)
{
  int i, j;

  for (i = 0; i < src->dim; i++)
    for (j = 0; j < src->dim; j++)
      dst[RIDX(i, j, src->dim)] = weighted_combo(src->dim, i, j, src);
}

inline void n_accumulate_weighted_sum(pixel_sum *sum, pixel p)
{
  sum->red +=  (p.red << 2) / 25;
  sum->green +=  (p.green << 2) / 25;
  sum->blue +=  (p.blue << 2) / 25;
}

char glow_descr[] = "glow: Current working version";
void glow(pixel *src, pixel *dst) {
  int i, j, dim;
  pixel_sum sum;
  pixel current_pix;
  dim = src->dim;
  for (i = 0; i < dim; i++)
    for (j = 0; j < dim; j++) {
      sum.red = sum.green = sum.blue = 0;
      pixel p = src[RIDX(i, j, dim)];
      sum.red = p.red * .30;
      sum.blue = p.blue * .30;
      sum.green = p.green * .30;
      if(i < dim-1){
        if(j < dim -1) n_accumulate_weighted_sum(&sum, src[RIDX(i + 1, j+1, dim)]);
        if(j > 0) n_accumulate_weighted_sum(&sum, src[RIDX(i + 1, j-1, dim)]);
      }
      if(i > 0){
        if(j < dim -1) n_accumulate_weighted_sum(&sum, src[RIDX(i - 1, j+1, dim)]);
        if(j > 0) n_accumulate_weighted_sum(&sum, src[RIDX(i - 1, j-1, dim)]);
      }
      current_pix.red = sum.red;
      current_pix.green = sum.green;
      current_pix.blue = sum.blue;
      dst[RIDX(i, j, dim)] = current_pix;
  }
}

void register_glow_functions() {
  add_glow_function(&glow, glow_descr);
  add_glow_function(&naive_glow, naive_glow_descr);
}
