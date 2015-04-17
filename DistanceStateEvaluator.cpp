//
//  DistanceStateEvaluator.cpp
//  HW2
//
//  Created by Mohammed Bellifa on 4/4/15.
//  Copyright (c) 2015 Mohammed Bellifa. All rights reserved.
//

#include "DistanceStateEvaluator.h"
DistanceStateEvaluator::DistanceStateEvaluator() {}
double DistanceStateEvaluator::evaluate(ChineseCheckersState &state, int forPlayer, bool repeat) {
    double score = 0;
    double distScore = 0;
    for (int i = 0;i < 81;++i) {
        int thisRow = i / 9;
        int thisCol = i % 9;
        if (state.board[i] == forPlayer) {
            if (forPlayer == 1) {
                distScore += thisRow + thisCol;
            } else { //Player 2
                distScore += ((8 - thisRow) + (8 - thisCol));
            }
        } else if (state.board[i] == 3 - forPlayer) {
            if (forPlayer == 1) {
                distScore -= ((8 - thisRow) + (8 - thisCol));
            } else {
                distScore -= thisRow + thisCol;
            }
        }
    }
    double goalScore = 0;
    //Player 1 goal
    for (const auto i : {53u, 61u, 62u, 69u, 70u, 71u, 77u, 78u, 79u, 80u}) {
        if (state.board[i] == 1 && forPlayer == 1) {
            ++goalScore;
        } else if (state.board[i] == 2 && forPlayer == 2) {
            --goalScore;
        }
    }
    //Player 2 goal
    for (const auto i : {0u, 1u, 2u, 3u, 9u, 10u, 11u, 18u, 19u, 27u}) {
        if (state.board[i] == 2 && forPlayer == 2) {
            ++goalScore;
        } else if (state.board[i] == 1 && forPlayer == 1) {
            --goalScore;
        }
    }
    
    std::vector<Move> moves;
    state.getMoves(moves);
    double moveScore = 0;
    int curPlayer = state.getCurrentPlayer();
    for (auto mv : moves) {
        double toRow = mv.to / 9;
        double toCol = mv.to % 9;
        if (curPlayer == forPlayer) {
            if (forPlayer == 1) {
                moveScore += toRow + toCol;
            } else { //Player 2
                moveScore += (8 - toRow) + (8 - toCol);
            }
        } else if (curPlayer == 3 - forPlayer) {
            if (forPlayer == 1) {
                moveScore -= (8 - toRow) + (8 - toCol);
            } else {
                moveScore -= toRow + toCol;
            }
        }
    }
    //moveScore /= moves.size();
    
    curPlayer = 3 - curPlayer;
    state.getMoves(moves,curPlayer);
    for (auto mv : moves) {
        double toRow = mv.to / 9;
        double toCol = mv.to % 9;
        if (curPlayer == forPlayer) {
            if (forPlayer == 1) {
                moveScore += toRow + toCol;
            } else { //Player 2
                moveScore += (8 - toRow) + (8 - toCol);
            }
        } else if (curPlayer == 3 - forPlayer) {
            if (forPlayer == 1) {
                moveScore -= (8 - toRow) + (8 - toCol);
            } else {
                moveScore -= toRow + toCol;
            }
        }
    }
 //   moveScore /= moves.size();
    score = (1 * distScore) + (100 * goalScore) + (0 * moveScore);
    return score;
}

double DistanceStateEvaluator::getLowerBound() {
    return -10000;
}
double DistanceStateEvaluator::getUpperBound() {
    return 10000;
}