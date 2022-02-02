/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: board.cpp
 * DATE: January 31st, 2022
 * DESCRIPTION: A board representation contains bitboards of each piece type of each color.
 * The board can handle move generation of a position
 */

#include "board.h"

#include <cstring>

#include "constants.h"
#include "util.h"

Board::Board()
{
    reset();
}

/**
 * @brief returns board to initial position of a chess game
 */
void Board::reset()
{
    wpawns   = Bitboard(Constants::WPAWNS_INIT);
    bpawns   = Bitboard(Constants::BPAWNS_INIT);
    wknights = Bitboard(Constants::WKNIGHTS_INIT);
    bknights = Bitboard(Constants::BKNIGHTS_INIT);
    wbishops = Bitboard(Constants::WBISHOPS_INIT);
    bbishops = Bitboard(Constants::BBISHOPS_INIT);
    wrooks   = Bitboard(Constants::WROOKS_INIT);
    brooks   = Bitboard(Constants::BROOKS_INIT);
    wqueens  = Bitboard(Constants::WQUEENS_INIT);
    bqueens  = Bitboard(Constants::BQUEENS_INIT);
    wking    = Bitboard(Constants::WKING_INIT);
    bking    = Bitboard(Constants::BKING_INIT);
}

std::string Board::to_string() const
{
    char board[8][8];
    std::memset(board, '*', sizeof(board));

    wpawns.iterate([&](int idx) {
      auto const [rank, file] = Util::rank_file_from_square(Square(idx));
      board[rank][file] = 'P';
    });

    std::string res = "";

    for (int rank = RANK_8; rank >= RANK_1; --rank)
    {
        for (int file = FILE_A; file <= FILE_H; ++file)
        {
            res += board[rank][file];
            res += ' ';
        }
        res += '\n';
    }

    return res;
}