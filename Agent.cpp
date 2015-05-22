#include "Agent.h"

#include <cstdlib>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <thread>

#include <algorithm>
#include "TranspositionTable.h"


Agent::Agent() : name("BestAI") {
    for (int i = 0;i < 32767;++i) {
        moveOrdering[i] = 0;
    }
}
void Agent::setName(std::string _name) {
    name = _name;
}
// Stores scores for our move ordering.
// Uncomment for move ordering.

double moveOrdering[32767]; //It takes 7 bits to represent the from/to of a move, so it takes 14 bits to represent them both which can represent 2^15-1 values
bool moveOrderingSort::operator()( const Move &lhs, const Move &rhs) const {
    uint32_t lhsMoveRepresentation = uint32_t(lhs);
    uint32_t rhsMoveRepresentation = uint32_t(rhs);
    double lhsScore = moveOrdering[lhsMoveRepresentation];
    double rhsScore = moveOrdering[rhsMoveRepresentation];

    if (lhsScore > rhsScore) { return true; }
    if (rhsScore > lhsScore) { return false; }
    return rhs < lhs;
    
}
//This is an overload of the real function we're using that does not use alpha beta or check transposition tables.
double Agent::miniMax(ChineseCheckersState& state, int depthRemaining, StateEvaluator& stateEval, bool maximizing) {
    int stateWinner = state.winner();
    if (stateWinner != -1) {
        if (stateWinner == myPlayerNumber) { return stateEval.getUpperBound() + depthRemaining; } //Adding the depth remaining is a hack to try to get the agent to finish the game if it sees a winning state
        else { return stateEval.getLowerBound() + stateEval.evaluate(state, myPlayerNumber); }
    }
    if (depthRemaining == 0) {
        return stateEval.evaluate(state, myPlayerNumber);
    }
    double result;
    if (maximizing) { result = stateEval.getLowerBound(); }
    else { result = stateEval.getUpperBound(); }
    
    std::vector<Move> moves;
    state.getMoves(moves);

    //std::sort(moves.begin(),moves.end(),moveOrderingSort());
    for (auto mv : moves) {
        state.applyMove(mv);
        double temp = miniMax(state,depthRemaining - 1,stateEval,!maximizing);
        if (maximizing) {
            result = std::max(result,temp);
        } else {
            result = std::min(result,temp);
        }
        state.undoMove(mv);
    }
    return result;
}


