# ChineseCheckersAI
Chinese Checkers State management and AI.


Command Line Args:

-name <name> Sets the name of the agent, useful for games against itself.
-ucbdepth <depth> Sets the depth to cut off UCB sampling and perform a state evaluation function. If the depth is set to 0 it samples to the end of the game returning 0 for a loss or 1 for a win instead.

UCB Notes:
To compare the agent to itself and test UCB full random vs. cut off one could create a game with these two agents (assuming it's compiled to ChineseCheckersAI)

Random sampling to the end:
ChineseCheckersAI -name BestAIUCBRandom -ucbdepth 0

Random sampling to a depth + state evaluation:
ChineseCheckersAI -name BestAIUCBDepth -ucbdepth 20