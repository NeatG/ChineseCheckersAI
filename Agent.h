//===------------------------------------------------------------*- C++ -*-===//
#ifndef AGENT_H_INCLUDED
#define AGENT_H_INCLUDED

#include <string>
#include <vector>

#include "ChineseCheckersState.h"
#include "StateEvaluator.h"
#include <chrono>
#include  <random>
#include  <iterator>
#include <unordered_map>



extern std::stringstream debugStream;

extern bool debugFlag;


struct moveOrderingSort {
    bool operator()( const Move &lhs, const Move &rhs) const;
};
// This map stores our scores for various moves. The move is turned into a uint32_t by bit shifting, since it takes at most 7 bits to store one location it takes 14 bits to store both.
extern std::unordered_map<uint32_t,double> moveOrdering;

class Agent {
    friend void TestTranspositionTable();
public:
    Agent();
    void playGame();
    void setEvaluator(StateEvaluator*);
    int getPlayerNumber() const;
private:
    Move nextMove();
    void printAndRecvEcho(const std::string &msg) const;
    std::string readMsg() const;
    std::vector<std::string> tokenizeMsg(const std::string &msg) const;
    void waitForStart();
    void switchCurrentPlayer();
    StateEvaluator* stateEval;
    bool isValidStartGameMessage(const std::vector<std::string> &tokens) const;
    bool isValidMoveMessage(const std::vector<std::string> &tokens) const;
 

    ChineseCheckersState state;
    enum Players { player1, player2 };
    Players current_player;
    Players my_player;
    int myPlayerNumber;
    std::string name;
    std::string opp_name;
    double miniMax(ChineseCheckersState& state, int depthRemaining, std::chrono::steady_clock::time_point timeLimit, double alpha, double beta, StateEvaluator& stateEval, bool maximizing);
    std::pair<Move,double> firstNode(ChineseCheckersState& state, int depthRemaining, std::chrono::steady_clock::time_point timeLimit, double alpha, double beta, StateEvaluator& stateEval);
};




#endif
