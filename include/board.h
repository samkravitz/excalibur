/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: board.h
 * DATE: January 31st, 2022
 * DESCRIPTION: A board representation contains bitboards of each piece type of each color.
 * 
 * The board class contains an array of 6 bitboards, piece_bb, one for each type of piece independent of color.
 * It also holds 2 more bitboards, color_bb, to hold all the pieces of a certain color.
 * 
 * This dense approach allows us to use less space by being able to make quick queries of pieces.
 */

#pragma once

#include <string>

#include "bitboard.h"
class Board
{
public:
    Board();

    // get all pieces on the board
    inline Bitboard pieces()                      const { return color_bb[WHITE] | color_bb[BLACK]; }
    // get all pieces of a certain color
    inline Bitboard pieces(Color c)               const { return color_bb[c]; }
    // get all pieces of a certain type
    inline Bitboard pieces(PieceType pt)          const { return piece_bb[pt]; }
    // get all pieces of a certain color and type
    inline Bitboard pieces(PieceType pt, Color c) const { return piece_bb[pt] & color_bb[c]; }

    std::string to_string() const;
private:
    Bitboard piece_bb[6];
    Bitboard color_bb[2];

    void reset();
};
