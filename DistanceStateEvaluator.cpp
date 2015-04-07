//
//  DistanceStateEvaluator.cpp
//  HW2
//
//  Created by Mohammed Bellifa on 4/4/15.
//  Copyright (c) 2015 Mohammed Bellifa. All rights reserved.
//

#include "DistanceStateEvaluator.h"
#include <iostream>
DistanceStateEvaluator::DistanceStateEvaluator() {}
double DistanceStateEvaluator::evaluate(ChineseCheckersState &state, int forPlayer, bool repeat) {
    /*int goalPosition = 0; //Find the closest goal position for our player
    if (forPlayer == 1) {
        for (const auto i : {77u, 69u, 61u, 53u, 78u, 70u, 62u, 79u,  71u,  80u}) {
            if (state.board[i] == 0) {
                goalPosition = i;
                break;
            }
        }
    } else {
        for (const auto i : {27u, 19u, 11u, 3u, 18u, 10u,  2u, 9u, 1u, 0u}) {
            if (state.board[i] == 0) {
                goalPosition = i;
                break;
            }
        }
    }
    int goalRow = goalPosition / 9;
    int goalCol = goalPosition % 9;
    
    int furthestDist = 0;
    int closestDist = 200;
    for (int i = 0;i < 81;++i) {
        if (state.board[i] == forPlayer) {
            int thisRow = i / 9;
            int thisCol = i % 9;
            int dist = 162 - pow(abs(goalRow - thisRow),2) + pow(abs(goalCol - thisCol),2);
            furthestDist = std::max(furthestDist,dist);
            closestDist = std::min(closestDist,dist);
        }
    }
    if (repeat) { furthestDist = furthestDist - evaluate(state, 3 - forPlayer,false); }
    return furthestDist + closestDist;*/
    /*if (memoMap.find(state) != memoMap.end()) {
        std::cout << "Found " << state.dumpState() << std::endl;
        return memoMap[state];
    }*/
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
    //memoMap[state] = score;
    return score;
}

double DistanceStateEvaluator::getLowerBound() {
    return lowerBound;
}
double DistanceStateEvaluator::getUpperBound() {
    return upperBound;
}