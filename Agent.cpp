#include "Agent.h"

#include <cstdlib>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

#include <algorithm>
#include "TranspositionTable.h"

Agent::Agent() : name("BestAI20150411") {
}
double Agent::minNode(ChineseCheckersState& state, int depthRemaining, std::chrono::steady_clock::time_point timeLimit, double alpha, double beta, StateEvaluator& stateEval) {
    int stateWinner = state.winner();
    if (stateWinner != -1) {
        if (stateWinner == state.getCurrentPlayer()) { return stateEval.getLowerBound(); }
        else { return stateEval.getUpperBound(); }
    }
    if (std::chrono::steady_clock::now() > timeLimit) {
        return 0; //It doesn't matter what we return because it will all be discarded if we've run out of time.
    }
    if (depthRemaining == 0) {
        return stateEval.evaluate(state, 3 - state.getCurrentPlayer());
    }
    std::vector<Move> moves;
    state.getMoves(moves);
    for (auto mv : moves) {
        state.applyMove(mv);
        beta = std::min(beta,maxNode(state,depthRemaining - 1,timeLimit,alpha,beta,stateEval));
        state.undoMove(mv);
        if (alpha >= beta) { return beta; }
    }
    return beta;
    
}
double Agent::maxNode(ChineseCheckersState& state, int depthRemaining, std::chrono::steady_clock::time_point timeLimit, double alpha, double beta, StateEvaluator& stateEval) {
    int stateWinner = state.winner();
    if (stateWinner != -1) {
        if (stateWinner == state.getCurrentPlayer()) { return stateEval.getUpperBound(); }
        else { return stateEval.getLowerBound(); }
    }
    if (std::chrono::steady_clock::now() > timeLimit) {
        return 0; //Doesn't matter what we return because the results will be discarded
    }
    if (depthRemaining == 0) {
        return stateEval.evaluate(state, state.getCurrentPlayer());
    }
    std::vector<Move> moves;
    state.getMoves(moves);
    for (auto mv : moves) {
        state.applyMove(mv);
        alpha = std::max(alpha,minNode(state,depthRemaining - 1,timeLimit,alpha,beta,stateEval));
        state.undoMove(mv);
        if (alpha >= beta) { return alpha; }
    }
    return alpha;
    
}
double Agent::miniMax(ChineseCheckersState& state, int depthRemaining, std::chrono::steady_clock::time_point timeLimit, double alpha, double beta, StateEvaluator& stateEval, bool maximizing) {
    int stateWinner = state.winner();
    if (stateWinner != -1) {
        if (stateWinner == myPlayerNumber) { return stateEval.getUpperBound() + depthRemaining; } //Adding the depth remaining is a hack to try to get the agent to finish the game if it sees a winning state
        else { return stateEval.getLowerBound(); }
    }
    if (std::chrono::steady_clock::now() > timeLimit) {
        return 0; //Doesn't matter what we return because the results will be discarded
    }
    if (depthRemaining == 0) {
        return stateEval.evaluate(state, myPlayerNumber);
    }
    //Check the cache.
    uint64_t hash = state.getHash();
    TranspositionTableEntry* cacheEntry = &TranspositionTable[hash % TranspositionTableCacheSize];
    if (cacheEntry->hash == hash && cacheEntry->depthRemaining >= depthRemaining && cacheEntry->value > alpha && cacheEntry->value < beta) {
     //   debug << "Used a transposition table!" << std::endl;
        return cacheEntry->value;
    }
    
    std::vector<Move> moves;
    state.getMoves(moves);
    for (auto mv : moves) {
        state.applyMove(mv);
        double temp = miniMax(state,depthRemaining - 1,timeLimit,alpha,beta,stateEval,!maximizing);
        if (maximizing) {
            alpha = std::max(alpha,temp);
        } else {
            beta = std::min(beta,temp);
        }
        
        state.undoMove(mv);
        if (alpha >= beta) { break; }
    }
    cacheEntry->hash = hash;
    cacheEntry->alpha = alpha;
    cacheEntry->beta = beta;
    cacheEntry->depthRemaining = depthRemaining;
    
    if (maximizing) {
        cacheEntry->value = alpha;
        return alpha;
    }
    cacheEntry->value = beta;
    return beta;
}
//Implemented pseudocode from Wikipedia for Negascout
double Agent::pvs(ChineseCheckersState& state, int depthRemaining, std::chrono::steady_clock::time_point timeLimit, double alpha, double beta, StateEvaluator& stateEval, int color) {
    int stateWinner = state.winner();
    if (stateWinner != -1) {
        if (stateWinner == state.getCurrentPlayer() && color == -1) { return stateEval.getUpperBound(); }
        else { return stateEval.getLowerBound(); }
    }
    if (std::chrono::steady_clock::now() > timeLimit) {
        return 0; //Doesn't matter what we return because the results will be discarded
    }
    if (depthRemaining == 0) {

        return color * stateEval.evaluate(state, myPlayerNumber);
        
    }
    std::vector<Move> moves;
    state.getMoves(moves);
    size_t moveSize = moves.size();
    double score;
    for (int i = 0;i < moveSize;++i) {
        state.applyMove(moves[i]);
        if (i == 0) { //First child
            score = -pvs(state,depthRemaining - 1, timeLimit, -beta, -alpha, stateEval, -color);
        } else {
            score = -pvs(state,depthRemaining - 1, timeLimit, -alpha-1, -alpha, stateEval, -color);
            if (score > alpha && score < beta) {
                score = score = -pvs(state,depthRemaining - 1, timeLimit, -beta, -score, stateEval, -color);
            }
        }
        alpha = std::max(alpha,score);
        state.undoMove(moves[i]);
        if (alpha >= beta) { break; }
    }
    return alpha;
}

