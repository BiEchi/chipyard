#include<stdio.h>

#define DIM_I 64
#define DIM_J 32
#define DIM_K 128
#define n_head 4

enum ACCEL_TYPE {CPU, WS, OS} acceleration_type;