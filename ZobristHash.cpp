//
//  ZobristHash.cpp
//  ChineseCheckersAI
//
//  Created by Mohammed Bellifa on 4/12/15.
//  Copyright (c) 2015 Mohammed Bellifa. All rights reserved.
//

#include "ZobristHash.h"
#include <iostream>

uint64_t zhash[3][82]; //Global array for the zobrist hash
//82nd value matches player turn

uint64_t ZobristHash(ChineseCheckersState& state)
{
    uint64_t toReturn = 0;
    for (int i = 0; i < 81; i++)
    {
        toReturn = toReturn ^ zhash[state.board[i]][i];
    }
    //Uncomment this to allow the player turn to be a part of the hash
    //toReturn ^= zhash[state.getCurrentPlayer()][81];
    return toReturn;
}
void ZobristHash(ChineseCheckersState& state, uint64_t& current, size_t index)
{
    current = current ^ zhash[state.board[index]][index];
}
void ZobristHashSetup()
{
    for (int j = 0; j < 82; j++)
    {
        for (int i = 0; i < 3; i++)
        {
            uint64_t temp = random();
            temp = temp << 32;
            temp = temp | random();
            zhash[i][j] = temp;
        }
    }
}
void PrintZHash()
{
    for (int j = 0; j < 81; j++)
    {
        for (int i = 0; i < 3; i++)
        {
            std::cerr << "[" << i << "][" << j << "] = " << zhash[i][j] << std::endl;
        }
    }
}

void DebugZHash() {
    ChineseCheckersState test;
    std::cerr << test.getHash() << " = " << ZobristHash(test) << std::endl;
    std::vector<Move> moves;
    test.getMoves(moves);
    test.applyMove(moves[0]);
    std::cerr << "After move: " << test.getHash() << " = " << ZobristHash(test) << std::endl;
    test.undoMove(moves[0]);
    std::cerr << "After undo move: " << test.getHash() << " = " << ZobristHash(test) << std::endl;
    test.swapTurn();
    std::cerr << "After swap turn: " << test.getHash() << " = " << ZobristHash(test) << std::endl;
    test.swapTurn();
    std::cerr << "After swap turnx2: " << test.getHash() << " = " << ZobristHash(test) << std::endl;

}