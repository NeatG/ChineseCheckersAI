//
//  TDLambdaStateEvaluator.cpp
//  ChineseCheckersAI
//
//  Created by Mohammed Bellifa on 4/29/15.
//  Copyright (c) 2015 Mohammed Bellifa. All rights reserved.
//

#include "TDLambdaStateEvaluator.h"
#include <iostream>

TDLambdaStateEvaluator::TDLambdaStateEvaluator() : numInputs((81 * 2) + 360 + 60), r(numInputs,0.01) {
    epsilon = 0.05;
    lambda = 0.8;
}

double TDLambdaStateEvaluator::evaluate(ChineseCheckersState& state, int who, bool repeat) {
    getFeatures(state, features, who);
    double result = r.test(features);
    return result;
}

double TDLambdaStateEvaluator::getLowerBound() {
    return -26; //All of the negative weights added = -25.384
    //return -10000;
}
double TDLambdaStateEvaluator::getUpperBound() {
    return 18; //I've calculated based on the current hardcoded weights that 17.3 is the heighest socre you could get.
    //return 1000;
}
void TDLambdaStateEvaluator::train(int numTrains) {

    std::vector<Move> moves;
    ChineseCheckersState s;
    for (int x = 0; x < numTrains; x++) {
        if (0 == x%100) {
            std::cerr << "Playing game " << x << "\n";
        }
        s.reset();
        moves.resize(0);
        
        while (!s.gameOver()) {
            double value;
            //s.Print();
            //std::cerr << "ABout to get an action" << std::endl;
            Move next = getNextAction(s, value);
            
            //std::cerr << s.currentPlayer << ": " << next.from << " to " << next.to << std::endl;
            s.applyMove(next);
            moves.push_back(next);
            //s.PrintASCII();
            
        }
        trainFromTrace(moves);
        
    }
    
    // load into td
    //tdmm.GetEval().SetWeights(tdr.GetWeights());
    // print for ourselves
    std::cerr << "Weights: {";
    int size = r.GetWeights().size();
    for (int i = 0;i < size; ++i) {
        std::cerr << r.GetWeights()[i];
        if (i < size - 1) {
            std::cerr << ", ";
        }
    }
    std::cerr << "}" << std::endl;
//    fprintf(stderr, "Our weights:\n");
//    for (int x = 0; x < 9; x++)
//    {
//        for (int y = 0; y < 9; y++)
//        {
//            fprintf(stderr, "%1.3f ", r.GetWeights()[x+y*9]);
//        }
//        fprintf(stderr, "\n");
//    }
//    
//    fprintf(stderr, "Opponent weights:\n");
//    for (int x = 0; x < 9; x++)
//    {
//        for (int y = 0; y < 9; y++)
//        {
//            fprintf(stderr, "%1.3f ", r.GetWeights()[81+x+y*9]);
//        }
//        fprintf(stderr, "\n");
//    }
//    fprintf(stderr, "Piece weights:\n");
//    for (int x = 0; x < 420; x++)
//    {
//        
//        fprintf(stderr, "%1.3f ", r.GetWeights()[162+x]);
//    }
} //Train the evaluator with

// Returns the best move (1-ply) for the purposes of training
Move TDLambdaStateEvaluator::getNextAction(const ChineseCheckersState &s, double &bestVal) {
    ChineseCheckersState state = s;
    double val = rand() % 100000;
    val /= 100000;
    if (val < epsilon) { // select random move
        return s.getRandomMove();
    }
    std::vector<Move> moves;
    s.getMoves(moves);
    Move best = {0, 0};
    bestVal = -1000;
    
    for (Move m : moves) {
        state.applyMove(m);
        getFeatures(state, features, s.currentPlayer);
        double value = r.test(features);
        if (value > bestVal || best == Move{0, 0}) {
            bestVal = value;
            best = m;
        }
        state.undoMove(m);
    }
    if (best == Move{0, 0}) { // only sideways moves
        return s.getRandomMove();
    }
    return best;
}


