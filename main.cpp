#include <cstddef>

#include "Agent.h"
#include "ChineseCheckersState.h"
#include <chrono>
#include <iostream>
std::string DFSmoveSearch(ChineseCheckersState &a, int minMoves, int depth=0) {
    std::set<Move> moves;
    a.getMoves(moves);
    if (moves.size() > minMoves) { return a.dumpState(); }
    if (depth > 100) { return ""; }
    for (auto mv : moves) {
        a.applyMove(mv);
        std::string res = DFSmoveSearch(a,minMoves,depth+1);
        if (res.length() > 0) { return res; }
        a.undoMove(mv);
    }
    return "";
}
int main(int /*argc*/, char **/*argv*/) {
    /*using namespace std::chrono;
    ChineseCheckersState a;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    //std::cout << DFSmoveSearch(a,60);
    a.loadState("2 1 0 1 1 0 0 0 0 0 1 1 1 0 0 0 0 0 0 1 1 1 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 2 0 0 0 0 0 2 0 0 0 0 0 0 0 0 2 2 0 0 0 0 0 0 0 0 0 2 0 0 0 0 0 0 2 2 0 0 0 0 0 0 2 0 2 2");
    std::set<Move> moves;
    for (int i = 0;i < 1000000;++i) {
        a.getMoves(moves);
    }
    /*std::cout << moves.size() << std::endl;
    std::cout << a.dumpState();
    for (auto mv : moves) {
        std::cout << "From : " << mv.from << " to " << mv.to << std::endl;
    }
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    
    std::cout << "It took me " << time_span.count() << " seconds.";
    std::cout << std::endl;*/
    Agent a;
  a.playGame();
  return EXIT_SUCCESS;
}
