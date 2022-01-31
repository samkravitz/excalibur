/* excalibur
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

#include <algorithm>
#include <cstring>
#include <vector>

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

    const std::vector<Bitboard> bb_all = { wpawns, bpawns, wknights, bknights, wbishops, bbishops, wrooks, brooks, wqueens, bqueens, wking, bking };
    const std::vector<char> chars = { 'P', 'p', 'N', 'n', 'B', 'b', 'R', 'r','Q', 'q', 'K', 'k' };

    std::vector<std::tuple<Bitboard, char>> zipped;

    std::transform(bb_all.begin(), bb_all.end(), chars.begin(), std::back_inserter(zipped), [](const auto &a, const auto &b) {
        return std::make_tuple(a, b);
    });

    for (const auto [bitboard, c] : zipped)
    {
        bitboard.iterate([&](int idx) {
            auto const [rank, file] = Util::rank_file_from_square(Square(idx));
            board[rank][file] = c;
        });
    }

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