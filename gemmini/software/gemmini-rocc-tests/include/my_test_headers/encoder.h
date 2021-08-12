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
  printf("\n\n>>Simulation for Transformer Encoder<<\n");

  uint64_t cycle[50] = {0};
  int length = 0;
  uint64_t start, end;
  printf("\n");
  static elem_t word_vector[wordNum][wordDim];

  //my multihead
  static elem_t q_mats[n_head][wordDim][weightDim];
  static elem_t k_mats[n_head][wordDim][weightDim];
  static elem_t v_mats[n_head][wordDim][weightDim];
  static elem_t z_qs[n_head][wordNum][weightDim];
  static elem_t z_ks[n_head][wordNum][weightDim];
  static elem_t z_vs[n_head][wordNum][weightDim];

  /*====================Transformer Encoder======================*/
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
  // loading_Positional_embedding(wordNum, wordDim, positions);
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
  cycle[length] = end - start;
  length++;
  printf("Time for positional encoding: %d\n", end - start);

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
  cycle[length] = end - start;
  length++;
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
  cycle[length] = end - start;
  length++;
  printf("Time for Q*K^T: %d\n", end - start);

  //softmax(Q*K^T)
  static elem_t softmaxResultMat[n_head][wordNum][wordNum];
  start = read_cycles();
  for (int count = 0; count < n_head; count++)
  {
    softmaxFunc(wordDim, wordDim, temp_qk[count], softmaxResultMat[count],accle_type);
  }
  end = read_cycles();
  cycle[length] = end - start;
  length++;
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
  cycle[length] = end - start;
  length++;
  printf("Time for softmax(Q*K^T)*V: %d\n", end - start);

  //concat z_vectors and multiple weight(multihead attention)
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
  cycle[length] = end - start;
  printf("Time for concatination: %d\n", cycle[length]);
  length++;

  // multihead + add&norm
  static elem_t multihead_weight[n_head * weightDim][wordDim];
  static elem_t normalized_z_mat[wordNum][wordDim];
  start = read_cycles();
  revised_add_normalize(wordNum,wordDim,n_head * weightDim,(elem_t *)concat_z,(elem_t *)multihead_weight,(elem_t *)normalized_z_mat,(elem_t *)word_vector,accle_type);
  end = read_cycles();
  cycle[length] = end - start;
  printf("Time for add & normalization: %d\n",cycle[length]);
  length++;

  // FC + add & norm
  static elem_t fc_weight1[wordDim][wordDim];
  static elem_t fc_result1[wordNum][wordDim];
  static elem_t final_encoder_output[wordDim][wordDim];
  start = read_cycles();
  revised_add_normalize(wordNum,wordDim,wordDim,(elem_t*)normalized_z_mat,(elem_t*)fc_weight1,(elem_t *)final_encoder_output,(elem_t *)normalized_z_mat,accle_type);
  end = read_cycles();
  cycle[length] = end - start;
  printf("Time for add & normalization: %d\n",cycle[length]);
  length++;
  total_time((uint64_t*)cycle,length);
  return;
}
