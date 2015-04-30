//
//  Player.h
//  CC UCT
//
//  Created by Nathan Sturtevant on 4/17/15.
//  Copyright (c) 2015 Nathan Sturtevant. All rights reserved.
//

#ifndef __CC_UCT__Player__
#define __CC_UCT__Player__

#include "ChineseCheckersState.h"

class Player
{
public:
	virtual Move GetNextAction(const ChineseCheckersState &s, double &value,
							   double timeLimit, int depthLimit = 100000) = 0;
	virtual const char *GetName() = 0;
};

#endif /* defined(__CC_UCT__Player__) */