double Agent::miniMax(ChineseCheckersState& state, int depthRemaining, std::chrono::steady_clock::time_point timeLimit, double alpha, double beta, StateEvaluator& stateEval, bool maximizing) {
    int stateWinner = state.winner();
    if (stateWinner != -1) {
        if (stateWinner == myPlayerNumber) { return stateEval.getUpperBound() + depthRemaining; } //Adding the depth remaining is a hack to try to get the agent to finish the game if it sees a winning state
        else { return stateEval.getLowerBound() + stateEval.evaluate(state, myPlayerNumber); }
    }
    
    if (std::chrono::steady_clock::now() > timeLimit) {
        return 0; //Doesn't matter what we return because the results will be discarded
    }
    if (depthRemaining == 0) {
        return stateEval.evaluate(state, myPlayerNumber);
    }
    //Check the cache.
    uint64_t hash = state.getHash();
    if (visitedStates.find(hash) != visitedStates.end()) {
        //If we repeat hte state it's a loss for the player playing.
        if (stateWinner == myPlayerNumber) {
            return stateEval.getLowerBound();
        } else { //If our opponent takes a repeat move it's a win for us.
            return stateEval.getUpperBound();
        }
    }
    TranspositionTableEntry* cacheEntry = nullptr;
    if (depthRemaining >= 2) {
        cacheEntry = &TranspositionTable[hash % TranspositionTableCacheSize];
        if (!debugFlag && cacheEntry->hash == hash && cacheEntry->depthRemaining >= depthRemaining) { //Cache hit!
            bool returnBoundedVal = false;
            if (cacheEntry->value > cacheEntry->alpha && cacheEntry->value < cacheEntry->beta) {
                //Value is inbetween the alpha and beta used at the time, so it is the true value
                returnBoundedVal = true;
            }
            if (!maximizing && cacheEntry->value == cacheEntry->beta && cacheEntry->value >= beta) {
                //We are minimizing and the value is greater or equal to our upper bound
                returnBoundedVal = true;
            }
            if (maximizing && cacheEntry->value == cacheEntry->alpha && cacheEntry->value <= alpha) {
                //We are maximizing and the value is less than or equal to our current lower bound
                returnBoundedVal = true;
            }
            if (returnBoundedVal) {
                if (maximizing) { return std::max(alpha,cacheEntry->value); }
                return std::min(beta,cacheEntry->value);
            }
            //If we're still here then we have a cache hit but the value only helps us limit our search
            if (maximizing && cacheEntry->value == cacheEntry->beta && alpha < cacheEntry->beta) {
                //We're maximizing and our transposition table contains a value that is greater than or equal to the beta value so we need to do a full search to get the true value. We update alpha to make the search faster.
                alpha = cacheEntry->beta;
            }
            if (!maximizing && cacheEntry->value == cacheEntry->alpha && beta > cacheEntry->alpha) {
                //We're maximizing and our transposition table contains a value that is less than or equal to the alpha value so we need to do a full search to get the true value. We update alpha to make the search faster.
                beta = cacheEntry->alpha;
            }
        }
    }
    std::vector<Move> moves;
    state.getMoves(moves);
    std::sort(moves.begin(),moves.end(),moveOrderingSort());
    Move bestMove = {0,0};
    for (auto &mv : moves) {
        state.applyMove(mv);
        double temp = miniMax(state,depthRemaining - 1,timeLimit,alpha,beta,stateEval,!maximizing);
        if (maximizing) {
            if (temp > alpha) {
                alpha = temp;
                bestMove = mv;
            }
            //alpha = std::max(alpha,temp);
        } else {
            if (temp < beta) {
                beta = temp;
                bestMove = mv;
            }
            //beta = std::min(beta,temp);
        }
        
        state.undoMove(mv);
        if (alpha >= beta) {
            break;
        }
    }
    if (bestMove != Move{0,0}) {
        uint32_t moveRepresentation = uint32_t(bestMove);
        moveOrdering[moveRepresentation] += pow(2,depthRemaining);
    }
    if (debugFlag && depthRemaining >= 2 && cacheEntry->hash == hash && cacheEntry->depthRemaining >= depthRemaining) {
        bool returnBoundedVal = false;
        if (cacheEntry->value > cacheEntry->alpha && cacheEntry->value < cacheEntry->beta) {
            //Value is inbetween the alpha and beta used at the time, so it is the true value
            returnBoundedVal = true;
        }
        if (!maximizing && cacheEntry->value == cacheEntry->beta && cacheEntry->value >= beta) {
            //We are minimizing and the value is greater or equal to our upper bound
            returnBoundedVal = true;
        }
        if (maximizing && cacheEntry->value == cacheEntry->alpha && cacheEntry->value <= alpha) {
            //We are maximizing and the value is less than or equal to our current lower bound
            returnBoundedVal = true;
        }
        if (returnBoundedVal) {
            if (maximizing && alpha != std::max(alpha,cacheEntry->value)) {
                debugStream << "Transposition table returned incorrect value (alpha)!" << std::endl;
            } else if (!maximizing && beta != std::min(beta,cacheEntry->value)) {
                debugStream << "Transposition table returned incorrect value (beta)!" << std::endl;
            }
        }
    }
    if (cacheEntry != nullptr) {
        cacheEntry->hash = hash;
        cacheEntry->alpha = alpha;
        cacheEntry->beta = beta;
        cacheEntry->depthRemaining = depthRemaining;
        if (maximizing) {
            cacheEntry->value = alpha;
        } else {
            cacheEntry->value = beta;
        }
    }
    if (maximizing) {
        
        return alpha;
    }
    return beta;
}
std::pair<Move,double> Agent::firstNode(ChineseCheckersState& state, int depthRemaining, std::chrono::steady_clock::time_point timeLimit, double alpha, double beta, StateEvaluator& stateEval) {
    std::vector<Move> moves;
    std::pair<Move,double> retVal;
    state.getMoves(moves);
    std::vector<Move> betterMoves;
    retVal.second = alpha - 1;
    //std::cerr << "Best Score: " << bestScore << std::endl;
    
    std::sort(moves.begin(),moves.end(),moveOrderingSort());
    for (auto mv : moves) {
        state.applyMove(mv);
        if (mv == *moves.begin()) { //If the move is the first move we do a full window search
            double minNodeScore = miniMax(state,depthRemaining - 1,timeLimit,alpha,beta,stateEval,false);
            if (debugFlag) {
                //Test our value against a basic minimax implementation
                double miniMaxResult = miniMax(state, depthRemaining - 1, stateEval, false);
                if (minNodeScore != std::min(beta,miniMaxResult)) {
                    debugStream << "Alpha Beta Error!" << std::endl;
                }
            }
            alpha = std::max(alpha,minNodeScore);
            retVal.second = minNodeScore;
            retVal.first = mv;
        } else { //Otherwise we do a null window search
            double minNodeScore = miniMax(state,depthRemaining - 1,timeLimit,alpha-0.1,alpha+0.1,stateEval,false);
            if (minNodeScore > alpha) {
                betterMoves.push_back(mv);
            }
        }
        
        state.undoMove(mv);

        
        //debugStream << "Depth " << depthRemaining << " Move from " << mv.from << " to " << mv.to << " : " << minNodeScore << std::endl;
        
        //std::cerr << "Alpha: " << alpha << " BETA: " << beta << std::endl;
        if (alpha >= beta) { break; }
    }
    size_t betterMovesSize = betterMoves.size();
    debugStream << betterMovesSize << " better moves out of " << moves.size() << " total moves." << std::endl;
    if (betterMovesSize > 0) {
        if (betterMovesSize == 1) {
            //There is only one better move than the first move so return it
            retVal.first = betterMoves[0];
            retVal.second = alpha + 0.1;
        } else {
            //If we have more than one then we have to do a full window search on a hopefully condensed list
            for (auto mv : betterMoves) {
                state.applyMove(mv);
                double minNodeScore = miniMax(state,depthRemaining - 1,timeLimit,alpha,beta,stateEval,false);
                alpha = std::max(alpha,minNodeScore);
                if (minNodeScore > retVal.second) {
                    retVal.first = mv;
                    retVal.second = minNodeScore;
                }
                state.undoMove(mv);
            }
        }
    }
    if (std::chrono::steady_clock::now() < timeLimit) {
        //If we're still within time record this move as the best for move ordering

        uint32_t moveRepresentation = uint32_t(retVal.first);
        moveOrdering[moveRepresentation] += pow(2,depthRemaining);
    }
    return retVal;
}


