#include "Agent.h"

#include <cstdlib>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

#include <algorithm>
#include "DistanceStateEvaluator.h"

Agent::Agent() : name("BestAI") {
}
double minNode(ChineseCheckersState& state, int depthRemaining, std::chrono::steady_clock::time_point timeLimit, double alpha, double beta, StateEvaluator& stateEval) {
    int stateWinner = state.winner();
    if (stateWinner != -1) {
        if (stateWinner == state.GetCurrentPlayer()) { return stateEval.getLowerBound(); }
        else { return stateEval.getUpperBound(); }
    }
    if (std::chrono::steady_clock::now() > timeLimit) {
        return stateEval.getUpperBound();
    }
    if (depthRemaining == 0) {
        return stateEval.evaluate(state, 3 - state.GetCurrentPlayer());
    }
    std::set<Move> moves;
    state.getMoves(moves);
    for (auto mv : moves) {
        state.applyMove(mv);
        beta = std::min(beta,maxNode(state,depthRemaining - 1,timeLimit,alpha,beta,stateEval));
        state.undoMove(mv);
        if (alpha >= beta) { return beta; }
    }
    return beta;
    
}
double maxNode(ChineseCheckersState& state, int depthRemaining, std::chrono::steady_clock::time_point timeLimit, double alpha, double beta, StateEvaluator& stateEval) {
    int stateWinner = state.winner();
    if (stateWinner != -1) {
        if (stateWinner == state.GetCurrentPlayer()) { return stateEval.getUpperBound(); }
        else { return stateEval.getLowerBound(); }
    }
    if (std::chrono::steady_clock::now() > timeLimit) {
        return stateEval.getLowerBound();
    }
    if (depthRemaining == 0) {
        return stateEval.evaluate(state, state.GetCurrentPlayer());
    }
    std::set<Move> moves;
    state.getMoves(moves);
    for (auto mv : moves) {
        state.applyMove(mv);
        alpha = std::max(alpha,minNode(state,depthRemaining - 1,timeLimit,alpha,beta,stateEval));
        state.undoMove(mv);
        if (alpha >= beta) { return alpha; }
    }
    return alpha;
    
}
Move firstNode(ChineseCheckersState& state, int depthRemaining, std::chrono::steady_clock::time_point timeLimit, double alpha, double beta, StateEvaluator& stateEval) {
    std::set<Move> moves;
    Move bestMove;
    state.getMoves(moves);
    double bestScore = alpha - 1;
    //std::cerr << "Best Score: " << bestScore << std::endl;
    state.getMoves(moves);
    for (auto mv : moves) {
        state.applyMove(mv);
        //std::cerr << "Move FROM " << moves[i].from << " TO " <<moves[i].to << " : " << minNode(state,depthRemaining - 1,startTime,timeLimit,alpha,beta,stateEval) << std::endl;
        //std::cerr << "Alpha: " << alpha << " BETA: " << beta << std::endl;
        alpha = std::max(alpha,minNode(state,depthRemaining - 1,timeLimit,alpha,beta,stateEval));
        state.undoMove(mv);
        if (alpha >= bestScore) {
            
            //std::cerr << mv << " score: " << alpha << std::endl;
        }
        if (alpha > bestScore) {
            bestScore = alpha;
            bestMove = mv;
        }
        
        //std::cerr << "Alpha: " << alpha << " BETA: " << beta << std::endl;
        if (alpha >= beta) { return bestMove; }
    }
    //std::cerr << "Best Score: " << bestScore << std::endl;
    return bestMove;
}


Move Agent::nextMove() { //Advances pieces based on distance to goal.
    std::chrono::high_resolution_clock::time_point move_time = std::chrono::high_resolution_clock::now(); //Move_time is when we have received our turn
    std::chrono::steady_clock::time_point timeLimit = move_time + std::chrono::milliseconds(9950);
    
    DistanceStateEvaluator stateEval;
    std::chrono::duration<double> timeSinceTurn;
    Move currentMove;
    Move lastMove;
    int i = 0;
    std::stringstream stmt;
    do {
        ++i;
        
        
        lastMove = currentMove;
        currentMove = firstNode(state,i,timeLimit,stateEval.getLowerBound(),stateEval.getUpperBound(),stateEval);

        
        /*std::chrono::high_resolution_clock::time_point current_time = std::chrono::high_resolution_clock::now();
        */
        timeSinceTurn = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - move_time);
        if (std::chrono::steady_clock::now() < timeLimit) {
            stmt.clear();
            stmt.str("");
            stmt << "Depth " << i << " Move From: " << currentMove.from << " to " << currentMove.to << " Completed in " << timeSinceTurn.count() << std::endl;
        }
    } while (std::chrono::steady_clock::now() < timeLimit);
    std::cout << stmt.str();
    timeSinceTurn = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - move_time);
    std::cerr << timeSinceTurn.count();
    
    return lastMove;
}

void Agent::playGame() {
  // Identify myself
  std::cout << "#name " << name << std::endl;
  while (true) {
        const Move m = nextMove();
      std::cout << "Move from " << m.from << " to " << m.to << std::endl;
      state.applyMove(m);
      
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

      // Tell the world
      printAndRecvEcho(m);

      // It is the opponents turn
      switchCurrentPlayer();
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
        break;
      } else if (tokens[3] == name) {
        // They go first
        opp_name = tokens[2];
        my_player = player2;
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
      std::set<Move> moves;
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
