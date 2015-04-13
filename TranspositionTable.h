//
//  TranspositionTable.h
//  ChineseCheckersAI
//
//  Created by Mohammed Bellifa on 4/12/15.
//  Copyright (c) 2015 Mohammed Bellifa. All rights reserved.
//

#ifndef __ChineseCheckersAI__TranspositionTable__
#define __ChineseCheckersAI__TranspositionTable__

#include <stdio.h>
#include <cstdint>

const int TranspositionTableCacheSize = 524288;

struct TranspositionTableEntry {
    uint64_t hash;
    double alpha;
    double beta;
    double depthRemaining;
    double value;
};

//Set the hash to 0 for all cache entries, marking it as invalid.
void TranspositionTableSetup();


extern TranspositionTableEntry TranspositionTable[TranspositionTableCacheSize];

#endif /* defined(__ChineseCheckersAI__TranspositionTable__) */
