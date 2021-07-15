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
  printf("start test\n");
  static elem_t word_vector0[wordNum][wordDim] row_align(1) = {5};
  enum tiled_matmul_type_t acceleration_type;
  printf("\n\n***** These tests are carried out by WS MatMul Case *****");
  encoder(word_vector0, acceleration_type = WS);
  decoder(acceleration_type = WS);

  printf("\n\n***** These tests are carried out by OS MatMul Case *****");
  // encoder(word_vector0, acceleration_type=OS);
  // decoder(acceleration_type=OS);


  printf("\n\n***** These tests are carried out by CPU MatMul Case *****");
  encoder(word_vector0, acceleration_type = CPU);
  decoder(acceleration_type = CPU);

  /***** DO NOT CHANGE CONTENT BELOW *****/

  gemmini_fence(); // system func
  exit(0);
}
