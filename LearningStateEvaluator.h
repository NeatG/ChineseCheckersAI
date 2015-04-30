//
//  LearningStateEvaluator.h
//  ChineseCheckersAI
//
//  Created by Mohammed Bellifa on 4/29/15.
//  Copyright (c) 2015 Mohammed Bellifa. All rights reserved.
//

#ifndef ChineseCheckersAI_LearningStateEvaluator_h
#define ChineseCheckersAI_LearningStateEvaluator_h
#include "StateEvaluator.h"

class LearningStateEvaluator : public StateEvaluator {
public:
    virtual double evaluate(ChineseCheckersState&,int,bool repeat = true) = 0;
    virtual double getLowerBound() = 0;
    virtual double getUpperBound() = 0;
    virtual void train(int) = 0; //Train the evaluator with
    virtual void getFeatures(const ChineseCheckersState &s, std::vector<double> &f, int who) = 0;
    virtual void setWeights(std::vector<double>) = 0; //Import saved weights
    
};


#endif
