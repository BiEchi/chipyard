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
#include "include/my_test_headers/decoder.h"
#include "include/my_test_headers/encoder.h"


//============================ignoring functions above ======================================

int main() {
#ifndef BAREMETAL
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
      perror("mlockall failed");
      exit(1);
    }
#endif

  gemmini_flush(0);

  static elem_t word_vector0[DIM_I][DIM_K] row_align(1) = {5};
  encoder(word_vector0);
  decoder();










  gemmini_fence();  // system func
  exit(0);
}

