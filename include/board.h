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
#include "move.h"
#include "types.h"
class Board
{
public:
    Board();

    void clear();
    void set_piece(PieceType, Square, Color);
    void set_to_move(Color);

    // get all pieces on the board
    inline u64 pieces()                      const { return color_bb[WHITE] | color_bb[BLACK]; }
    // get all pieces of a certain color
    inline u64 pieces(Color c)               const { return color_bb[c]; }
    // get all pieces of a certain type
    inline u64 pieces(PieceType pt)          const { return piece_bb[pt]; }
    // get all pieces of a certain color and type
    inline u64 pieces(PieceType pt, Color c) const { return piece_bb[pt] & color_bb[c]; }

    inline bool get_castle_rights(Color c, CastleTypes ct) const { return castle_rights[c][ct]; }
    inline void set_castle_rights(Color c, CastleTypes ct) { castle_rights[c][ct] = true; }

    inline Square king_square(Color c) const
    {
        u64 king = piece_bb[KING] & color_bb[c];
        return bitscan(king);
    }

    inline Color mover() const { return to_move; }
    inline PieceType piece_on(Square square) const { return board[square]; }

    void make_move(Move const &);
    void undo_move(Move const &);

    std::string to_string() const;
    void print() const;
private:
    u64 piece_bb[6];
    u64 color_bb[2];
    bool castle_rights[2][2];
    PieceType board[64];

    // type of the most recently captured piece (used for undo move)
    PieceType captured_piece;

    Color to_move;

    void reset();
};
