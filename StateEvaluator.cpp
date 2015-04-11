//
//  StateEvaluator.cpp
//  HW2
//
//  Created by Mohammed Bellifa on 4/4/15.
//  Copyright (c) 2015 Mohammed Bellifa. All rights reserved.
//

#include "StateEvaluator.h"


double StateEvaluator::evaluate(ChineseCheckersState&,int,bool repeat) {
    return 1;
}
double StateEvaluator::getLowerBound() {
    return -1;
}
double StateEvaluator::getUpperBound() {
    return 1;
}
