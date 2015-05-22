//
//  TDLambdaStateEvaluator.h
//  ChineseCheckersAI
//
//  Created by Mohammed Bellifa on 4/29/15.
//  Copyright (c) 2015 Mohammed Bellifa. All rights reserved.
//

#ifndef __ChineseCheckersAI__TDLambdaStateEvaluator__
#define __ChineseCheckersAI__TDLambdaStateEvaluator__

#include <stdio.h>
#include "LearningStateEvaluator.h"
#include "LinearRegression.h"

class TDLambdaStateEvaluator : public LearningStateEvaluator {
public:
    TDLambdaStateEvaluator();
    double evaluate(ChineseCheckersState&,int,bool repeat = true);
    double getLowerBound();
    double getUpperBound();
    void train(int); //Train the evaluator with
    void getFeatures(const ChineseCheckersState &s, std::vector<double> &f, int who);
    void setWeights(std::vector<double>); //Import saved weights
    void trainFromTrace(const std::vector<Move> &trace); //Train weights from a trace
    Move getNextAction(ChineseCheckersState &s, double &bestVal); // Returns the best move (1-ply) for the purposes of training
private:
    int numInputs;
    LinearRegression r;
    double epsilon, lambda;
    std::vector<double> features;
};


#endif /* defined(__ChineseCheckersAI__TDLambdaStateEvaluator__) */
