#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif

#include "include/gemmini.h"
#include "include/gemmini_nn.h"
#include "include/my_test_headers/auxiliary.h"
#include "parameters4.h"

int main (int argc, char * argv[]) {
#ifndef BAREMETAL
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
      perror("mlockall failed");
      exit(1);
    }
#endif

    gemmini_flush(0);

    enum tiled_matmul_type_t tiled_matmul_type;
    if (argc < 2) {
        tiled_matmul_type = WS;
    } else if (strcmp(argv[1], "cpu") == 0) {
        tiled_matmul_type = CPU;
    } else if (strcmp(argv[1], "os") == 0) {
        tiled_matmul_type = OS;
    } else if (strcmp(argv[1], "ws") == 0) {
        tiled_matmul_type = WS;
    } else if (strcmp(argv[1], "-h") == 0) {
        printf("usage: %s [-h] matmul_option [check]\n  matmul_option may be 'os', 'ws', or cpu'\n", argv[0]);
        exit(0);
    } else {
        printf("Unknown command-line argument\n");
        printf("usage: %s [-h] matmul_option [check]\n  matmul_option may be 'os', 'ws', or cpu'\n", argv[0]);
        exit(1);
    }

    bool check;
    if (argc < 3) {
        check = false;
    } else if (strcmp(argv[2], "check") == 0) {
        check = true;
    } else {
        printf("Unknown command-line argument\n");
        printf("usage: %s [-h] matmul_option [check]\n  matmul_option may be 'os', 'ws', or cpu'\n", argv[0]);
        exit(1);
    }

    // uint64_t cycles[2]={0};
    uint64_t start,end;


    
    // FILE* fp = fopen("software/gemmini-rocc-tests/include/my_test_headers/embedding_position.txt","r");
    // if(fp == NULL){
    //     printf("failed\n");
    //     return 0;
    // }
    // /* matmul number: 0 */
    // start = read_cycles();
    // loading_Positional_embedding(DIM,DIM,input_mat,fp);

    // tiled_matmul_nn_auto(64, 4608, 3072,
    //     input_mat, weights0, NULL, inter_results0,
    //     RELU, 0, 0, false,
    //     tiled_matmul_type, check, "layer_0");

    end = read_cycles();
    uint64_t cycles= end-start;

    /* matmul number: 1 */
    // start = read_cycles();

    // tiled_matmul_nn_auto(64, 3072, 4608,
    //     inter_results0, weights1, NULL, inter_results1,
    //     RELU, 0, 0, false,
    //     tiled_matmul_type, check, "layer_1");

    // end = read_cycles();
    // cycles[1] = end-start;

    // uint64_t overall_cycles = 0;
    // for(int cyc = 0; cyc < 2 ; cyc++){
    //     overall_cycles += cycles[cyc];
    //     printf("Cycles taken in layer %d: %llu\n", cyc,cycles[cyc]);
    // }
    printf("reading overall cycles taken: %llu\n",cycles);
    printMatrix(input_mat);

    return 0;
}

