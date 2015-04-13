//
//  ZobristHash.h
//  ChineseCheckersAI
//
//  Created by Mohammed Bellifa on 4/12/15.
//  Copyright (c) 2015 Mohammed Bellifa. All rights reserved.
//

#ifndef __ChineseCheckersAI__ZobristHash__
#define __ChineseCheckersAI__ZobristHash__

#include "ChineseCheckersState.h"
#include <stdio.h>
#include <cstdint>


extern uint64_t zhash[3][82];//the Zobrist hash table random values, the 82nd value is to represent player turn

//Computes the value of the entire board given a state.
uint64_t ZobristHash(ChineseCheckersState& state);
//Modifies the current Zobrist Hash value by ^ with zhash[state.board[index]][index].
void ZobristHash(ChineseCheckersState& state, uint64_t& current, size_t index);
//Fill in the zhash table with random values.
void ZobristHashSetup();
//Print the zhash table for testing purposes.
void PrintZHash();
//Tests iteratative hash building vs. incremental
void DebugZHash();

#endif /* defined(__ChineseCheckersAI__ZobristHash__) */
