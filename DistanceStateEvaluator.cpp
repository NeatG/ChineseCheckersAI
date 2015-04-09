//
//  DistanceStateEvaluator.cpp
//  HW2
//
//  Created by Mohammed Bellifa on 4/4/15.
//  Copyright (c) 2015 Mohammed Bellifa. All rights reserved.
//

#include "DistanceStateEvaluator.h"
#include <iostream>
#include <algorithm>
DistanceStateEvaluator::DistanceStateEvaluator() {}
double DistanceStateEvaluator::evaluate(ChineseCheckersState &state, int forPlayer, bool repeat) {
    double score = 0;
    for (int i = 0;i < 81;++i) {
        int thisRow = i / 9;
        int thisCol = i % 9;
        if (state.board[i] == forPlayer) {
            if (forPlayer == 1) {
                score += thisRow + thisCol;
            } else { //Player 2
                score += (8 - thisRow) + (8 - thisCol);
            }
        } else if (state.board[i] == 3 - forPlayer) {
            if (forPlayer == 1) {
                score -= ((8 - thisRow) + (8 - thisCol));
            } else {
                score -= thisRow + thisCol;
            }
        }
    }
    return score;
}

double DistanceStateEvaluator::getLowerBound() {
    return lowerBound;
}
double DistanceStateEvaluator::getUpperBound() {
    return upperBound;
}