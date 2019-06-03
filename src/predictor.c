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
int pc_g;

// local history
int* lhistory_table;
int* l_2bits;
int l_size;
int l_mask;
int l_table_size;
// tournament 2-bit selector
int tour_select;
int pc_l

void
free_mem() {
  free(gshare_2bits);
  if (bpType == TOURNAMENT) {
    free(lhistory_table);
    free(l_2bits)
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
  if (bpType == GSHARE) {
    // init global
    ghistory = 0;
    gshare_size = (int) pow(2, ghistoryBits);
    gshare_2bits = (int* ) malloc(gshare_size * sizeof(int));
    gshare_mask = 0;
    for (int i = 0; i < ghistoryBits; i ++) {
      gshare_mask <<= 1;
      gshare_mask += 1;
    }
    for (int i = 0; i < gshare_size; i ++) gshare_2bits[i] = WN;
  }
  elif (bpType == TOURNAMENT) {
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

    // init local
    lhistoryBits, &pcIndexBits
    l_table_size = (int *) pow(2, pcIndexBits);
    l_size = (int) pow(2, lhistoryBits);
    lhistory_table = (int *) calloc(l_table_size, sizeof(int));
    l_2bits = (int* ) malloc(l_size * sizeof(int));
    l_mask = 0;
    for (int i = 0; i < pcIndexBits; i ++) {
      l_mask <<= 1;
      l_mask += 1;
    }
    for (int i = 0; i < l_size; i ++) l_2bits[i] = WN;

    // init selector
    tour_select = 0;
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
      pc &= gshare_mask;
      ghistory &= gshare_mask;
      pc = pc ^ ghistory;
      // printf("index: %d", pc);
      return gshare_2bits[pc] > WN;
    case TOURNAMENT:
      pc_g = pc & g_mask;
      pc_l = pc & lhistory_table l_mask;

      // select
      if tour_select > 1 // global if 11, 10
        return g_2bits[pc_g] > WN;
      else
        return g_2bits[pc_g] > WN;
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
      pc &= gshare_mask;
      ghistory &= gshare_mask;
      pc = pc ^ ghistory;
      if (outcome == TAKEN) {
        // printf("prev: %d", gshare_2bits[pc]);
        gshare_2bits[pc] = fmin(3, gshare_2bits[pc] + 1);
        // printf("after: %d", gshare_2bits[pc]);
      }
      else {
        // printf("index: %d ", pc);
        // printf("prev: %d ", gshare_2bits[pc]);
        gshare_2bits[pc] = fmax(0, gshare_2bits[pc] - 1);
        // printf("after: %d \n", gshare_2bits[pc]);
      }
      ghistory <<= 1;
      ghistory += outcome;
    default:
      break;
  }
}