int Agent::getPlayerNumber() const {
    return myPlayerNumber;
}
//Generates the first 5 moves for both p1 and p2 from our currently WARNING: resets the internal state of hte agent.
void Agent::generateOpeningBook(int secondsPerMove) {
    state.reset();
    for (int i = 0;i < 10;++i) {
        Move best = nextMove(secondsPerMove * 1000);
        std::cout << "P" <<
        state.currentPlayer << ": " << best.from << " -> " << best.to << std::endl;
        std::cout << debugStream.str();
        debugStream.clear();
        debugStream.str("");
        state.applyMove(best);
    }
}
void Agent::setUCBDepth(int depth) {
    ucbDepth = depth;
    std::cerr << "Setting UCB Depth to " << depth << std::endl;
}
void Agent::setSearchType(std::string type) {
    if (type == "MCTS") {
        currentSearchType = MCTS;
        std::cerr << "Search mode set to: MCTS" << std::endl;
    }
    if (type == "MINIMAX") {
        currentSearchType = MINIMAX;
        std::cerr << "Search mode set to: MINIMAX" << std::endl;
    }
}
void Agent::setExploration(double exp) {
    exploration = exp;
    std::cerr << "Set exploraiton constant to: " << exp << std::endl;
}

double Agent::SelectLeaf(uint32_t node) {
    double payout = 0;
    int numThreads = std::thread::hardware_concurrency() + 1;
    if (IsLeaf(node)) {
        if (MCTree[node].samples > 4) {
            Expand(node);
        }
        
        uint32_t playOutNode = node;
        if (MCTree[node].numberChildren > 0) {
            playOutNode = SelectBestChild(node);
        }
        double results[numThreads];
        std::thread** threads = new std::thread*[numThreads];
//        void (Agent::*func)(uint32_t, int, double*);
//        func = &Agent::DoPlayOutThread;
        for (int i = 0;i < numThreads;++i) {
            threads[i] = new std::thread(&Agent::DoPlayOutThread, this, playOutNode, ucbDepth, &results[i]);
        }
        for (int i = 0;i < numThreads;++i) {
            threads[i]->join();
            delete threads[i];
            payout += results[i];
        }
        delete[] threads;
        
    } else {
        payout = SelectLeaf(SelectBestChild(node));
    }
    double multiplier = 1;
    if (!MCTree[node].maximizing) { multiplier = -1; }
    MCTree[node].payOff += payout * multiplier;
    MCTree[node].samples += numThreads;
    return payout;
}
uint32_t Agent::SelectBestChild(uint32_t node) {
    MCTSNode nodeEntry = MCTree[node];
    uint32_t returnNode = 0;
    double bestScore = 0;
    int minSamples = nodeEntry.samples >> 10; //EXPERIMENTAL: Make sure all child nodes have at least 0.1% of hte samples of the parent.
    if (minSamples < 1) { minSamples = 1; }
    for (int i = nodeEntry.indexFirstChild;i < nodeEntry.indexFirstChild + nodeEntry.numberChildren;++i) {
        //if (MCTree[i].samples == 0) { return i; }
        if (MCTree[i].samples < minSamples) { return i; }
        double score = GetUCBVal(i,node);
        if (score > bestScore || returnNode == 0) {
            bestScore = score;
            returnNode = i;
        }
    }
    return returnNode;
    
}// Use the UCB rule to find the best child
double Agent::GetUCBVal(uint32_t node, uint32_t parent) {
    double average = MCTree[node].payOff / MCTree[node].samples;
    double score = average + (exploration * sqrt(log(MCTree[parent].samples) / MCTree[node].samples));
    return score;
} // Get the UCB value of a given node
bool Agent::IsLeaf(uint32_t node) {
    return MCTree[node].numberChildren == 0;
} // is the designated node a leaf
void Agent::GetNodeState(uint32_t node, ChineseCheckersState& state) {
    uint32_t parent = node;
    std::vector<Move> moves;
    while (parent != 0) {
        moves.push_back(MCTree[parent].gotToHere);
        parent = MCTree[parent].parentIndex;
    }
    std::reverse(moves.begin(), moves.end());
    for (auto mv : moves) {
        state.applyMove(mv);
    }
}
void Agent::Expand(uint32_t node) {
    ChineseCheckersState s = state;
    GetNodeState(node, s);
    if (s.gameOver()) {
        return;
    }
    bool parentMaximizing = MCTree[node].maximizing;
    std::vector<Move> leafMoves;
    s.getMoves(leafMoves);
    MCTree[node].numberChildren = leafMoves.size();
    MCTree[node].indexFirstChild = MCTree.size();
    for (auto mv : leafMoves) {
        MCTSNode leafNode;
        leafNode.parentIndex = node;
        leafNode.gotToHere = mv;
        leafNode.samples = 0;
        leafNode.payOff = 0;
        leafNode.maximizing = !parentMaximizing;
        leafNode.numberChildren = 0;
        MCTree.push_back(leafNode);
    }
}; // expand the designated node and add its children to the tree

