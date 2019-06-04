//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include <math.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "NAME";
const char *studentID   = "PID";
const char *email       = "EMAIL";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//

// global history
int ghistory;
int* g_2bits;
int g_size;
int g_mask;
int g_index; // index in g_2bits table
int g_predict;

// local history
int* lhistory_table;
int* l_2bits;
int l_size;
int l_mask;
int lhistory_mask;
int l_table_size;
int pc_l;
int l_index; // index in l_2bits table
int l_predict;

// tournament 2-bit selector
int* choice_predictor;

void
free_mem() {
  free(g_2bits);
  if (bpType == TOURNAMENT) {
    free(lhistory_table);
    free(l_2bits);
    free(choice_predictor);
  }
}

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
  switch (bpType) {
    case GSHARE:
      // init global
      ghistory = 0;
      g_size = (int) pow(2, ghistoryBits);
      g_2bits = (int* ) malloc(g_size * sizeof(int));
      g_mask = 0;
      for (int i = 0; i < ghistoryBits; i ++) {
        g_mask <<= 1;
        g_mask += 1;
      }
      for (int i = 0; i < g_size; i ++) g_2bits[i] = WN;
      break;

    case TOURNAMENT:
      // init global
      ghistory = 0;
      g_size = (int) pow(2, ghistoryBits);
      g_2bits = (int* ) malloc(g_size * sizeof(int));
      g_mask = 0;
      for (int i = 0; i < ghistoryBits; i ++) {
        g_mask <<= 1;
        g_mask += 1;
      }
      for (int i = 0; i < g_size; i ++) g_2bits[i] = WN;
      printf("global init complete. \n");

      // init local
      l_table_size = (int) pow(2, pcIndexBits);
      l_size = (int) pow(2, lhistoryBits);
      lhistory_table = (int *) calloc(l_table_size, sizeof(int));
      l_2bits = (int* ) malloc(l_size * sizeof(int));
      l_mask = 0;
      lhistory_mask = 0;
      for (int i = 0; i < pcIndexBits; i ++) {
        l_mask <<= 1;
        l_mask += 1;
      }
      for (int i = 0; i < lhistoryBits; i ++) {
        lhistory_mask <<= 1;
        lhistory_mask += 1;
      }
      for (int i = 0; i < l_size; i ++) l_2bits[i] = WN;
      printf("local init complete. \n");

      // init choice predictor
      choice_predictor = (int *) malloc(g_size * sizeof(int));
      for (int i = 0; i < g_size; i ++) choice_predictor[i] = WT;
      printf("all init complete. \n");

      break;

    default:
      break;
  }
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;

    case GSHARE:
      pc &= g_mask;
      ghistory &= g_mask;
      pc = pc ^ ghistory;
      // printf("index: %d", pc);
      return g_2bits[pc] > WN;

    case TOURNAMENT:
      g_index = ghistory & g_mask;
      pc_l = pc & l_mask;
      l_index = lhistory_table[pc_l] & lhistory_mask;

      // get predictions
      g_predict = g_2bits[g_index] > WN;
      l_predict = l_2bits[l_index] > WN;
      // printf("g_index: %d, l_index: %d \n", g_predict, l_predict);

      // select
      if (choice_predictor[g_index] > WN) // global if 11, 10
        return g_predict;
      else               // local if 01, 00
        return l_predict;

    case CUSTOM:
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //
  switch (bpType) {
    case GSHARE:
      pc &= g_mask;
      ghistory &= g_mask;
      pc = pc ^ ghistory;
      if (outcome == TAKEN) {
        // printf("prev: %d", gshare_2bits[pc]);
        g_2bits[pc] = fmin(3, g_2bits[pc] + 1);
        // printf("after: %d", gshare_2bits[pc]);
      }
      else {
        // printf("index: %d ", pc);
        // printf("prev: %d ", gshare_2bits[pc]);
        g_2bits[pc] = fmax(0, g_2bits[pc] - 1);
        // printf("after: %d \n", gshare_2bits[pc]);
      }
      ghistory <<= 1;
      ghistory += outcome;
      break;

    case TOURNAMENT:
      g_index = ghistory & g_mask;
      pc_l = pc & l_mask;
      l_index = lhistory_table[pc_l] & lhistory_mask;

      // update global
      if (outcome == TAKEN) {
        g_2bits[g_index] = fmin(3, g_2bits[g_index] + 1);
      }
      else {
        g_2bits[g_index] = fmax(0, g_2bits[g_index] - 1);
      }
      ghistory <<= 1;
      ghistory += outcome;

      // update local
      if (outcome == TAKEN) {
        l_2bits[l_index] = fmin(3, l_2bits[l_index] + 1);
      }
      else {
        l_2bits[l_index] = fmax(0, l_2bits[l_index] - 1);
      }
      lhistory_table[pc_l] <<= 1;
      lhistory_table[pc_l] += outcome;

      // update choice predictor
      // counter change = I(?global correct) - I(?local correct)
      choice_predictor[g_index] = choice_predictor[g_index] + (g_predict == outcome) - (l_predict == outcome);
      // printf("inc_dec = %d", (g_predict == outcome) - (l_predict == outcome));
      choice_predictor[g_index] = fmin(3, choice_predictor[g_index]);
      choice_predictor[g_index] = fmax(0, choice_predictor[g_index]);

      break;

    default:
      break;
  }
}
