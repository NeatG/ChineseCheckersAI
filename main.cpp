// Group 2:
// Mohammed Bellifa and Stephen Fleming
//
// debugFlag global variable is below. Setting this to true verifies the alpha-beta utilizing minimax against the transposition table and it verifies the alpha beta search against a regular minimax search.
#include <cstddef>

#include "Agent.h"
#include "ChineseCheckersState.h"
#include <chrono>
#include <iostream>
#include <sstream>
#include "DistanceStateEvaluator.h"
#include "TDLambdaStateEvaluator.h"
#include "ZobristHash.h"
#include "TranspositionTable.h"

bool debugFlag = false; //Global debug flag, turning this on will cause the agent to no longer work within 10 seconds and will cause it to double check values from multiple functions (slow).

//This stringstream is used to print debug information _after_ the move timer has completed so that long debug messages don't disadvantage the agent.
std::stringstream debugStream;

int main(int argc, char ** argv) {
    ZobristHashSetup();
    TranspositionTableSetup();
/*  Debugs the ZHash implementation by testing the incremental version vs. creating the whole hash at once and printing out hte results.
    DebugZHash();
 */
    Agent a;
    DistanceStateEvaluator distanceStateEval;
    TDLambdaStateEvaluator tdlambda;
    //100k trains
    std::vector<double> weights = {-0.307312, -0.339539, -0.251665, -0.246691, -0.250036, -0.135097, -0.0990512, -0.0858252, -0.131595, -0.218764, -0.201225, -0.314214, -0.24741, -0.21092, -0.165623, -0.0823553, -0.102514, -0.30157, -0.331962, -0.117086, -0.108304, -0.0137429, -0.0850764, -0.118107, -0.0692807, -0.0670176, -0.110714, -0.134626, -0.177352, -0.179839, -0.000430668, 0.128354, -0.0151272, 0.209483, 0.000325294, -0.140233, -0.130799, -0.238712, -0.114207, -0.126015, 0.0276815, 0.209766, 0.0936781, -0.113975, 0.0863978, -0.0966484, -0.0365608, -0.051896, -0.0202094, 0.0709003, -0.0128015, -0.0310936, -0.0132754, -0.0246654, 0.0563962, -0.094141, -0.0873183, -0.0152764, -0.0390265, 0.140753, 0.088226, 0.214345, 0.176518, -0.260114, -0.107073, 0.111228, 0.213045, 0.123676, 0.190296, 0.166669, 0.291352, 0.192827, -0.116173, 0.126329, 0.0325726, 0.0752927, 0.117878, 0.0304552, 0.117813, 0.317331, 0.330105, -0.31682, -0.257699, -0.175377, -0.193978, -0.258031, -0.287641, -0.227428, -0.17926, -0.0455592, -0.204188, -0.247275, -0.228036, -0.213404, -0.238054, -0.297307, -0.139975, 0.00169132, 0.164315, -0.19087, -0.183224, -0.134154, -0.0941036, -0.0543402, -0.131445, -0.00915771, -0.0299467, -0.250968, -0.0102699, -0.0389637, -0.107051, -0.0381127, -0.132504, -0.0954712, -0.0500832, -0.0583592, -0.0297679, -0.133018, -0.0737002, -0.0860168, -0.103919, -0.0735202, -0.0349269, -0.00116318, 0.0777224, -0.0142916, -0.0250995, -0.18501, -0.229796, 0.00739241, -0.156338, -0.0343058, 0.066664, 0.0259689, 0.196007, -0.031733, -0.0832626, -0.0410458, 0.0708851, 0.038052, -0.00951905, 0.0522875, 0.076652, 0.454161, 0.0333682, -0.0337209, 0.0553117, 0.0712813, 0.0459244, 0.0418212, 0.139175, 0.198034, 0.149187, 0.0161972, 0.00322443, -0.0137535, 0.142816, 0.118163, 0.20507, 0.221011, 0.272536, 0.417454, 0.0136574, -0.00480642, -0.0388088, 0.0690207, 0, 0.0267082, 0, -0.019877, 0.0804086, -0.0975693, 0.131852, -0.0933368, -0.0112514, 0.0472635, -0.106466, 0.0371236, -0.0654561, 0.0616517, -0.044166, -0.111223, 0, -0.00465327, 0, -0.0410057, 0, -0.0442675, 0.0022948, -0.0616278, 0.125725, -0.11433, 0.0950133, -0.0685656, 0.120454, -0.127299, 0.0954946, -0.0523553, -0.0522871, -0.0787459, -0.0156801, 0.0731118, -0.0551317, -0.0890714, 0.0630019, -0.0940528, -0.0150442, 0.0330585, -0.0380813, 0.205182, -0.0390366, 0.135589, -0.0184963, -0.0784696, -0.0429355, -0.0711114, 0.0470768, -0.0697026, -0.0193403, -0.170774, -0.0421136, -0.0907229, -0.00142528, 0.0276121, 0.0773191, -0.0581336, 0.0755487, 0.0960355, 0.0418401, 0.0749683, -0.0511002, 0.181255, 0.00518918, 0.0831398, 0.0572717, 0.172451, 0.00569706, -0.032718, 0.0448265, -0.01899, -0.0951905, -0.0361693, 0.0285242, 0.10245, 0.0336272, 0.000979215, 0.00470545, 0.0283849, 0.163643, -0.0550888, 0.0999119, -0.0148756, -0.078249, -0.0454923, -0.00564781, -0.0482844, -0.0355662, 0.0521989, -0.0170859, -0.0570891, -0.162989, 0.0265335, 0.015598, 0.0476615, -0.0447007, 0.00124038, 0.123122, 0.058648, 0.0665888, 0.139451, 0.0350272, 0.0764345, 0.121428, -0.0502383, 0.104392, -0.0490058, 0.0658517, -0.0337324, 0.0972609, -0.0335271, -0.00414324, 0.0721782, -0.0733308, -0.0105934, -0.000802593, -0.156304, 0.00830996, 0.0143513, 0.022552, -0.0604361, -0.0789304, -0.0475662, -0.0519334, 0.0247742, -0.133525, 0.0259198, -0.0582407, 0.0653129, -0.0120914, -0.0744911, -0.00788297, -0.0357588, -0.014324, -0.0480236, 0.00838003, -0.017449, -0.0084928, -0.00859793, -0.0187887, 0.000143227, -0.0383037, -0.11407, -0.0129775, 0.0153119, 0.018515, -0.0327658, -0.130841, 0.105574, -0.0351979, -0.0173536, -0.0820133, -0.005073, -0.00658115, 0.0848818, -0.159796, 0.0940308, -0.1107, -0.0243591, -0.0686062, 0.0167907, -0.0391212, 0.0504144, -0.00933294, -0.0334685, 0.0840566, -0.0480006, 0.0645685, 0.0521992, -0.0547975, 0.00692723, -0.0879504, 0.0659128, -0.065579, 0.090162, -0.0372991, 0.00236419, -0.0553924, 0.0298406, -0.0753056, 0.0551612, 0.00899083, 0.095583, 0.0553231, -0.0260103, -0.0753491, 0.067999, 0.0627634, -0.0944945, -0.016428, -0.0853318, 0.0076325, -0.061828, -0.0291326, -0.0416199, 0.0536789, 0.0545289, -0.0416361, -0.0284206, 0.0461199, -0.0200101, -0.0968656, -0.013937, 0.0276713, 0.0340135, -0.0262345, -0.0122204, -0.0453798, 0.0201679, 0.0134225, -0.0479226, 0.098477, -0.0827848, 0.0295672, -0.038512, -0.0549406, -0.0570912, 0.00117258, 0.0458194, 0.064772, 0.032736, 0.0879961, -0.143343, -0.0413863, -0.00623079, -0.0208585, 0.0208549, -0.0484931, -0.0497728, 0.033489, 0.0631531, -0.132639, -0.00699647, 0.000194926, -0.0179588, -0.020438, -0.0151278, -0.0453281, -0.0414926, 0.0134023, -0.0642796, 0.0133997, -0.0339204, 0.0830146, -0.0150866, 0.0502828, -0.0154588, 0.0526846, -0.0164949, -0.0457501, 0.00220467, -0.0294932, -0.0525256, -0.111636, -0.010825, -0.10419, -0.0732823, -0.0145841, -0.00193477, -0.025885, -0.0174265, 0.0074644, -0.0291568, -0.0242788, -0.0169375, -0.00940051, -0.142512, -0.00128107, 0.0239121, 0.0174895, -0.0349941, 0.045259, 0.122356, 0.0288982, -0.0751834, 0.00868598, -0.0200965, -0.0891347, 0.0441362, -0.021247, 0.0485725, -0.0914174, 0.00167017, -0.0293001, -0.0753028, -0.0685989, 0.122809, -0.00613777, 0.0911504, -0.0612803, 0.000251943, -0.00689913, -0.129751, -0.00026794, -0.0824694, 0.0195203, -0.10425, 0.0515087, -0.176154, 0.0959838, -0.136413, 0.0725585, 0.0180578, 0.00620325, 0.0498458, 0.0770643, 0.072464, 0.352128, -0.033051, 0.0257144, 0.127698, -0.064539, 0.00924901, -0.119315, 0.0165828, -0.276684, -0.00603578, -0.0997844, 0.0565853, -0.0287549, 0.0526295, -0.0354316, 0.0720417, 0.218457, 0.0258621, -0.0462868, 0.005526, 0.0848274, -0.000959499, 0.0286217, -0.0458084, -0.1003, -0.112758, -0.00574025, 0.0690669, -0.132724, -0.0966278, -0.0953216, -0.0777901, -0.152025, 0.0369261, 0.0120302, -0.0308322, -0.0086464, 0, -0.0200253, -0.0339652, 0.20782, 0.149128, 0.0475037, -0.023637, 0.0700339, -0.0114372, -0.0762299, 0, -0.046445, 0, -0.0211871, 0.027737, 0.0649743, 0.0483186, 0.169965, -0.132036, 0.0762841, 0.188944, 0.0107483, 0.016851, -0.00514356, -0.0170721, 0.00437153, 0.0927282, 0.0525952, 0.117036, -0.0610809, -0.0833585, -0.0623507, 0, -0.0523989, 0, 0.0296577, -0.0716421, -0.0477695, 0.111966, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.315059};
    //tdlambda.train(100000);
    bool calculatingExploration = false;
    if (argc > 1) {
        for (int i = 1;i < argc;++i) {
            std::string curArg = argv[i];
            if (curArg == "-name") {
                if (i > argc) {
                    std::cerr << "Invalid argument for -name." << std::endl;
                }
                ++i;
                a.setName(argv[i]);
                continue;
            } else if (curArg == "-ucbdepth") {
                if (i > argc) {
                    std::cerr << "Invalid argument for -ucbdepth." << std::endl;
                }
                ++i;
                a.setUCBDepth(atoi(argv[i]));
                continue;
            } else if (curArg == "-searchtype") {
                if (i > argc) {
                    std::cerr << "Invalid argument for -searchtype." << std::endl;
                }
                ++i;
                std::string type = argv[i];
                if (type != "MCTS" && type != "MINIMAX") {
                    std::cerr << "Invalid argument for -searchtype, expected MINIMAX or MCTS" << std::endl;
                }
                a.setSearchType(type);
                continue;
            } else if (curArg == "-exploration") {
                if (i > argc) {
                    std::cerr << "Invalid argument for -exploration." << std::endl;
                }
                ++i;
                a.setExploration(atof(argv[i]));
                continue;
            } else if (curArg == "-calculate-exploration") {
                calculatingExploration = true;
                continue;
            }
        }
    }
    tdlambda.setWeights(weights);
    //a.setEvaluator(&distanceStateEval);
    a.setEvaluator(&tdlambda);
    //a.generateOpeningBook(300);
    if (calculatingExploration) {
        a.calculateExploration(0.25,0);
        return EXIT_SUCCESS;
    }
    a.playGame();
    return EXIT_SUCCESS;
}
