#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef BAREMETAL
#include <sys/mman.h>
#endif // system func

#include "include/gemmini_testutils.h"
#include "include/gemmini_nn.h"
#include "include/gemmini.h"
#include "./auxiliary.h"

void encoder(elem_t word_vector0[wordNum][wordDim], enum tiled_matmul_type_t accel_type)
{
  uint64_t start, end;
  printf("\n");

  //my multihead
  static elem_t q_mats[n_head][wordDim][weightDim];
  static elem_t k_mats[n_head][wordDim][weightDim];
  static elem_t v_mats[n_head][wordDim][weightDim];
  static elem_t z_qs[n_head][wordNum][weightDim];
  static elem_t z_ks[n_head][wordNum][weightDim];
  static elem_t z_vs[n_head][wordNum][weightDim];
  /*====================Transformer Encoder======================*/

  static elem_t word_vector[wordNum][wordDim];
  static elem_t positions[wordNum][wordDim];
  static elem_t id_word[wordDim][wordDim];
  
  for (size_t i = 0; i < wordDim; i++)
  {
    for (size_t j = 0; j < wordDim; j++)
    {
      id_word[i][j] = i == j;
    }
  }
  // positional embedding
  start = read_cycles();
  positional_embedding(wordNum, wordDim, positions);
  tiled_matmul_auto(wordNum, wordDim, wordDim,
                    (elem_t *)word_vector0, (elem_t *)id_word, (elem_t *)positions, (elem_t *)word_vector,
                    wordDim, wordDim, wordDim, wordDim,
                    MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                    NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                    false, false,
                    false, false,
                    3,
                    accel_type);
  end = read_cycles();
  printf("Time for positional encoding: %d\n", end - start);

  // self-attention
  // get Q K V
  start = read_cycles();
  for (int count = 0; count < n_head; count++)
  {
    tiled_matmul_auto(wordNum, weightDim, wordDim,
                      (elem_t *)word_vector, (elem_t *)q_mats[count], NULL, (elem_t *)z_qs[count],
                      wordDim, weightDim, weightDim, weightDim,
                      MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                      NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                      false, false,
                      false, false,
                      3,
                      accel_type);
    tiled_matmul_auto(wordNum, weightDim, wordDim,
                      (elem_t *)word_vector, (elem_t *)k_mats[count], NULL, (elem_t *)z_ks[count],
                      wordDim, weightDim, weightDim, weightDim,
                      MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                      NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                      false, false,
                      false, false,
                      3,
                      accel_type);
    tiled_matmul_auto(wordNum, weightDim, wordDim,
                      (elem_t *)word_vector, (elem_t *)v_mats[count], NULL, (elem_t *)z_vs[count],
                      wordDim, weightDim, weightDim, weightDim,
                      MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                      NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                      false, false,
                      false, false,
                      3,
                      accel_type);
  }
  end = read_cycles();
  printf("Time for get Q K V matrix: %d\n", end - start);

  // Q*K^T
  static elem_t temp_qk[n_head][wordNum][wordNum];
  start = read_cycles();
  for (int count = 0; count < n_head; count++)
  {
    tiled_matmul_auto(wordNum, wordNum, wordDim,
                      (elem_t *)z_qs[count], (elem_t *)z_ks[count], NULL, (elem_t *)temp_qk[count],
                      wordDim, wordNum, wordNum, wordNum,
                      MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                      NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                      false, true,
                      false, false,
                      3,
                      accel_type);
  }
  end = read_cycles();
  printf("Time for Q*K^T: %d\n", end - start);

  //softmax(Q*K^T),unfinished
  static elem_t softmaxResultMat[n_head][wordNum][wordNum];
  start = read_cycles();
  for (int count = 0; count < n_head; count++)
  {
    softmaxFunc(wordDim, wordDim, temp_qk[count], softmaxResultMat[count]);
  }
  end = read_cycles();
  printf("Time for softmax(Q*K^T): %d\n", end - start);

  //softmax(Q*K^T) * V
  static elem_t z_vector[n_head][wordNum][weightDim];
  start = read_cycles();
  for (int count = 0; count < n_head; count++)
  {
    tiled_matmul_auto(wordNum, weightDim, wordNum,
                      (elem_t *)softmaxResultMat[count], (elem_t *)z_vs[count], NULL, (elem_t *)z_vector[count],
                      wordNum, weightDim, weightDim, weightDim,
                      MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                      NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                      false, false,
                      false, false,
                      3,
                      accel_type);
  }
  end = read_cycles();
  printf("Time for softmax(Q*K^T)*V: %d\n", end - start);

  //concat z_vectors and multiple weight(multihead attention)
  static elem_t multihead_weight[n_head * weightDim][wordDim];
  static elem_t concat_z[wordNum][n_head * weightDim];
  start = read_cycles();
  for (int i = 0; i < n_head; i++)
  {
    for (int j = 0; j < i * weightDim; j++)
    {
      for (int k = 0; k < wordNum; k++)
      {
        concat_z[k][j] = z_vector[i][k][j];
      }
    }
  }
  end = read_cycles();
  printf("Time for concatination: %d\n", end - start);
  static elem_t final_z_mat[wordNum][wordDim];
  start = read_cycles();
  tiled_matmul_auto(wordNum, wordDim, n_head * weightDim,
                    (elem_t *)concat_z, (elem_t *)multihead_weight, NULL, (elem_t *)final_z_mat,
                    n_head * weightDim, wordDim, wordDim, wordDim,
                    MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                    NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                    false, false,
                    false, false,
                    3,
                    accel_type);
  end = read_cycles();
  printf("Time for multihead attention: %d\n", end - start);

  // add and normalize(add part)
  static elem_t added_z_mat[wordNum][wordDim];
  static elem_t id_mat_z[wordDim][wordDim];
  for (size_t i = 0; i < wordDim; i++)
    for (size_t j = 0; j < wordDim; j++)
      id_mat_z[i][j] = (i == j);
  start = read_cycles();
  tiled_matmul_auto(wordNum, wordDim, wordDim,
                    (elem_t *)final_z_mat, (elem_t *)id_mat_z, (elem_t *)z_vector, (elem_t *)added_z_mat,
                    wordDim, wordDim, wordDim, wordDim,
                    MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                    NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                    false, false,
                    false, false,
                    3,
                    accel_type);

  // add and normalize(normalization part), unfinished
  static elem_t normalized_z_mat[wordNum][wordDim];

  add_normalize(wordDim, wordDim, (elem_t *)final_z_mat, (elem_t *)word_vector, (elem_t *)normalized_z_mat);
  end = read_cycles();
  printf("Time for add&normalization: %d\n", end - start);

  // fully connected layer1
  static elem_t fc_weight1[wordDim][wordDim];
  static elem_t fc_result1[wordNum][wordDim];
  start = read_cycles();
  tiled_matmul_nn_auto(wordNum, wordDim, wordDim,
                       normalized_z_mat, fc_weight1, NULL, fc_result1,
                       RELU, 0, 0, false,
                       accel_type, false, "fc_layer1");
  end = read_cycles();
  printf("Time for fc_layer: %d\n", end - start);

  // add & normalization after FC(add part)
  static elem_t final_encoder_output[wordDim][wordDim];
  add_normalize(wordNum, wordDim, (elem_t *)fc_result1, (elem_t *)normalized_z_mat, (elem_t *)final_encoder_output);

  // linear layer

  tiled_matmul_nn_auto(wordDim, wordDim, wordDim,
                       normalized_z_mat, fc_weight1, NULL, fc_result1,
                       RELU, 0, 0, false,
                       WS, false, "fc_layer1");

  return;
}
