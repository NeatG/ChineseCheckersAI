//
//  TDLambdaStateEvaluator.cpp
//  ChineseCheckersAI
//
//  Created by Mohammed Bellifa on 4/29/15.
//  Copyright (c) 2015 Mohammed Bellifa. All rights reserved.
//

#include "TDLambdaStateEvaluator.h"
#include <iostream>

TDLambdaStateEvaluator::TDLambdaStateEvaluator() : numInputs(1350), r(numInputs) {
    epsilon = 0.05;
    lambda = 0.8;
}

double TDLambdaStateEvaluator::evaluate(ChineseCheckersState& state, int who, bool repeat) {
    getFeatures(state, features, who);
    double result = r.test(features);
    return result;
}

double TDLambdaStateEvaluator::getLowerBound() {
    return -15; //All of the negative weights added = -14.8
    //return -10000;
}
double TDLambdaStateEvaluator::getUpperBound() {
    return 16; //I've calculated based on the current hardcoded weights that 15.2 is the heighest socre you could get.
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
Move TDLambdaStateEvaluator::getNextAction(ChineseCheckersState &s, double &bestVal) {
    double val = rand() % 100000;
    val /= 100000;
    if (val < epsilon) { // select random move
        return s.getRandomMove();
    }
    std::vector<Move> moves;
    s.getMoves(moves);
    Move best = {0, 0};
    bestVal = -1000;
    int currentPlayer = s.currentPlayer;
    for (Move m : moves) {
        s.applyMove(m);
        getFeatures(s, features, currentPlayer);
        double value = r.test(features);
        if (value > bestVal || best == Move{0, 0}) {
            bestVal = value;
            best = m;
        }
        s.undoMove(m);
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
     //   std::cerr << "Win reward: " << winReward << " Lose Reward: " << loseReward << std::endl;
        
        if (winReward < 0.01) { winReward = 0.01; }
        if (winReward > 1) { winReward = 1; }
        if (loseReward > -0.01) { loseReward = -0.01; }
        if (loseReward < -1) { loseReward = -1; }
        
        
    }
}
void TDLambdaStateEvaluator::getFeatures(const ChineseCheckersState &s, std::vector<double> &features, int who) {
    features.clear();
    features.resize(numInputs);
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
        for (auto mod : {-1,8,9,1,-8,-9,-2,7,16,17,18,10,2,-7,-16,-17,-18,-10}) {
            if (who == 1) { mod *= -1; }
            int newPos = i + mod;
            if (newPos < 0 || newPos > 80) {
                //features[index] = 0;
                ++index;
                //features[index] = 0;
                ++index;
                features[index] = 1; //EXPERIMENTAL: Mark a piece of ours being on the edge as a feature.
                ++index;
                continue;
            }
            if (s.board[newPos] == who) {
                features[index] = 1;
                alone = false;
            } /*else {
                features[index] = 0;
            }*/
            ++index;
            if (s.board[newPos] == 3 - who) {
                features[index] = 1;
                alone = false;
            }/* else {
                features[index] = 0;
            }*/
            ++index;
         
            //features[index] = 0; //Piece is not on this particular edge.
            ++index;
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
    i = 0;
    int opp = 3 - who;
    if (opp == 2) { i = 80; }
    std::vector<Move> oppMoves;
    s.getMoves(oppMoves, who);
    while ((opp == 1 && i < 81) || (opp == 2 && i >= 0)) {
        if (s.board[i] != opp) {
            if (opp == 1) {
                i++;
            } else {
                i--;
            }
            continue;
        }
        bool alone = true;
        for (auto mod : {-1,8,9,1,-8,-9,-2,7,16,17,18,10,2,-7,-16,-17,-18,-10}) {
            if (opp == 1) { mod *= -1; }
            int newPos = i + mod;
            if (newPos < 0 || newPos > 80) {
                //features[index] = 0;
                ++index;
                //features[index] = 0;
                ++index;
                features[index] = 1; //EXPERIMENTAL: Mark a piece of ours being on the edge as a feature.
                ++index;
                continue;
            }
            if (s.board[newPos] == opp) {
                features[index] = 1;
                alone = false;
            } /*else {
                features[index] = 0;
            }*/
            ++index;
            if (s.board[newPos] == 3 - opp) {
                features[index] = 1;
                alone = false;
            } /*else {
                features[index] = 0;
            }*/
            ++index;
            
            //features[index] = 0; //Piece is not on this particular edge.
            ++index;
        }
        
        features[index] = alone;
        ++index;
        bool blocked = true;
        bool canJump = false;
        for (auto mv : oppMoves) {
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
        if (opp == 1) {
            i++;
        } else {
            i--;
        }
    }
}
void TDLambdaStateEvaluator::setWeights(std::vector<double> weights) { //Import saved weights
    r.SetWeights(weights);
    
}
