//
//  StateEvaluator.h
//  HW2
//
//  Created by Mohammed Bellifa on 4/4/15.
//  Copyright (c) 2015 Mohammed Bellifa. All rights reserved.
//

#ifndef __HW2__StateEvaluator__
#define __HW2__StateEvaluator__
#include <stdio.h>
#include "ChineseCheckersState.h"




class StateEvaluator {
public:
    virtual double evaluate(ChineseCheckersState&,int,bool repeat = true);
    virtual double getLowerBound();
    virtual double getUpperBound();
private:
    double lowerBound;
    double upperBound;
};

#endif /* defined(__HW2__StateEvaluator__) */
