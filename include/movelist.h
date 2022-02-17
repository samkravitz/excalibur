/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: movelist.h
 * DATE: February 16th, 2022
 * DESCRIPTION: wrapper class for holding all the possible moves in a position
 */
#pragma once

#include <array>
#include <cstddef>

#include "move.h"

// global board object
class Board;
extern Board board;

constexpr std::size_t N_MOVES = 256;

class Movelist
{
public:
    Movelist();

    void add(Move);
    inline std::size_t size() const { return m_size; }
    inline std::array<Move, N_MOVES>::const_iterator begin() const { return movelist.begin(); }
    inline std::array<Move, N_MOVES>::const_iterator end()   const { return movelist.begin() + m_size; }

private:
    std::array<Move, N_MOVES> movelist;
    std::size_t m_size;
};
