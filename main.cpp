// Group 2:
// Mohammed Bellifa and Stephen Fleming
//
// debugFlag global variable is below. Setting this to true verifies the alpha-beta utilizing minimax against the transposition table and it verifies the alpha beta search against a regular minimax search.
#include <cstddef>

#include "Agent.h"
#include "ChineseCheckersState.h"
#include <chrono>
#include <iostream>
#include <sstream>
#include "DistanceStateEvaluator.h"
#include "ZobristHash.h"
#include "TranspositionTable.h"

bool debugFlag = true; //Global debug flag, turning this on will cause the agent to no longer work within 10 seconds and will cause it to double check values from multiple functions (slow).

//This stringstream is used to print debug information _after_ the move timer has completed so that long debug messages don't disadvantage the agent.
std::stringstream debugStream;

int main(int /*argc*/, char **/*argv*/) {
    ZobristHashSetup();
    TranspositionTableSetup();
/*  Debugs the ZHash implementation by testing the incremental version vs. creating the whole hash at once and printing out hte results.
    DebugZHash();
 */
    Agent a;
    DistanceStateEvaluator distanceStateEval;
    a.setEvaluator(&distanceStateEval);
    a.playGame();
    return EXIT_SUCCESS;
}
