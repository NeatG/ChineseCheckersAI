//===------------------------------------------------------------*- C++ -*-===//
#ifndef AGENT_H_INCLUDED
#define AGENT_H_INCLUDED

#include <string>
#include <vector>

#include "ChineseCheckersState.h"
#include "StateEvaluator.h"
#include <chrono>

class Agent {
public:
  Agent();
  void playGame();

private:
  int distanceScore(Move mv, int goal); //Returns a score based on how good the move is, based on distance.
  Move nextMove();
  void printAndRecvEcho(const std::string &msg) const;
  std::string readMsg() const;
  std::vector<std::string> tokenizeMsg(const std::string &msg) const;
  void waitForStart();
  void switchCurrentPlayer();
  StateEvaluator stateEval;
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