void Agent::DoPlayOutThread(uint32_t node, int depth, double* result) {    
    *result = DoPlayout(node, depth);
}

double Agent::DoPlayout(uint32_t node, int depth) {
    ChineseCheckersState s = state;
    GetNodeState(node, s);
    if (visitedStates.find(s.getHash()) != visitedStates.end()) {
        if (s.currentPlayer == myPlayerNumber) { //Our opponent moved to a visited state so it is a win for us
            if (depth == 0) { return 1; }
            return (*stateEval).getUpperBound();
        } else { // We moved into a visited state so it is a loss.
            if (depth == 0) { return 0; }
            return (*stateEval).getLowerBound();
        }
    }
    int depthRemaining = depth;
    if (depth == 0) { depthRemaining = 1; }
    while (!s.gameOver() && depthRemaining > 0) {
        if (depth > 0) { depthRemaining--; }
        std::vector<Move> moves;
        s.getMoves(moves);
        Move toApply = {0,0};
        double val = rand() % 100000;
        val /= 100000;
        if (val < 0.05) { // select random move Epsilon greedy
            toApply = s.getRandomForwardMove();
        } else {
            int greatestDistance = 0;
            for (auto &mv : moves) {
                int difference = abs(static_cast<int>(mv.to) - static_cast<int>(mv.from));
                int distance = (difference % 9) + (difference / 9);
                if (distance > greatestDistance) {
                    greatestDistance = distance;
                    toApply = mv;
                }
            }
        }
        s.applyMove(toApply);
    }
    int score = 0;
    if (depth > 0) {
        int winner = s.winner();
        if (winner > 0) {
            if (winner == myPlayerNumber) { return (*stateEval).getUpperBound(); }
            else { return (*stateEval).getLowerBound(); }
        }
        score = (*stateEval).evaluate(s, myPlayerNumber);
    } else {
        if (s.currentPlayer == state.currentPlayer) {
            score = 1;
        }
    }
    return score;
}// play out the game, returning the evaluation at the end of the game
double Agent::sampleRatio() { //Returns the ratio of hte smallest to largest sample sizes in the MCTree
    MCTSNode root = MCTree[0];
    double maxSamples = 0;
    double minSamples = -1;
    for (int i = root.indexFirstChild;i < root.indexFirstChild + root.numberChildren;++i) {
        MCTSNode child = MCTree[i];
        //std::cerr << " * " << child.samples << std::endl;
        if (child.samples > maxSamples) {
            maxSamples = child.samples;
        }
        if (minSamples == -1) {
            minSamples = child.samples;
        } else if (child.samples < minSamples) {
            minSamples = child.samples;
        }
    }
    //std::cerr << "Min: " << minSamples << " / " << maxSamples << std::endl;
    return minSamples / maxSamples;
}
void Agent::calculateExploration(double increment, int direction) { //Tries to find the best exploration constant. Increment is the amount it adds or subtracts.
    double currentRatio = 0;
    for (int i = 0;i < 100;i++) {
        MCTree.clear();
        MCTSNode root;
        root.parentIndex = 0;
        root.payOff = 0;
        root.samples = 0;
        root.maximizing = false;
        MCTree.push_back(root);
        Expand(0);
        while (MCTree[0].samples < 10000) {
            SelectLeaf(0);
        }
        currentRatio += sampleRatio();
    }
    double targetRatio = 0.01;
    currentRatio /= 100;
    if (fabs(currentRatio - targetRatio) < 0.0001) {
        std::cerr << "Set exploration to " << exploration << std::endl;
        return;
    }
    int newDirection = 0;
    if (targetRatio < currentRatio) { newDirection = -1; }
    if (targetRatio > currentRatio) { newDirection = 1; }
    if ((direction != 0 && newDirection != direction) || currentRatio < 0) {
        exploration += increment * -1 * newDirection;
        calculateExploration(increment * 0.5,newDirection);
    } else {
        exploration += increment * newDirection;
        std::cerr << "Set exploration to " << exploration << " (" << currentRatio << ")" << std::endl;
        calculateExploration(increment,newDirection);
    }

    
}
Move Agent::nextMove(int milliseconds) { //Advances pieces based on distance to goal.
    std::chrono::high_resolution_clock::time_point move_time = std::chrono::high_resolution_clock::now(); //Move_time is when we have received our turn
    std::chrono::steady_clock::time_point timeLimit = move_time + std::chrono::milliseconds(milliseconds);
    debugStream << "  *****  " << std::endl;
    if (debugFlag) { timeLimit = move_time + std::chrono::seconds(30); }
    if (currentSearchType == MINIMAX) {
            Move currentMove = {0, 0};
            Move lastMove = {0, 0};
            std::pair<Move,double> moveScore;
            moveScore.second = 0;
            int i = 0;
            do {
                ++i;
                lastMove = currentMove;
                if (moveScore.second >= stateEval->getUpperBound()) { break; } // we won!
                moveScore = firstNode(state,i,timeLimit,stateEval->getLowerBound(),stateEval->getUpperBound(),*stateEval);
                currentMove = moveScore.first;
                if (std::chrono::steady_clock::now() < timeLimit) {
                    debugStream.clear();
                    debugStream.str("");
                    debugStream << "Depth " << i << " Move From: " << currentMove.from << " to " << currentMove.to << " took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - move_time).count() << "s" << std::endl;
                    
                }
                
            } while (std::chrono::steady_clock::now() < timeLimit);
            return lastMove;
        
    } else if (currentSearchType == MCTS) {
        MCTree.clear();
        MCTSNode root;
        root.parentIndex = 0;
        root.payOff = 0;
        root.samples = 0;
        root.maximizing = false;
        MCTree.push_back(root);
        Expand(0);
        
        while (std::chrono::steady_clock::now() < timeLimit) {
            SelectLeaf(0);
        }
        root = MCTree[0];
        debugStream << "Tree size: " << MCTree.size() << std::endl;
        debugStream << "Root samples: " << MCTree[0].samples << std::endl;
        //    std::cerr << MCTree[0].numberChildren << std::endl;
        Move bestMove = Move{0,0};
        double bestScore = 0;
        for (int i = root.indexFirstChild;i < root.indexFirstChild + root.numberChildren;++i) {
            MCTSNode child = MCTree[i];
            
            double score = child.payOff / child.samples;
            debugStream << child.gotToHere.from << " to " << child.gotToHere.to << ": " << score << "(" << child.samples << ")" << std::endl;
            if (score > bestScore || bestMove == Move{0,0}) {
                bestScore = score;
                bestMove = child.gotToHere;
            }
        }
        
        return bestMove;
        
    }
    return Move{0,0};
}

