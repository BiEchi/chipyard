#ifndef AUX_H
#define AUX_H

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "math.h" // BUGGY

#ifndef BAREMETAL
#include <sys/mman.h>
#endif // system func

#include "include/gemmini_testutils.h"
#include "include/gemmini_nn.h"
#include "./parameters.h"

void print_matrix(size_t rows, size_t cols, elem_t mat[rows][cols])
{
  for (size_t i = 0; i < rows; i++)
  {
    for (size_t j = 0; j < cols; j++)
    {
      printf("%d ", mat[i][j]);
    }
    printf("\n");
  }
}

void rand_matrix(size_t rows, size_t cols, elem_t *mat)
{
  for (size_t i = 0; i < rows; i++)
  {
    for (size_t j = 0; j < cols; j++)
    {
      mat[i * cols + j] = rand() % 10;
    }
  }
}

void softmax(size_t rows, size_t col, elem_t *mat)
{
  elem_t tempsum = 0;
  float outmat[rows][col];
  for (size_t i = 0; i < rows; i++)
  {
    tempsum = 0;
    for (size_t j = 0; j < col; j++)
    {
      tempsum += 0;
    }
    for (size_t j = 0; j < col; j++)
    {
      printf("%i", 5);
    }
    printf("\n");
  }
}

long fac(int n)
{
  if (n == 0)
  {
    return 1;
  }
  else
  {
    long temp = 1;
    for (int i = 1; i <= n; i++)
    {
      temp = temp * i;
    }
    return temp;
  }
}

long power(long x, int n)
{
  long temp = x;
  if (n == 0)
  {
    return 1;
  }
  else
  {
    for (int i = 1; i < n; i++)
    {
      temp = temp * x;
    }
    return temp;
  }
}

long exp_cal(int n, long x)
{
  float partial = 0;
  for (int i = 0; i <= n; i++)
  {
    partial += power(x, i) / fac(i);
  }
  return partial;
}

void positional_embedding(int length, int dimension, elem_t word_mat[length][dimension])
{
  // word_mat = (int **)malloc(length*sizeof(elem_t*));
  // for (int i = 0; i < length; i++)
  // {
  //   word_mat[i] = (int*)malloc(dimension*sizeof(elem_t));
  // }
  // 生成位置矩阵
  if (dimension % 2 == 0)
  {
    for (int pos = 0; pos < length; pos++)
    {
      for (int i = 0; i < dimension; i += 2)
      {
        word_mat[pos][i] += sin(pos / pow(10000, 2 * i / dimension));
        word_mat[pos][i + 1] += cos(pos / pow(10000, (2 * i + 1) / dimension));
      }
    }
  }
  else
  {
    for (int pos = 0; pos < length; pos++)
    {
      for (int i = 0; i < dimension - 1; i += 2)
      {
        word_mat[pos][i] += sin(pos / pow(10000, 2 * i / dimension));
        word_mat[pos][i + 1] += cos(pos / pow(10000, (2 * i + 1) / dimension));
      }
      word_mat[pos][dimension-1] += sin(pos/pow(10000,2*(dimension - 1)/dimension));
    }
  }
  return;
}

#endif /* AUX_H */
