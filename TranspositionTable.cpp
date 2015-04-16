//
//  TranspositionTable.cpp
//  ChineseCheckersAI
//
//  Created by Mohammed Bellifa on 4/12/15.
//  Copyright (c) 2015 Mohammed Bellifa. All rights reserved.
//

#include "TranspositionTable.h"
#include "Agent.h"
#include "DistanceStateEvaluator.h"
#include <iostream>
//const size_t TranspositionTableCacheSize = 1024;

TranspositionTableEntry TranspositionTable[TranspositionTableCacheSize];

void TranspositionTableSetup() {
    for (int i = 0;i < TranspositionTableCacheSize;++i) {
        TranspositionTable[i].hash = 0;
    }
}
void TestTranspositionTable() {
    Agent test;
    DistanceStateEvaluator dist;
    test.setEvaluator(&dist);
    uint64_t hash = test.state.getHash();
    std::chrono::steady_clock::time_point timeLimit = std::chrono::steady_clock::now() + std::chrono::seconds(20);
    double score = test.miniMax(test.state, 4, timeLimit, dist.getLowerBound(), dist.getUpperBound(), dist, true);
    TranspositionTableEntry* cacheEntry = &TranspositionTable[hash % TranspositionTableCacheSize];
    std::vector<Move> moves;
    std::cerr << "TransP: " << cacheEntry->value << " actual: " << score << std::endl;
    test.state.getMoves(moves);
    for (auto mv : moves) {
        test.state.applyMove(mv);
        score = test.miniMax(test.state, 3, timeLimit, dist.getLowerBound(), dist.getUpperBound(), dist, false);
        cacheEntry = &TranspositionTable[test.state.getHash() % TranspositionTableCacheSize];
        if (cacheEntry->hash == test.state.getHash()) {
            std::cerr << "TransP " << cacheEntry->value << " Acutal: " << score << std::endl;
        }
        test.state.undoMove(mv);
    }
    
}