void Agent::setEvaluator(StateEvaluator* stateEval) {
    this->stateEval = stateEval;
}

void Agent::playGame() {
  // Identify myself

  std::cout << "#name " << name << std::endl;
    while (false) {
        const Move m = nextMove();
        // Apply it locally
        state.applyMove(m);
        std::cerr << debugStream.str(); //Print our debug stream _after_ we submit the move so it's not counted against our time
        debugStream.clear();
        debugStream.str("");
    }
  // Wait for start of game
  waitForStart();

  // Main game loop
  for (;;) {
    if (current_player == my_player) {
      // My turn

      // Check if game is over
      if (state.gameOver()) {
        std::cerr << "I, " << name << ", have lost" << std::endl;
        switchCurrentPlayer();
        continue;
      }

      // Determine next move
      const Move m = nextMove();
      // Apply it locally
      state.applyMove(m);
        visitedStates.insert(state.getHash());
      // Tell the world
      printAndRecvEcho(m);

      // It is the opponents turn
      switchCurrentPlayer();
        std::cerr << debugStream.str(); //Print our debug stream _after_ we submit the move so it's not counted against our time
        debugStream.clear();
        debugStream.str("");
    } else {
      // Wait for move from other player
      // Get server's next instruction
      std::string server_msg = readMsg();
      const std::vector<std::string> tokens = tokenizeMsg(server_msg);

      if (tokens.size() == 5 && tokens[0] == "MOVE") {
        // Translate to local coordinates and update our local state
        const Move m = state.translateToLocal(tokens);
        state.applyMove(m);
          visitedStates.insert(state.getHash());
        // It is now my turn
        switchCurrentPlayer();
      } else if (tokens.size() == 4 && tokens[0] == "FINAL" &&
                 tokens[2] == "BEATS") {
        // Game over
        if (tokens[1] == name && tokens[3] == opp_name)
          std::cerr << "I, " << name << ", have won!" << std::endl;
        else if (tokens[3] == name && tokens[1] == opp_name)
          std::cerr << "I, " << name << ", have lost." << std::endl;
        else
          std::cerr << "Did not find expected players in FINAL command.\n"
                    << "Found '" << tokens[1] << "' and '" << tokens[3] << "'. "
                    << "Expected '" << name << "' and '" << opp_name << "'.\n"
                    << "Received message '" << server_msg << "'" << std::endl;
        break;
      } else {
        // Unknown command
        std::cerr << "Unknown command of '" << server_msg << "' from the server"
                  << std::endl;
      }
    }
  }
}