Move Agent::firstNode(ChineseCheckersState& state, int depthRemaining, std::chrono::steady_clock::time_point timeLimit, double alpha, double beta, StateEvaluator& stateEval) {
    std::vector<Move> moves;
    std::vector<Move> bestMoves;
    state.getMoves(moves);
    double bestScore = alpha - 1;
    //std::cerr << "Best Score: " << bestScore << std::endl;
    state.getMoves(moves);
    for (auto mv : moves) {
        state.applyMove(mv);
        //std::cerr << "Move FROM " << moves[i].from << " TO " <<moves[i].to << " : " << minNode(state,depthRemaining - 1,startTime,timeLimit,alpha,beta,stateEval) << std::endl;
        //std::cerr << "Alpha: " << alpha << " BETA: " << beta << std::endl;
        //double minNodeScore = minNode(state,depthRemaining - 1,timeLimit,alpha,beta,stateEval);
        double minNodeScore = miniMax(state,depthRemaining - 1,timeLimit,alpha,beta,stateEval,false);
        //alpha = std::max(alpha,minNodeScore);
        state.undoMove(mv);
        if (minNodeScore > bestScore) {
            bestScore = minNodeScore;
            bestMoves.clear();
        }
        if (minNodeScore == bestScore) {
            bestMoves.push_back(mv);
        }
        debug << "Depth " << depthRemaining << " Move from " << mv.from << " to " << mv.to << " : " << minNodeScore << std::endl;
        
        //std::cerr << "Alpha: " << alpha << " BETA: " << beta << std::endl;
        if (alpha >= beta) { return *select_randomly(bestMoves.begin(), bestMoves.end()); }
    }
    
    return *select_randomly(bestMoves.begin(), bestMoves.end());
}


int Agent::getPlayerNumber() const {
    return myPlayerNumber;
}

Move Agent::nextMove() { //Advances pieces based on distance to goal.
    std::chrono::high_resolution_clock::time_point move_time = std::chrono::high_resolution_clock::now(); //Move_time is when we have received our turn
    std::chrono::steady_clock::time_point timeLimit = move_time + std::chrono::milliseconds(9950);
    Move currentMove = {0, 0};
    Move lastMove = {0, 0};
    int i = 0;
    do {
        ++i;
        lastMove = currentMove;
        currentMove = firstNode(state,i,timeLimit,stateEval->getLowerBound(),stateEval->getUpperBound(),*stateEval);
        debug << "Depth " << i << " Move From: " << currentMove.from << " to " << currentMove.to << std::endl;
    } while (std::chrono::steady_clock::now() < timeLimit);
    return lastMove;
}

void Agent::setEvaluator(StateEvaluator* stateEval) {
    this->stateEval = stateEval;
}

void Agent::playGame() {
  // Identify myself
    
  std::cout << "#name " << name << std::endl;

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

      // Tell the world
      printAndRecvEcho(m);

      // It is the opponents turn
      switchCurrentPlayer();
        std::cerr << debug.str();
        debug.clear();
        debug.str("");
    } else {
      // Wait for move from other player
      // Get server's next instruction
      std::string server_msg = readMsg();
      const std::vector<std::string> tokens = tokenizeMsg(server_msg);

      if (tokens.size() == 5 && tokens[0] == "MOVE") {
        // Translate to local coordinates and update our local state
        const Move m = state.translateToLocal(tokens);
        state.applyMove(m);

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
