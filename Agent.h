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
//Code from http://stackoverflow.com/questions/6942273/get-random-element-from-container-c-stl
template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}

class Agent {
public:
    Agent();
    void setEvaluator(StateEvaluator*);
    void playGame();

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
    std::string name;
    std::string opp_name;
};
double minNode(ChineseCheckersState& state, int depthRemaining, std::chrono::steady_clock::time_point timeLimit, double alpha, double beta, StateEvaluator& stateEval);
double maxNode(ChineseCheckersState& state, int depthRemaining, std::chrono::steady_clock::time_point timeLimit, double alpha, double beta, StateEvaluator& stateEval);
Move firstNode(ChineseCheckersState& state, int depthRemaining, std::chrono::steady_clock::time_point timeLimit, double alpha, double beta, StateEvaluator& stateEval);



#endif
