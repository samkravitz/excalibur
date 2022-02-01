#include "movegen.h"

#include <iostream>

#include "bitset_iter.h"
#include "constants.h"

std::vector<Move> pseudolegal(Board const &board, Color c)
{
    std::vector<Move> moves;

    // get all pieces of our color
    auto pieces = board.pieces(c);

    //auto pawns = board.pieces(PAWN, c);
    //pawns.iterate([&](int from)
    //{
    //    auto moves_bb = Bitboard(Constants::pawn_push_table[c][from]);
    //    moves_bb.iterate([&](int to)
    //    {
    //        moves.push_back(Move(from, to));
    //    });
    //});

    auto knights = board.pieces(KNIGHT, c);

    for (auto const &from : bitset::indices_on(std::bitset<64>{knights}))
    {
        auto moves_bb = Constants::knight_move_table[from];
        for (auto const &to : bitset::indices_on(std::bitset<64>{moves_bb}))
        {
            if ((pieces & to) == 0)
                moves.push_back(Move(from, to));
        }
    }

    
    //knights.iterate([&](int from)
    //{
    //    auto moves_bb = Bitboard(Constants::knight_move_table[from]);
    //    moves_bb.iterate([&](int to)
    //    {
    //        auto intersection = moves_bb & to;
    //        // discount moves that we have a piece on that square
    //        if (intersection.empty())
    //            moves.push_back(Move(from, to));
    //    });
    //});

    return moves;
}

std::vector<Move> legal(Board const &board, Color c)
{
    std::vector<Move> moves;
    return moves;
}
