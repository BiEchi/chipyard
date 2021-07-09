#ifndef AUX_H
#define AUX_H

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif // system func

#include "include/gemmini_testutils.h"
#include "include/gemmini_nn.h"
#include "./parameters.h"

void print_matrix(size_t rows, size_t cols, elem_t *mat)
{
  for (size_t i = 0; i < rows; i++)
  {
    for (size_t j = 0; j < cols; j++)
    {
      printf("%d ", mat[i * cols + j]);
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

// x^n:需优化
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

// Tylor method to calculate exponential value
long exp_cal(int n, long x)
{
  float partial = 0;
  for (int i = 0; i <= n; i++)
  {
    partial += power(x, i) / fac(i);
  }
  return partial;
}

<<<<<<< HEAD
// this code is borrowed from https://ourcodeworld.com/articles/read/884/how-to-get-the-square-root-of-a-number-without-using-the-sqrt-function-in-c
void my_sqrt(float number)
{
  float temp, sqrt;
  sqrt = number / 2;
  temp = 0;
  while(sqrt != temp){
    temp = sqrt;
    sqrt = ( number/temp + temp) / 2;
}

void layer_normalization(size_t dim_i, size_t dim_j, elem_t* added_mat)
{
  long alpha, beta, epsilon;

  /***** SUPER-PERIMETERS *****/
  alpha = 1;
  beta = 0;
  epsilon = 0.05;

  elem_t mean_value[i];
  elem_t square_deviation[i];
  for (size_t i = 0; i < dim_i; i++)
  {
    size_t sum = 0;
    for (size_t j = 0; j < dim_j; j++)
    {
      sum += added_mat[dim_i*i + j];
    }
    mean_value[i] = sum / dim_j;
  }

  for (size_t i = 0; i < dim_i; i++)
  {
    for (size_t j = 0; j < dim_j; j++)
    {
      square_deviation[i] += (added_mat[dim_i*i + j] - mean_value[i]) * (added_mat[dim_i*i + j] - mean_value[i]);
    }
  }

  for (size_t i = 0; i < dim_i; i++)
  {
    for (size_t j = 0; j < dim_j; j++)
    {
      added_mat[dim_i*i + j] = alpha * (added_mat[dim_i*i + j] - mean_value[i]) / (my_sqrt(square_deviation[i] + epsilon)) + beta;
    }
  }
  
  return;
}

void add_normalize(size_t dim_i,size_t dim_j,elem_t* mat_a,elem_t* mat_b,elem_t* added_mat)
=======
double sinfunc(double x)
{
  printf("sin function\n");
  double sum = 0, term = 1;
  int n = 1, t = 1;
  while (term >= 1e-5)
  {
    term = power(x, 2 * n - 1) / fac(2 * n - 1);
    sum += t * term;
    t = -t;
    n++;
  }
  return sum;
}

double cosfunc(double x)
{
  printf("cos function\n");
  double sum = 0, term = 1;
  int n = 0, t = 1;
  while (term >= 1e-5)
  {
    term = power(x, 2 * n) / fac(2 * n);
    sum += t * term;
    t = -t;
    n++;
  }
  return sum;
}

// fastly calculate sqrt
float CarmSqrt(float x)
{
  printf("sqrt function\n");
  union
  {
    int intPart;
    float floatPart;
  } convertor;
  union
  {
    int intPart;
    float floatPart;
  } convertor2;
  convertor.floatPart = x;
  convertor2.floatPart = x;
  convertor.intPart = 0x1FBCF800 + (convertor.intPart >> 1);
  convertor2.intPart = 0x5f3759df - (convertor2.intPart >> 1);
  return 0.5f * (convertor.floatPart + (x * convertor2.floatPart));
}

// 对原来的字矩阵直接进行position embedding，不保留原数据
void positional_embedding(size_t length, size_t dimension, elem_t positional_mat[length][dimension])
{
  printf("positional_embedding\n");
  // 生成位置矩阵
  if (dimension % 2 == 0)
  {
    for (int pos = 0; pos < length; pos++)
    {
      for (int i = 0; i < dimension; i += 2)
      {
        positional_mat[pos][i] = sinfunc(pos / power(10000, 2 * i / dimension));
        positional_mat[pos][i + 1] = cosfunc(pos / power(10000, (2 * i + 1) / dimension));
      }
    }
  }
  else
  {
    for (int pos = 0; pos < length; pos++)
    {
      for (int i = 0; i < dimension - 1; i += 2)
      {
        positional_mat[pos][i] = sinfunc(pos / power(10000, 2 * i / dimension));
        positional_mat[pos][i + 1] = cosfunc(pos / power(10000, (2 * i + 1) / dimension));
      }
      positional_mat[pos][dimension - 1] = sinfunc(pos / power(10000, 2 * (dimension - 1) / dimension));
    }
  }
  return;
}

elem_t row_summary(int column, elem_t *matrixRow)
{
  printf("row sum\n");
  float sum = 0;
  for (int i = 0; i < column; i++)
  {
    sum += *(matrixRow + i);
  }
  return sum;
}

// generate the softmax result
void softmaxFunc(size_t row, size_t column, elem_t objectMat[row][column], elem_t softmaxResultMat[row][row])
{
  printf("softmax function\n");
  // change the value into exp weight
  for (int rowNum = 0; rowNum < row; rowNum++)
  {
    for (int columnNum = 0; columnNum < column; columnNum++)
    {
      objectMat[rowNum][columnNum] = exp_cal(10, objectMat[rowNum][columnNum] / CarmSqrt(weightDim));
    }
  }
  for (int rowNum = 0; rowNum < row; rowNum++)
  {
    elem_t row_sum = row_summary(column, (elem_t *)objectMat[rowNum]);
    for (int columnNum = 0; columnNum < column; columnNum++)
    {
      softmaxResultMat[rowNum][columnNum] = objectMat[rowNum][columnNum] / row_sum;
    }
  }
  return;
}
void add_normalize(size_t dim_i, size_t dim_j, elem_t *mat_a, elem_t *mat_b, elem_t *added_mat)
>>>>>>> 4f68475dcc415350a8aa644f5f3e19c99d4f3b74
{
  elem_t id_mat[dim_j][dim_j];
  for (size_t i = 0; i < dim_j; i++)
    for (size_t j = 0; j < dim_j; j++)
      id_mat[i][j] = (i == j);
  u_int64_t start, end;
  start = read_cycles();
  tiled_matmul_auto(dim_i, dim_j, dim_j,
<<<<<<< HEAD
            (elem_t*)mat_a, (elem_t*)id_mat, (elem_t*)mat_b, (elem_t*)added_mat,
            dim_j,dim_j, dim_j, dim_j,
            MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
            NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
            false, false,
            false, true,
            3,
            WS);
  // change added_mat in function normalize()
  layer_normalization(dim_i, dim_j, added_mat);
  end=read_cycles();
  printf("Time for add & normalization: %d\n",end-start);
=======
                    (elem_t *)mat_a, (elem_t *)id_mat, (elem_t *)mat_b, (elem_t *)added_mat,
                    dim_j, dim_j, dim_j, dim_j,
                    MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                    NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                    false, false,
                    false, true,
                    3,
                    WS);
  end = read_cycles();
  printf("Time for add & normalization: %d\n", end - start);
>>>>>>> 4f68475dcc415350a8aa644f5f3e19c99d4f3b74
}

#endif /* AUX_H */
