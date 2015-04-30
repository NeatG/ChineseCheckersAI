// -*- C++ -*-
//  TDRegression.h
//  CC UCT
//
//  Created by Nathan Sturtevant on 4/18/15.
//  Copyright (c) 2015 Nathan Sturtevant. All rights reserved.
//

#ifndef TD_REGRESSION_H
#define TD_REGRESSION_H

#include <stdio.h>
#include <assert.h>
#include <string>
#include "ChineseCheckersState.h"
#include "Player.h"
#include "LinearRegression.h"

class TDEval
{
public:
	TDEval() :r((81 * 2) + 360 + 60, 0.01) {}
	double eval(ChineseCheckersState &s, int whichPlayer);
	const char *GetName() { return "TD"; }
	void SetWeights(const std::vector<double> &w) { r.SetWeights(w); }
private:
	void GetFeatures(const ChineseCheckersState &s, std::vector<double> &features, int who);
	LinearRegression r;
	std::vector<double> features;
};

class TDRegression : public Player
{
public:
	TDRegression(double e, double l) : epsilon(e), lambda(l), r((81 * 2) + 360 + 60, 0.001) {}
	Move GetNextAction(const ChineseCheckersState &s, double &,
					   double timeLimit, int depthLimit = 100000);
	void Train(const std::vector<Move> &trace);
	void Print();
	virtual const char *GetName() { return "TD-Regression"; }
	const std::vector<double> &GetWeights() { return r.GetWeights(); }
	
private:
	void GetFeatures(const ChineseCheckersState &s, std::vector<double> &features, int who);
	LinearRegression r;
	double epsilon, lambda;
};

#endif
