//
//  TDRegression.cpp
//  CC UCT
//
//  Created by Nathan Sturtevant on 4/18/15.
//  Copyright (c) 2015 Nathan Sturtevant. All rights reserved.
//

#include "TDRegression.h"

#include <cstdlib>
#include <iostream>
double TDEval::eval(ChineseCheckersState &s, int whichPlayer)
{
	GetFeatures(s, features, whichPlayer);
	double result = r.test(features);
	return result;
}

void TDEval::GetFeatures(const ChineseCheckersState &s, std::vector<double> &f, int who)
{
    features.resize(0);
    features.resize((81 * 2) + 360 + 60); // 360 = 18 fields around each piece x 2 for ours or our opponents
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

void TDRegression::GetFeatures(const ChineseCheckersState &s, std::vector<double> &features,
							   int who)
{
	features.resize(0);
    features.resize((81 * 2) + 360 + 60); // 360 = 18 fields around each piece x 2 for ours or our opponents
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

Move TDRegression::GetNextAction(const ChineseCheckersState &s,
								 double &bestVal, double timeLimit, int)
{
	ChineseCheckersState state = s;
	double val = rand() % 100000;
	val /= 100000;
	if (val < epsilon) // select random move
		
	{
		return s.getRandomMove();
	}
	std::vector<Move> moves;
	s.getMoves(moves);
	Move best = {0, 0};
	bestVal = -1000;
	std::vector<double> features;
	for (Move m : moves)
	{
		state.applyMove(m);
		GetFeatures(state, features, s.currentPlayer);
        double value = r.test(features);
		if (value > bestVal || best == Move{0, 0})
		{
			bestVal = value;
			best = m;
		}
		state.undoMove(m);
	}
	if (best == Move{0, 0}) // only sideways moves
	{
		return s.getRandomMove();
	}
	return best;
}

void TDRegression::Train(const std::vector<Move> &trace)
{
	ChineseCheckersState s;
	for (auto m : trace)
	{
		s.applyMove(m);
	}

	// Game should be done at the end of the trace
	assert(s.gameOver());
	int winner = s.winner();
	assert(winner != -1);
	
	std::vector<double> features;
	
	double winReward = 1.0;
	double loseReward = -1.0;
    
	// Step backwards through the code training
	for (int x = trace.size() - 1; x >= 0; x--)
	{
		s.undoMove(trace[x]);
        GetFeatures(s, features, s.currentPlayer);
        if (winner == s.currentPlayer) {
            r.train(features,winReward);
            winReward = (1 - lambda) * r.test(features) + lambda * winReward;
        } else {
            r.train(features,loseReward);
            loseReward = (1 - lambda) * r.test(features) + lambda * loseReward;
        }
    	
	
	}
}

void TDRegression::Print()
{
	auto w = r.GetWeights();
	for (int x = 0; x < 2; x++)
	{
		if (x == 0)
			printf("  Us: ");
		if (x == 1)
			printf("Them: ");
		for (int y = 0; y < 81; y++)
		{
			printf("%1.3f ", w[y + 81 * x]);
		}
		printf("\n");
	}
}
