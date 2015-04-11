#include <cstddef>

#include "Agent.h"
#include "ChineseCheckersState.h"
#include <chrono>
#include <iostream>
#include <sstream>
#include "DistanceStateEvaluator.h"



std::stringstream debug;

int main(int /*argc*/, char **/*argv*/) {
    Agent a;
    DistanceStateEvaluator distanceStateEval;
    a.setEvaluator(&distanceStateEval);
    a.playGame();
    return EXIT_SUCCESS;
}
