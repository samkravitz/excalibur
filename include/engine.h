/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: engine.h
 * DATE: February 2nd, 2022
 * DESCRIPTION: Search & evaluation functionality
 */

#pragma once

#include <string>
#include <tuple>

#include "board.h"

class Engine
{
public:
    Board board;

    float evaluate();
    std::tuple<Move, float> alphabeta(int);
    std::tuple<Move, float> negamax(int depth);
    std::tuple<Move, float> best_move();

    void parse_uci_moves(std::string const &);
    void parse_uci_move(std::string const &);

    void load_fen(std::string const &);
};