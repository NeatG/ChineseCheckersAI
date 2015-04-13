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
    for (int i = 0;i < 81;++i) {
        int thisRow = i / 9;
        int thisCol = i % 9;
        if (state.board[i] == forPlayer) {
            if (forPlayer == 1) {
                score += thisRow + thisCol;
            } else { //Player 2
                score += ((8 - thisRow) + (8 - thisCol));
            }
        } else if (state.board[i] == 3 - forPlayer) {
            if (forPlayer == 1) {
                score -= ((8 - thisRow) + (8 - thisCol));
            } else {
                score -= thisRow + thisCol;
            }
        }
    }
    /*std::vector<Move> moves;
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
    moveScore /= moves.size();
    score += moveScore;
    moveScore = 0;
    
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
    moveScore /= moves.size();
    score += moveScore;*/
  //  score += bestMoveScore / 16;
    /*state.swapTurn();
    state.getMoves(moves);
    curPlayer = state.getCurrentPlayer();
    for (auto mv : moves) {
        int toRow = mv.to / 9;
        int toCol = mv.to % 9;
        if (curPlayer == forPlayer) {
            if (forPlayer == 1) {
                score += toRow + toCol;
            } else { //Player 2
                score += (8 - toRow) + (8 - toCol);
            }
        } else if (curPlayer == 3 - forPlayer) {
            if (forPlayer == 1) {
                score -= ((8 - toRow) + (8 - toCol));
            } else {
                score -= toRow + toCol;
            }
        }
    }
    state.swapTurn();*/
    return score;
}

double DistanceStateEvaluator::getLowerBound() {
    return -10000;
}
double DistanceStateEvaluator::getUpperBound() {
    return 10000;
}