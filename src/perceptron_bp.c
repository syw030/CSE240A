//========================================================//
//  perceptron_bp.c                                       //
//  described in "Neural Methods for Dynamic              //
// Branch Prediction" Jimenez et al.                      //
//========================================================//

#include <stdio.h>
#include <math.h>

// hard code number of perceptrons: in power of 2
// configs use: page 384 2KB budget entry
#define NUM_PERCEPTRON 55
#define GLOBAL_BITS 31
#define LOCAL_BITS 5
#define LOCALH_SIZE 11 // calculated: lh size = 2864 > 2^11
#define INIT_CONST 0
#define THRESHOLD floor(1.93 * (GLOBAL_BITS + LOCAL_BITS) + 14)

// data structure
int8_t * weights_table;
int8_t * input;
int weight_len;
int output;
uint8_t last_predict;
int gh;
int lh;

void
init_perceptron() {
  weight_len = GLOBAL_BITS + LOCAL_BITS + 1;
  weights_table = (int8_t *) malloc(weight_len * NUM_PERCEPTRON * sizeof(int8_t));

  for (int i = 0; i < NUM_PERCEPTRON; i++) {
    for (int j = 0; j < weight_len; j ++) {
      weights_table[i * weight_len + j] = INIT_CONST;
    }
  }
  input = (int8_t *) malloc(weight_len * sizeof(int8_t));
  input[weight_len - 1] = 1;
}

uint8_t
perceptron_predicit(int idx, int globalh, int localh) {
  output = 0;

  /* TODO: save globalh, localh in +1, -1 form for training*/
  for (int i = 0;i < GLOBAL_BITS + LOCAL_BITS;i++) {
    if (i < GLOBAL_BITS) {
      input[i] = (int8_t) (globalh & 1) * 2 - 1;
      globalh >>= 1;
    }
    else {
      input[i] = (int8_t) (localh & 1) * 2 - 1;
      localh >>= 1;
    }
    output += (int) input[i] * weights_table[idx * weight_len + i]; // 0 -> -1
  }
  output += weights_table[(idx + 1) * weight_len - 1]; // bias
  last_predict = (uint8_t) output >= 0;
  // printf("output: %d \n", output);

  return last_predict;
}

void
train_perceptron(int idx, uint8_t outcome) {
  int8_t signed_outcome = ((int8_t) outcome * 2) - 1;
  if (abs(output) <= THRESHOLD || last_predict != outcome) {
    for (int i = 0;i < GLOBAL_BITS + LOCAL_BITS;i++) {
      if (input[i] == signed_outcome)
        weights_table[idx * weight_len + i] += 1; // unsigned int is self-modula in c++
      else
        weights_table[idx * weight_len + i] -= 1; // unsigned int is self-modula in c++
    }
    weights_table[(idx + 1) * weight_len - 1] += signed_outcome; // update bias weight
  }
}
