//
//  DistanceStateEvaluator.h
//  HW2
//
//  Created by Mohammed Bellifa on 4/4/15.
//  Copyright (c) 2015 Mohammed Bellifa. All rights reserved.
//

#ifndef __HW2__DistanceStateEvaluator__
#define __HW2__DistanceStateEvaluator__

#include <stdio.h>
#include "StateEvaluator.h"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <string>
class DistanceStateEvaluator : public StateEvaluator {
public:
    DistanceStateEvaluator();
    double getUpperBound();
    double getLowerBound();
    double evaluate(ChineseCheckersState &state, int forPlayer, bool repeat=true);
private:
    double lowerBound = -324;
    double upperBound = 324;
    //std::unordered_map<ChineseCheckersState,double> memoMap;
};

#endif /* defined(__HW2__DistanceStateEvaluator__) */
