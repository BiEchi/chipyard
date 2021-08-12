// See LICENSE for license details.

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
#include "include/my_test_headers/decoder.h"
#include "include/my_test_headers/encoder.h"

int main()
{
  /***** DO NOT CHANGE CONTENT BELOW *****/
#ifndef BAREMETAL
  if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0)
  {
    perror("mlockall failed");
    exit(1);
  }
#endif

  gemmini_flush(0);
  /***** YOUR CODE STARTS HERE *****/

  // elem_t test_a[32][16];
  // elem_t test_b[16][16];
  // elem_t test_c[16][16];
  // elem_t test_d[16][16];
  // gemmini_config_ld(16 * sizeof(elem_t));
  // gemmini_config_st(16 * sizeof(elem_t));
  // gemmini_config_ex(OUTPUT_STATIONARY, 0, 0, ACC_SCALE_IDENTITY, 0);
  // size_t In_sp_addr = 16;
  // size_t Identity_sp_addr = 32;
  // size_t Out_sp_addr = 64;

  // // gemmini_extended_preload(BD, C, BD_cols, BD_rows, C_cols, C_rows)
  // // gemmini_extended_compute_preloaded(A, BD, A_cols, A_rows, BD_cols, BD_rows) \



  // gemmini_mvin(test_a, In_sp_addr);
  // gemmini_mvin(test_b, Identity_sp_addr);

  // // gemmini_extended_preload(Out_sp_addr);

  // u_int64_t teststart = read_cycles();
  // gemmini_extended_compute_preloaded(In_sp_addr, Identity_sp_addr, 1, 1, 1, 1);
  // u_int64_t testend = read_cycles();
  // gemmini_mvout(test_c, Out_sp_addr);

  // printf("Time: %d\n",testend-teststart);


  printf("start test\n");
  static elem_t word_vector0[wordNum][wordDim] row_align(1) = {5};
  enum tiled_matmul_type_t acceleration_type;
  printf("\n\n***** These tests are carried out by WS MatMul Case *****");
  encoder(word_vector0, acceleration_type = WS);
  decoder(acceleration_type = WS);

  printf("\n\n***** These tests are carried out by OS MatMul Case *****");
  encoder(word_vector0, acceleration_type=OS);
  decoder(acceleration_type=OS);


  // printf("\n\n***** These tests are carried out by CPU MatMul Case *****");
  // encoder(word_vector0, acceleration_type = CPU);
  // decoder(acceleration_type = CPU);

  /***** DO NOT CHANGE CONTENT BELOW *****/

  gemmini_fence(); // system func
  exit(0);
}