void TDLambdaStateEvaluator::trainFromTrace(const std::vector<Move> &trace)
{
    ChineseCheckersState s;
    for (auto m : trace) {
        s.applyMove(m);
    }
    
    // Game should be done at the end of the trace
    assert(s.gameOver());
    int winner = s.winner();
    assert(winner != -1);
    
  //  std::vector<double> features;
    
    double winReward = 1.0;
    double loseReward = -1.0;
    
    // Step backwards through the code training
    for (int x = trace.size() - 1; x >= 0; x--) {
        s.undoMove(trace[x]);
        getFeatures(s, features, s.currentPlayer);
        if (winner == s.currentPlayer) {
            r.train(features,winReward);
            winReward = (1 - lambda) * r.test(features) + lambda * winReward;
        } else {
            r.train(features,loseReward);
            loseReward = (1 - lambda) * r.test(features) + lambda * loseReward;
        }
        
        
    }
}
void TDLambdaStateEvaluator::getFeatures(const ChineseCheckersState &s, std::vector<double> &features, int who) {
    features.clear();
    features.resize(numInputs); // 360 = 18 fields around each piece x 2 for ours or our opponents
    for (int x = 0; x < 81; x++)
    {
        if (s.board[x] == who)
        {
            if (who == 1)
                features[x] = 1;
            else
                features[80 - x] = 1;
        } else if (s.board[x] != 0)
        {
            if (who == 1)
                features[81 + x] = 1;
            else
                features[81 + 80 - x] = 1;
        }
    }
    int i = 0;
    int index = 162;
    if (who == 2) { i = 80; }
    std::vector<Move> ourMoves;
    s.getMoves(ourMoves, who);
    while ((who == 1 && i < 81) || (who == 2 && i >= 0)) {
        if (s.board[i] != who) {
            if (who == 1) {
                i++;
            } else {
                i--;
            }
            continue;
        }
        bool alone = true;
        if (who == 2) {
            for (auto mod : {-9,-8,1,9,8,-1,-18,-17,-16,-7,2,10,18,17,16,7,-2,-10}) {
                int newPos = i + mod;
                if (newPos < 0 || newPos > 80) {
                    features[index] = 0;
                    ++index;
                    features[index] = 0;
                    ++index;
                    continue;
                }
                if (s.board[newPos] == who) {
                    features[index] = 1;
                    alone = false;
                } else {
                    features[index] = 0;
                }
                ++index;
                if (s.board[newPos] == 3 - who) {
                    features[index] = 1;
                    alone = false;
                } else {
                    features[index] = 0;
                }
                ++index;
            }
            
        } else {
            for (auto mod : {1,-8,-9,-1,8,9,2,-7,-16,-17,-18,-10,-2,7,16,17,18,10}) {
                int newPos = i + mod;
                if (newPos < 0 || newPos > 80) {
                    features[index] = 0;
                    ++index;
                    features[index] = 0;
                    ++index;
                    continue;
                }
                if (s.board[newPos] == who) {
                    features[index] = 1;
                    alone = false;
                } else {
                    features[index] = 0;
                }
                ++index;
                if (s.board[newPos] == 3 - who) {
                    features[index] = 1;
                    alone = false;
                } else {
                    features[index] = 0;
                }
                ++index;
            }
        }
        features[index] = alone;
        ++index;
        bool blocked = true;
        bool canJump = false;
        for (auto mv : ourMoves) {
            if (mv.from == i) {
                blocked = false;
                int to = mv.to;
                int from = mv.from;
                int dist = abs(from - to);
                if ((dist % 9) + (dist / 9) > 1) {
                    canJump = true;
                    break;
                }
            }
        }
        
        features[index] = blocked;
        ++index;
        
        features[index] = canJump;
        
        ++index;
        if (who == 1) {
            i++;
        } else {
            i--;
        }
    }
}
void TDLambdaStateEvaluator::setWeights(std::vector<double> weights) { //Import saved weights
    r.SetWeights(weights);
    
}
