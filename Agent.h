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
#include <unordered_set>
#include <unordered_map>



extern std::stringstream debugStream;

extern bool debugFlag;


struct moveOrderingSort {
    bool operator()( const Move &lhs, const Move &rhs) const;
};
// This map stores our scores for various moves. The move is turned into a uint32_t by bit shifting, since it takes at most 7 bits to store one location it takes 14 bits to store both.
extern double moveOrdering[32767];
struct MCTSNode {
    uint32_t numberChildren;
    uint32_t indexFirstChild;
    uint32_t samples;
    double payOff;
    bool maximizing; //Essentially this is True if the node is on our move.
    uint32_t parentIndex;
    Move gotToHere;
};

class Agent {
public:
    Agent();
    void playGame();
    void setEvaluator(StateEvaluator*);
    int getPlayerNumber() const;
    void generateOpeningBook(int secondsPerMove);
    void setName(std::string);
    void setUCBDepth(int);
    void setSearchType(std::string);
    void setExploration(double);
    void calculateExploration(double increment, int direction = 0); //Sets the exploration parameter in UCB based on our evaluation function

private:
    Move nextMove(int milliseconds = 9950);
    void printAndRecvEcho(const std::string &msg) const;
    std::string readMsg() const;
    std::vector<std::string> tokenizeMsg(const std::string &msg) const;
    void waitForStart();
    void switchCurrentPlayer();
    StateEvaluator* stateEval;

    bool isValidStartGameMessage(const std::vector<std::string> &tokens) const;
    bool isValidMoveMessage(const std::vector<std::string> &tokens) const;
    
    //UCB Stuff
    std::vector<MCTSNode> MCTree;

    int ucbDepth = 0;
    Move GetBestMove(); // the high-level function for computing the best move
    double SelectLeaf(uint32_t node); // traverse down the tree (recursively), returning the value at the leaf of the sample
    uint32_t SelectBestChild(uint32_t node); // Use the UCB rule to find the best child
    double GetUCBVal(uint32_t node, uint32_t parent); // Get the UCB value of a given node
    bool IsLeaf(uint32_t node); // is the designated node a leaf
    void Expand(uint32_t node); // expand the designated node and add its children to the tree
    void DoPlayOutThread(uint32_t node, int depth, double* result);
    double DoPlayout(uint32_t node, int depth); // play out the game, returning the evaluation at the end of the game
    void GetNodeState(uint32_t node, ChineseCheckersState& state);
    double exploration = 1.175;
    double sampleRatio();
    std::unordered_set<uint64_t> visitedStates;
    ChineseCheckersState state;
    enum Players { player1, player2 };
    Players current_player;
    Players my_player;
    int myPlayerNumber;
    enum searchType { MINIMAX, MCTS };
    searchType currentSearchType = MINIMAX;
    std::string name;
    std::string opp_name;
    //Overloaded minimax that does not check transposition tables or use alpha beta
    double miniMax(ChineseCheckersState& state, int depthRemaining, StateEvaluator& stateEval, bool maximizing);
    double miniMax(ChineseCheckersState& state, int depthRemaining, std::chrono::steady_clock::time_point timeLimit, double alpha, double beta, StateEvaluator& stateEval, bool maximizing);
    std::pair<Move,double> firstNode(ChineseCheckersState& state, int depthRemaining, std::chrono::steady_clock::time_point timeLimit, double alpha, double beta, StateEvaluator& stateEval);
};




#endif
