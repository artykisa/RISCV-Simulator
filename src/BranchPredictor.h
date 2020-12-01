/*
 * The branch perdiction module with the following strategies
 *   Always Taken
 *   Always Not Taken
 *   Backward Taken, Forward Not Taken
 *   Branch Prediction Buffer with 2bit history information
 * 
 * Created by He, Hao on 2019-3-25
 */

#ifndef BRANCH_PREDICTOR_H
#define BRANCH_PREDICTOR_H

#include <cstdint>
#include <string>

const int PRED_BUF_SIZE = 4096;

class BranchPredictor {
public:
  enum Strategy {
    AT, // Always Taken
    NT, // Always Not Taken
    BTFNT, // Backward Taken, Forward Not Taken
    BPB, // Branch Prediction Buffer with 2bit history information
    MY, // My Branch Predictor
  } strategy;

  BranchPredictor();
  ~BranchPredictor();

  bool predict(uint32_t pc, uint32_t insttype, int64_t op1, int64_t op2,
               int64_t offset);

  // For Branch Prediction Buffer 
  void update(uint32_t pc, bool branch);

  std::string strategyName();
  
private:
  enum PredictorState {
    STRONG_TAKEN = 0, WEAK_TAKEN = 1,
    STRONG_NOT_TAKEN = 3, WEAK_NOT_TAKEN = 2,
  } predbuf[PRED_BUF_SIZE]; // initial state: WEAK_TAKEN
  enum MyPredictorState{
    TAKEN4 = 0, TAKEN3 = 1,TAKEN2 = 2, TAKEN1= 3,
    NOT_TAKEN1 = 4, NOT_TAKEN2 = 5, NOT_TAKEN3 = 6, NOT_TAKEN4 = 7,
  };
  MyPredictorState myState = MyPredictorState::TAKEN2;
  bool buf_taken = false;
  bool buf_not_taken = false;
  bool buf_previous = true;
  bool buf_previous2 = true;
};

#endif
