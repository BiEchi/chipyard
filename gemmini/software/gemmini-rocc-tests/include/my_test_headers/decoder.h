#ifndef DECODER_H
#define DECODER_H

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
#include "include/gemmini.h"
#include "./auxiliary.h"

void decoder(enum tiled_matmul_type_t accel_type)
{
  printf("\n>>Simulation for Transformer Decoder<<\n\n");

  uint64_t cycle[50] = {0};
  int length = 0;
  static elem_t q_weights[n_head][wordDim][weightDim];
  static elem_t k_weights[n_head][wordDim][weightDim];
  static elem_t v_weights[n_head][wordDim][weightDim];
  static elem_t q_mat[n_head][wordNum][weightDim];
  static elem_t k_mat[n_head][wordNum][weightDim];
  static elem_t v_mat[n_head][wordNum][weightDim];
  elem_t last_output[wordNum][wordDim];
  elem_t final_encoder_output[wordNum][wordDim];
  u_int64_t start, end;
  // positional encoding
  static elem_t positions[wordNum][wordDim];
  static elem_t id_word[wordDim][wordDim];
  static elem_t positioned_word[wordNum][wordDim];
  for (size_t i = 0; i < wordDim; i++)
    for (size_t j = 0; j < wordDim; j++)
      id_word[i][j] = i == j;
  start = read_cycles();

  // positional_embedding(wordNum, wordDim, positions);
  tiled_matmul_auto(wordNum, wordDim, wordDim,
                    (elem_t *)last_output, (elem_t *)id_word, (elem_t *)positions, (elem_t *)positioned_word,
                    wordDim, wordDim, wordDim, wordDim,
                    MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                    NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                    false, false,
                    false, true,
                    3,
                    WS);
  end = read_cycles();
  cycle[length] = end - start;
  length++;
  printf("Time for positional encoding: %d\n", end - start);

  // get Q K V
  start = read_cycles();
  for (int count = 0; count < n_head; count++)
  {
    tiled_matmul_auto(wordNum, weightDim, wordDim,
                      (elem_t *)positioned_word, (elem_t *)q_weights[count], NULL, (elem_t *)q_mat[count],
                      wordDim, weightDim, weightDim, weightDim,
                      MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                      NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                      false, false,
                      false, false,
                      3,
                      WS);
    tiled_matmul_auto(wordNum, weightDim, wordDim,
                      (elem_t *)positioned_word, (elem_t *)k_weights[count], NULL, (elem_t *)k_mat[count],
                      wordDim, weightDim, weightDim, weightDim,
                      MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                      NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                      false, false,
                      false, false,
                      3,
                      WS);
    tiled_matmul_auto(wordNum, weightDim, wordDim,
                      (elem_t *)positioned_word, (elem_t *)v_weights[count], NULL, (elem_t *)v_mat[count],
                      wordDim, weightDim, weightDim, weightDim,
                      MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                      NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                      false, false,
                      false, false,
                      3,
                      WS);
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
                      (elem_t *)q_mat[count], (elem_t *)k_mat[count], NULL, (elem_t *)temp_qk[count],
                      wordDim, wordNum, wordNum, wordNum,
                      MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                      NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                      false, true,
                      false, false,
                      3,
                      WS);
  }
  end = read_cycles();
  cycle[length] = end - start;
  length++;
  printf("Time for Q*K^T: %d\n", end - start);

  // masking
  static elem_t masked_qk[n_head][wordNum][wordNum];
  static elem_t mask[wordNum][wordNum];
  static elem_t mask_id[wordNum][wordNum];
  for (size_t i = 0; i < wordNum; i++)
    for (size_t j = 0; j < wordNum; j++)
      mask_id[i][j] = i == j;
  for (int i = 0; i < wordNum; i++)
  {
    for (int j = 0; j < wordNum; j++)
    {
      if (j > i)
      {
        mask[i][j] = elem_t_min;
      }
      else
      {
        mask[i][j] = 0;
      }
    }
  }

  start = read_cycles();
  for (int count = 0; count < n_head; count++)
  {
    tiled_matmul_auto(wordNum, wordNum, wordNum,
                      (elem_t *)temp_qk[count], (elem_t *)mask_id, (elem_t *)mask, (elem_t *)masked_qk[count],
                      wordNum, wordNum, wordNum, wordNum,
                      MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                      NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                      false, true,
                      false, false,
                      3,
                      WS);
  }
  end = read_cycles();
  cycle[length] = end - start;
  length++;
  printf("Time for masking: %d\n", end - start);

  //softmax(Q*K^T)
  static elem_t softmax_qk[n_head][wordNum][wordNum];
  start = read_cycles();
  for (int count = 0; count < n_head; count++)
  {
    softmaxFunc(wordDim, wordDim, masked_qk[count], softmax_qk[count]);
  }
  end = read_cycles();
  cycle[length] = end - start;
  length++;
  printf("Time for softmax(Q*K^T): %d\n", end - start);

  //softmax(Q*K^T) * V
  static elem_t z_matrix[n_head][wordNum][weightDim];
  start = read_cycles();
  for (int count = 0; count < n_head; count++)
  {
    tiled_matmul_auto(wordNum, weightDim, wordNum,
                      (elem_t *)softmax_qk[count], (elem_t *)v_mat[count], NULL, (elem_t *)z_matrix[count],
                      wordNum, weightDim, weightDim, weightDim,
                      MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                      NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                      false, false,
                      false, false,
                      3,
                      WS);
  }
  end = read_cycles();
  cycle[length] = end - start;
  length++;
  printf("Time for softmax(Q*K^T)*V: %d\n", end - start);

  //concat z_vectors
  static elem_t multihead_weight[n_head * weightDim][wordDim];
  static elem_t concat_z[wordNum][n_head * weightDim];
  start = read_cycles();
  for (int i = 0; i < n_head; i++)
  {
    for (int j = 0; j < i * weightDim; j++)
    {
      for (int k = 0; k < wordNum; k++)
      {
        concat_z[k][j] = z_matrix[i][k][j];
      }
    }
  }
  end = read_cycles();
  cycle[length] = end - start;
  length++;
  printf("Time for concatination: %d\n", end - start);

  //multihead and add&norm
  static elem_t normalized_z_mat[wordNum][wordDim];
  revised_add_normalize(wordNum,wordDim,n_head * weightDim,(elem_t *)concat_z,(elem_t *)multihead_weight,(elem_t *)normalized_z_mat,(elem_t *)positioned_word);

  //encoder-decoder attention
  static elem_t q_weights2[n_head][wordDim][weightDim];
  static elem_t k_weights2[n_head][wordDim][weightDim];
  static elem_t v_weights2[n_head][wordDim][weightDim];
  static elem_t q_mat2[n_head][wordNum][weightDim];
  static elem_t k_mat2[n_head][wordNum][weightDim];
  static elem_t v_mat2[n_head][wordNum][weightDim];

  // get encoder-decoder Q K V
  start = read_cycles();
  for (int count = 0; count < n_head; count++)
  {
    tiled_matmul_auto(wordNum, weightDim, wordDim,
                      (elem_t *)normalized_z_mat, (elem_t *)q_weights2[count], NULL, (elem_t *)q_mat2[count],
                      wordDim, weightDim, weightDim, weightDim,
                      MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                      NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                      false, false,
                      false, false,
                      3,
                      WS);
    tiled_matmul_auto(wordNum, weightDim, wordDim,
                      (elem_t *)final_encoder_output, (elem_t *)k_weights2[count], NULL, (elem_t *)k_mat2[count],
                      wordDim, weightDim, weightDim, weightDim,
                      MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                      NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                      false, false,
                      false, false,
                      3,
                      WS);
    tiled_matmul_auto(wordNum, weightDim, wordDim,
                      (elem_t *)final_encoder_output, (elem_t *)v_weights2[count], NULL, (elem_t *)v_mat2[count],
                      wordDim, weightDim, weightDim, weightDim,
                      MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                      NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                      false, false,
                      false, false,
                      3,
                      WS);
  }
  end = read_cycles();

  // Q*K^T
  static elem_t temp_qk2[n_head][wordNum][wordNum];
  start = read_cycles();
  for (int count = 0; count < n_head; count++)
  {
    tiled_matmul_auto(wordNum, wordNum, wordDim,
                      (elem_t *)q_mat2[count], (elem_t *)k_mat2[count], NULL, (elem_t *)temp_qk2[count],
                      wordDim, wordNum, wordNum, wordNum,
                      MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                      NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                      false, true,
                      false, false,
                      3,
                      WS);
  }
  end = read_cycles();
  cycle[length] = end - start;
  length++;
  printf("Time for Q*K^T: %d\n", end - start);

  //softmax(Q*K^T)
  static elem_t softmax_qk2[n_head][wordNum][wordNum];
  start = read_cycles();
  for (int count = 0; count < n_head; count++)
  {
    softmaxFunc(wordDim, wordDim, temp_qk2[count], softmax_qk2[count]);
  }

  end = read_cycles();
  cycle[length] = end - start;
  length++;
  printf("Time for softmax(Q*K^T): %d\n", end - start);

  //softmax(Q*K^T) * V
  static elem_t z_matrix2[n_head][wordNum][weightDim];
  start = read_cycles();
  for (int count = 0; count < n_head; count++)
  {
    tiled_matmul_auto(wordNum, weightDim, wordNum,
                      (elem_t *)softmax_qk2[count], (elem_t *)v_mat2[count], NULL, (elem_t *)z_matrix2[count],
                      wordNum, weightDim, weightDim, weightDim,
                      MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                      NO_ACTIVATION, ACC_SCALE_IDENTITY, 0, false,
                      false, false,
                      false, false,
                      3,
                      WS);
  }
  end = read_cycles();
  cycle[length] = end - start;
  length++;
  printf("Time for softmax(Q*K^T)*V: %d\n", end - start);


  //concat z_vectors
  static elem_t multihead_weight2[n_head * weightDim][wordDim];
  static elem_t concat_z2[wordNum][n_head * weightDim];
  start = read_cycles();
  for (int i = 0; i < n_head; i++)
  {
    for (int j = 0; j < i * weightDim; j++)
    {
      for (int k = 0; k < wordNum; k++)
      {
        concat_z2[k][j] = z_matrix2[i][k][j];
      }
    }
  }
  end = read_cycles();
  cycle[length] = end - start;
  length++;
  printf("Time for concatination: %d\n", end - start);

  // multihead and add&&norm
  static elem_t normalized_z_mat2[wordNum][wordDim];
  revised_add_normalize(wordNum,wordDim,n_head * weightDim,(elem_t *)concat_z2,(elem_t *)multihead_weight2,(elem_t *)normalized_z_mat2,(elem_t *)normalized_z_mat);
 
  // FC+add&&norm
  static elem_t fc_weight1[wordDim][wordDim];
  static elem_t linear_input[wordNum][wordDim];
  start = read_cycles();
  revised_add_normalize(wordNum,wordDim,wordDim,(elem_t *)normalized_z_mat2,(elem_t *)fc_weight1,(elem_t *)linear_input,(elem_t *)normalized_z_mat2);

  //linear layer
  static elem_t linear_weight[wordDim][n_words];
  static elem_t linear_output[wordDim][n_words];
  tiled_matmul_nn_auto(wordNum, n_words, wordDim,
                       linear_input, linear_weight, NULL, linear_output,
                       RELU, 0, 0, false,
                       WS, false, "fc_layer1");

  // softmax
  static elem_t softmax_final[wordDim][n_words];
  start = read_cycles();
  softmaxFunc(wordDim, wordDim, linear_output, softmax_final);
  end = read_cycles();
  cycle[length] = end - start;
  length++;

  return;
}

#endif /* DECODER_H */
