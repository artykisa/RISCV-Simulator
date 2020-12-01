/*
 * Created by He, Hao on 2019-3-25
 */

#include "BranchPredictor.h"
#include "Debug.h"

BranchPredictor::BranchPredictor() {
  for (int i = 0; i < PRED_BUF_SIZE; ++i) {
    this->predbuf[i] = WEAK_TAKEN;
  }
}

BranchPredictor::~BranchPredictor() {}

bool BranchPredictor::predict(uint32_t pc, uint32_t insttype, int64_t op1,
                              int64_t op2, int64_t offset) {
  switch (this->strategy) {
  case NT:
    return false;
  case AT:
    return true;
  case BTFNT: {
    if (offset >= 0) {
      return false;
    } else {
      return true;
    }
  }
  break;
  case BPB: {
    PredictorState state = this->predbuf[pc % PRED_BUF_SIZE];
    if (state == STRONG_TAKEN || state == WEAK_TAKEN) {
      return true;
    } else if (state == STRONG_NOT_TAKEN || state == WEAK_NOT_TAKEN) {
      return false;
    } else {
      dbgprintf("Strange Prediction Buffer!\n");
    }   
  }
  break;
  case MY:{
    switch(this->myState){
      case MyPredictorState::TAKEN4: return true;
      case MyPredictorState::TAKEN3: return true;
      case MyPredictorState::TAKEN2: return buf_previous || buf_previous2;
      case MyPredictorState::TAKEN1: return buf_previous && buf_previous2;
      case MyPredictorState::NOT_TAKEN1: return buf_previous || buf_previous2;
      case MyPredictorState::NOT_TAKEN2: return buf_previous && buf_previous2;
      case MyPredictorState::NOT_TAKEN3: return false;
      case MyPredictorState::NOT_TAKEN4: return false;

    }
  }
  default:
    dbgprintf("Unknown Branch Perdiction Strategy!\n");
    break;
  }
  return false;
}

void BranchPredictor::update(uint32_t pc, bool branch) {
  switch(this->strategy){
    case MY:
    buf_previous2 = buf_previous;
    buf_previous = branch;
    if(branch){
      switch(this->myState){
        case MyPredictorState::TAKEN4: myState=MyPredictorState::TAKEN4;break;
        case MyPredictorState::TAKEN3: 
          if(buf_taken){
            myState=MyPredictorState::TAKEN4;break;
          }
          else{
            buf_taken=true; break;
          }
        case MyPredictorState::TAKEN2: myState=MyPredictorState::TAKEN3;break;
        case MyPredictorState::TAKEN1: myState=MyPredictorState::TAKEN2;break;
        case MyPredictorState::NOT_TAKEN1: myState=MyPredictorState::TAKEN1;break;
        case MyPredictorState::NOT_TAKEN2: myState=MyPredictorState::NOT_TAKEN1;break;
        case MyPredictorState::NOT_TAKEN3: myState=MyPredictorState::NOT_TAKEN2;break;
        case MyPredictorState::NOT_TAKEN4: myState=MyPredictorState::NOT_TAKEN3;break;
      }
      buf_not_taken=false;
    }
    else{
      switch(this->myState){
        case MyPredictorState::TAKEN4: myState=MyPredictorState::TAKEN3;break;
        case MyPredictorState::TAKEN3: myState=MyPredictorState::TAKEN2;break;
        case MyPredictorState::TAKEN2: myState=MyPredictorState::TAKEN1;break;
        case MyPredictorState::TAKEN1: myState=MyPredictorState::NOT_TAKEN1;break;
        case MyPredictorState::NOT_TAKEN1: myState=MyPredictorState::NOT_TAKEN2;break;
        case MyPredictorState::NOT_TAKEN2: myState=MyPredictorState::NOT_TAKEN3;break;
        case MyPredictorState::NOT_TAKEN3:
          if(buf_not_taken){
            myState=MyPredictorState::TAKEN4;break;
          }
          else{
            buf_not_taken=true; break;
          }
        case MyPredictorState::NOT_TAKEN4: myState=MyPredictorState::NOT_TAKEN4;break;
      }
      buf_taken = false;
    }
    
    default:
      int id = pc % PRED_BUF_SIZE;
      PredictorState state = this->predbuf[id];
    if (branch) {
      if (state == STRONG_NOT_TAKEN) {
        this->predbuf[id] = WEAK_NOT_TAKEN;
      } else if (state == WEAK_NOT_TAKEN) {
        this->predbuf[id] = WEAK_TAKEN;
      } else if (state == WEAK_TAKEN) {
        this->predbuf[id] = STRONG_TAKEN;
      } // do nothing if STRONG_TAKEN
    } else { // not branch
      if (state == STRONG_TAKEN) {
        this->predbuf[id] = WEAK_TAKEN;
      } else if (state == WEAK_TAKEN) {
        this->predbuf[id] = WEAK_NOT_TAKEN;
      } else if (state == WEAK_NOT_TAKEN) {
        this->predbuf[id] = STRONG_NOT_TAKEN;
      } // do noting if STRONG_NOT_TAKEN
    }
  }
  
}

std::string BranchPredictor::strategyName() {
  switch (this->strategy) {
  case NT:
    return "Always Not Taken";
  case AT:
    return "Always Taken";
  case BTFNT:
    return "Back Taken Forward Not Taken";
  case BPB:
    return "Branch Prediction Buffer";
  case MY:
    return "My Branch Predictor";
  default:
    dbgprintf("Unknown Branch Perdiction Strategy!\n");
    break;
  }
  return "error"; // should not go here
}