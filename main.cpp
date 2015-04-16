#include <cstddef>

#include "Agent.h"
#include "ChineseCheckersState.h"
#include <chrono>
#include <iostream>
#include <sstream>
#include "DistanceStateEvaluator.h"
#include "ZobristHash.h"
#include "TranspositionTable.h"


bool debugFlag = false;

std::stringstream debugStream;

int main(int /*argc*/, char **/*argv*/) {
    ZobristHashSetup();
    TranspositionTableSetup();
/*    DebugZHash();
    return EXIT_SUCCESS;*/
    Agent a;
    DistanceStateEvaluator distanceStateEval;
    a.setEvaluator(&distanceStateEval);
    a.playGame();
    return EXIT_SUCCESS;
}