// Sends a msg to stdout and verifies that the next message to come in is it
// echoed back. This is how the server validates moves
void Agent::printAndRecvEcho(const std::string &msg) const {
  // Note the endl flushes the stream, which is necessary
  std::cout << msg << std::endl;
  const std::string echo_recv = readMsg();
  if (msg != echo_recv)
    std::cerr << "Expected echo of '" << msg << "'. Received '" << echo_recv
              << "'" << std::endl;
}

// Reads a line, up to a newline from the server
std::string Agent::readMsg() const {
  std::string msg;
  std::getline(std::cin, msg); // This is a blocking read

  // Trim white space from beginning of string
  const char *WhiteSpace = " \t\n\r\f\v";
  msg.erase(0, msg.find_first_not_of(WhiteSpace));
  // Trim white space from end of string
  msg.erase(msg.find_last_not_of(WhiteSpace) + 1);

  return msg;
}

// Tokenizes a message based upon whitespace
std::vector<std::string> Agent::tokenizeMsg(const std::string &msg) const {
  // Tokenize using whitespace as a delimiter
  std::stringstream ss(msg);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  std::vector<std::string> tokens(begin, end);

  return tokens;
}

void Agent::waitForStart() {
  for (;;) {
    std::string response = readMsg();
    const std::vector<std::string> tokens = tokenizeMsg(response);

    if (tokens.size() == 4 && tokens[0] == "BEGIN" &&
        tokens[1] == "CHINESECHECKERS") {
      // Found BEGIN GAME message, determine if we play first
      if (tokens[2] == name) {
        // We go first!
        opp_name = tokens[3];
        my_player = player1;
          myPlayerNumber = 1;
          std::cerr << "Player number set to " << myPlayerNumber << std::endl;
        break;
      } else if (tokens[3] == name) {
        // They go first
        opp_name = tokens[2];
        my_player = player2;
          myPlayerNumber = 2;
          std::cerr << "Player number set to " << myPlayerNumber << std::endl;
        break;
      } else {
        std::cerr << "Did not find '" << name
                  << "', my name, in the BEGIN command.\n"
                  << "# Found '" << tokens[2] << "' and '" << tokens[3] << "'"
                  << " as player names. Received message '" << response << "'"
                  << std::endl;
        std::cout << "#quit" << std::endl;
        std::exit(EXIT_FAILURE);
      }
    } else if (response == "DUMPSTATE") {
      std::cout << state.dumpState() << std::endl;
    } else if (tokens[0] == "LOADSTATE") {
      std::string new_state = response.substr(10);
      if (!state.loadState(new_state))
        std::cerr << "Failed to load '" << new_state << "'\n";
    } else if (response == "LISTMOVES") {
      std::vector<Move> moves;
      state.getMoves(moves);
      for (const auto i : moves)
        std::cout << i.from << ", " << i.to << "; ";
      std::cout << std::endl;
    } else if (tokens[0] == "MOVE") {
      // Just apply the move
      const Move m = state.translateToLocal(tokens);
      if (!state.applyMove(m))
        std::cout << "Unable to apply move " << m << std::endl;
    } else {
      std::cerr << "Unexpected message " << response << "\n";
    }
  }

  // Game is about to begin, restore to start state in case DUMPSTATE/LOADSTATE/LISTMOVES
  // were used
  state.reset();

  // Player 1 goes first
  current_player = player1;
}

void Agent::switchCurrentPlayer() {
  current_player = (current_player == player1) ? player2 : player1;
}

bool Agent::isValidStartGameMessage(const std::vector<std::string> &tokens) const {
  return tokens.size() == 4 && tokens[0] == "BEGIN" && tokens[1] == "CHINESECHECKERS";
}

bool Agent::isValidMoveMessage(const std::vector<std::string> &tokens) const {
  return tokens.size() == 5 && tokens[0] == "MOVE" && tokens[1] == "FROM" &&
    tokens[3] == "TO";
}
