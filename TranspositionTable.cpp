//
//  TranspositionTable.cpp
//  ChineseCheckersAI
//
//  Created by Mohammed Bellifa on 4/12/15.
//  Copyright (c) 2015 Mohammed Bellifa. All rights reserved.
//

#include "TranspositionTable.h"

//const size_t TranspositionTableCacheSize = 1024;

TranspositionTableEntry TranspositionTable[TranspositionTableCacheSize];

void TranspositionTableSetup() {
    for (int i = 0;i < TranspositionTableCacheSize;++i) {
        TranspositionTable[i].hash = 0;
    }
}