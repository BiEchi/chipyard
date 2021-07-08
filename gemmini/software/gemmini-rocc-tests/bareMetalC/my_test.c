// See LICENSE for license details.

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

#define DIM_I 64
#define DIM_J 32
#define DIM_K 128
#define n_head 4

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

//============================ignoring functions above ======================================

int main()
{
#ifndef BAREMETAL
  if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0)
  {
    perror("mlockall failed");
    exit(1);
  }
#endif

  gemmini_flush(0);

  uint64_t start, end;
  printf("\n");

  //my multihead
  static elem_t word_vector0[DIM_I][DIM_K] row_align(1) = {5};
  static elem_t q_mats[n_head][DIM_K][DIM_J];
  static elem_t k_mats[n_head][DIM_K][DIM_J];
  static elem_t v_mats[n_head][DIM_K][DIM_J];
  static elem_t z_qs[n_head][DIM_I][DIM_J];
  static elem_t z_ks[n_head][DIM_I][DIM_J];
  static elem_t z_vs[n_head][DIM_I][DIM_J];

  /*====================Transformer Encoder======================*/

  // positional embedding
  static elem_t word_vector[DIM_I][DIM_K];
  static elem_t positions[DIM_I][DIM_K];
  static elem_t id_word[DIM_K][DIM_K];
  for (size_t i = 0; i < DIM_K; i++)
    for (size_t j = 0; j < DIM_K; j++)
      id_word[i][j] = i == j;
  start = read_cycles();
  tiled_matmul_auto(DIM_I, DIM_K, DIM_K,
                    (elem_t *)word_vector0, (elem_t *)id_word, (elem_t *)positions, (elem_t *)word_vector,
                    DIM_K, DIM_K, DIM_K, DIM_K,
                    MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                    NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                    false, false,
                    false, false,
                    3,
                    WS);
  end = read_cycles();
  printf("Time for positional encoding: %d\n", end - start);

  // get Q K V
  start = read_cycles();
  for (int count = 0; count < n_head; count++)
  {
    tiled_matmul_auto(DIM_I, DIM_J, DIM_K,
                      (elem_t *)word_vector, (elem_t *)q_mats[count], NULL, (elem_t *)z_qs[count],
                      DIM_K, DIM_J, DIM_J, DIM_J,
                      MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                      NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                      false, false,
                      false, false,
                      3,
                      WS);
    tiled_matmul_auto(DIM_I, DIM_J, DIM_K,
                      (elem_t *)word_vector, (elem_t *)k_mats[count], NULL, (elem_t *)z_ks[count],
                      DIM_K, DIM_J, DIM_J, DIM_J,
                      MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                      NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                      false, false,
                      false, false,
                      3,
                      WS);
    tiled_matmul_auto(DIM_I, DIM_J, DIM_K,
                      (elem_t *)word_vector, (elem_t *)v_mats[count], NULL, (elem_t *)z_vs[count],
                      DIM_K, DIM_J, DIM_J, DIM_J,
                      MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                      NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                      false, false,
                      false, false,
                      3,
                      WS);
  }
  end = read_cycles();
  printf("Time for get Q K V matrix: %d\n", end - start);

  // Q*K^T
  static elem_t temp_qk[n_head][DIM_I][DIM_I];
  start = read_cycles();
  for (int count = 0; count < n_head; count++)
  {
  tiled_matmul_auto(DIM_I, DIM_I, DIM_K,
            (elem_t*)z_qs[count], (elem_t*)z_ks[count], NULL, (elem_t*)temp_qk[count],
            DIM_K,DIM_I, DIM_I, DIM_I,
            MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
            NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
            false, true,
            false, false,
            3,
            WS);
  }
  end = read_cycles();
  printf("Time for Q*K^T: %d\n", end - start);

  //softmax(Q*K^T),unfinished
  static elem_t softmax_qk[n_head][DIM_I][DIM_I];
  start = read_cycles();

  end = read_cycles();
  printf("Time for softmax(Q*K^T): %d\n", end - start);

  //softmax(Q*K^T) * V
  static elem_t z_vector[n_head][DIM_I][DIM_J];
  start = read_cycles();
  for (int count = 0; count < n_head; count++)
  {
    tiled_matmul_auto(DIM_I, DIM_J, DIM_I,
                      (elem_t *)softmax_qk[count], (elem_t *)z_vs[count], NULL, (elem_t *)z_vector[count],
                      DIM_I, DIM_J, DIM_J, DIM_J,
                      MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                      NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                      false, false,
                      false, false,
                      3,
                      WS);
  }
  end = read_cycles();
  printf("Time for softmax(Q*K^T)*V: %d\n", end - start);

  //concat z_vectors and multiple weight(multihead attention)
  static elem_t multihead_weight[n_head * DIM_J][DIM_K];
  static elem_t concat_z[DIM_I][n_head * DIM_J];
  start = read_cycles();
  for (int i = 0; i < n_head; i++)
  {
    for (int j = 0; j < i * DIM_J; j++)
    {
      for (int k = 0; k < DIM_I; k++)
      {
        concat_z[k][j] = z_vector[i][k][j];
      }
    }
  }
  end = read_cycles();
  printf("Time for concatination: %d\n", end - start);
  static elem_t final_z_mat[DIM_I][DIM_K];
  start = read_cycles();
  tiled_matmul_auto(DIM_I, DIM_K, n_head * DIM_J, 
                    (elem_t *)concat_z, (elem_t *)multihead_weight, NULL, (elem_t *)final_z_mat,
                    n_head * DIM_J, DIM_K, DIM_K, DIM_K,
                    MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                    NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                    false, false,
                    false, false,
                    3,
                    WS);
  end = read_cycles();
  printf("Time for multihead attention: %d\n", end - start);

  // add and normalize(add part)
  static elem_t added_z_mat[DIM_I][DIM_K];
  static elem_t id_mat_z[DIM_K][DIM_K];
  for (size_t i = 0; i < DIM_K; i++)
    for (size_t j = 0; j < DIM_K; j++)
	id_mat_z[i][j] = (i == j);
  start = read_cycles();
  tiled_matmul_auto(DIM_I, DIM_K, DIM_K,
                    (elem_t *)final_z_mat, (elem_t *)id_mat_z, (elem_t *)z_vector, (elem_t *)added_z_mat,
                    DIM_K, DIM_K, DIM_K, DIM_K,
                    MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                    NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                    false, false,
                    false, false,
                    3,
                    WS);

  // add and normalize(normalization part), unfinished
  static elem_t normalized_z_mat[DIM_I][DIM_K];

  end = read_cycles();
  printf("Time for add&normalization: %d\n", end - start);

  // fully connected layer1
  static elem_t fc_weight1[DIM_K][DIM_K];
  static elem_t fc_result1[DIM_I][DIM_K];
  start = read_cycles();
  tiled_matmul_nn_auto(DIM_I, DIM_K, DIM_K,
                       normalized_z_mat, fc_weight1, NULL, fc_result1,
                       RELU, 0, 0, false,
                       WS, false, "fc_layer1");
  end = read_cycles();
  printf("Time for fc_layer: %d\n", end - start);

  // add & normalization after FC(add part)
  static elem_t encoder_out[DIM_I][DIM_K];
  start = read_cycles();
  tiled_matmul_auto(DIM_I, DIM_K, DIM_K,
                    (elem_t *)fc_result1, (elem_t *)id_mat_z, (elem_t *)normalized_z_mat, (elem_t *)encoder_out,
                    DIM_K, DIM_K, DIM_K, DIM_K,
                    MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                    NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                    false, false,
                    false, false,
                    3,
                    WS);
  end = read_cycles();
  printf("Time for add & normalization after Fully Connected Layer: %d\n",end-start);

  // add and normalize(normalization part), unfinished
  static elem_t final_encoder_output[DIM_I][DIM_K];


  gemmini_fence();  // system func
  exit(0);
}
