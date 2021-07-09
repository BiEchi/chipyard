#include <stdio.h>
#include "include/gemmini.h"

#define LEN(arr) ((int) (sizeof (arr) / sizeof (arr[0])))

// batch size: 64
// before zeropad: 784x800x10
// after zeropad: 832x832x64
static elem_t word_mat[64][832] row_align(1)= {0};

static elem_t queryWeight[832][416] row_align(1)= {0};
static elem_t keyWeight[832][416] row_align(1)= {0};
static elem_t infoWeight[832][416] row_align(1)= {0};
static elem_t queryResult[64][416] row_align(1)= {0};
static elem_t keyResult[64][416] row_align(1)= {0};
static elem_t infoResult[64][416] row_align(1)= {0};

static elem_t hiddenLayerMatrix;
