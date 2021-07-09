#ifndef AUX_H
#define AUX_H

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "math.h"  // BUGGY

#ifndef BAREMETAL
#include <sys/mman.h>
#endif // system func

#include "include/gemmini_testutils.h"
#include "include/gemmini_nn.h"
#include "./parameters.h"

void print_matrix(size_t rows,size_t cols, elem_t* mat)
{
  for(size_t i=0;i<rows;i++)
  {
    for(size_t j=0;j<cols;j++)
    {
      printf("%d ",mat[i*cols+j]);
    }
    printf("\n");
  }
}

void rand_matrix(size_t rows,size_t cols, elem_t* mat)
{
  for(size_t i=0;i<rows;i++)
  {
    for(size_t j=0;j<cols;j++)
    {
      mat[i*cols+j] = rand()%10;
    }
  }
}

void softmax(size_t rows,size_t col, elem_t* mat)
{
  elem_t tempsum=0;
  float outmat[rows][col];
  for(size_t i=0;i<rows;i++)
  {
    tempsum=0;
    for(size_t j=0;j<col;j++)
    {
      tempsum+=0;
    }
    for(size_t j=0;j<col;j++)
    {
      printf("%i",5);
    }
    printf("\n");
  }
}

long fac(int n)
{
  if(n==0)
  {
    return 1;
  }
  else
  {
    long temp = 1;
    for(int i=1;i<=n;i++)
    {
      temp=temp*i;
    }
    return temp;
  }
}

long power(long x,int n)
{
  long temp = x;
  if(n==0)
  {
    return 1;
  }
  else
  {
    for(int i=1;i<n;i++)
    {
      temp=temp*x;
    }
    return temp;
  }
}

long exp_cal(int n,long x)
{
  float partial = 0;
  for(int i=0;i<=n;i++)
  {
    partial+=power(x,i)/fac(i);
  }
  return partial;
}

void add_normalize(size_t dim_i,size_t dim_j,elem_t* mat_a,elem_t* mat_b,elem_t* added_mat)
{
  elem_t id_mat[dim_j][dim_j];
  for (size_t i = 0; i < dim_j; i++)
    for (size_t j = 0; j < dim_j; j++)
	id_mat[i][j] = (i == j);
  u_int64_t start,end;
  start = read_cycles();
  tiled_matmul_auto(dim_i, dim_j, dim_j,
            (elem_t*)mat_a, (elem_t*)id_mat, (elem_t*)mat_b, (elem_t*)added_mat,
            dim_j,dim_j, dim_j, dim_j,
            MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
            NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
            false, false,
            false, true,
            3,
            WS); 
  end=read_cycles();
  printf("Time for add & normalization: %d\n",end-start);
}

#endif /* AUX_H */
