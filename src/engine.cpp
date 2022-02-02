/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: engine.cpp
 * DATE: February 2nd, 2022
 * DESCRIPTION: Search & evaluation functionality
 */

#include "engine.h"

#include <sstream>

#include "move.h"
#include "util.h"

void Engine::parse_uci_moves(std::string const &moves)
{
    std::stringstream ss(moves);
    std::string move = "";
    while (std::getline(ss, move, ' '))
        parse_uci_move(move);
}

void Engine::parse_uci_move(std::string const &move)
{
    Square from = Util::from_algebraic(move.substr(0, 2));
    Square to = Util::from_algebraic(move.substr(2, 2));

    // check if move is capture
    bool is_capture = board.piece_on(to) != NONE;

    MoveFlags flags = QUIET_MOVE;
    
    if (is_capture)
        flags = CAPTURE;

    Move mv(from, to, flags);
    board.make_move(mv);
}