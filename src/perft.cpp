
/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: perft.cpp
 * DATE: February 4th, 2022
 * DESCRIPTION: performance testing for move generation
 * 
 * https://www.chessprogramming.org/Perft
 * https://www.chessprogramming.org/Perft_Results
 */

#include "perft.h"

#include <functional>

#include "engine.h"
#include "movegen.h"

/**
 * @brief test accuracy of move generation
 * @param depth number of ply to search
 * @param fen optional fen string of initial position to begin search
 * @return total number of nodes traversed
 */
int perft(int depth, std::string fen)
{
    Engine engine;

    if (fen != "")
        engine.load_fen(fen);
    
    std::function<int(int)> helper = [&](int depth) -> int
    {
        int nodes = 0;
        auto legal_moves = generate_moves(engine.board);

        if (depth == 1)
            return legal_moves.size();

        for (auto mv : legal_moves)
        {
            engine.board.make_move(mv);
            nodes += helper(depth - 1);
            engine.board.undo_move(mv);
        }
        return nodes;
    };

    return helper(depth);
}
