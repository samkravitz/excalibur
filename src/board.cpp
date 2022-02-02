/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: board.cpp
 * DATE: January 31st, 2022
 * DESCRIPTION: A board representation contains bitboards of each piece type of each color.
 */

#include "board.h"

#include <algorithm>
#include <cstring>
#include <vector>

#include "bitboard.h"
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
    piece_bb[PAWN]   = Constants::WPAWNS_INIT   | Constants::BPAWNS_INIT;
    piece_bb[KNIGHT] = Constants::WKNIGHTS_INIT | Constants::BKNIGHTS_INIT;
    piece_bb[BISHOP] = Constants::WBISHOPS_INIT | Constants::BBISHOPS_INIT;
    piece_bb[ROOK]   = Constants::WROOKS_INIT   | Constants::BROOKS_INIT;
    piece_bb[QUEEN]  = Constants::WQUEENS_INIT  | Constants::BQUEENS_INIT;
    piece_bb[KING]   = Constants::WKING_INIT    | Constants::BKING_INIT;

    color_bb[WHITE]  = Constants::WPAWNS_INIT | Constants::WKNIGHTS_INIT | Constants::WBISHOPS_INIT | Constants::WROOKS_INIT | Constants::WQUEENS_INIT | Constants::WKING_INIT;
    color_bb[BLACK]  = Constants::BPAWNS_INIT | Constants::BKNIGHTS_INIT | Constants::BBISHOPS_INIT | Constants::BROOKS_INIT | Constants::BQUEENS_INIT | Constants::BKING_INIT;

    to_move = WHITE;
}

std::string Board::to_string() const
{
    char board[8][8];
    std::memset(board, '*', sizeof(board));

    const std::vector<u64> bb_all = { 
        pieces(PAWN,   WHITE),
        pieces(PAWN,   BLACK),
        pieces(KNIGHT, WHITE),
        pieces(KNIGHT, BLACK),
        pieces(BISHOP, WHITE),
        pieces(BISHOP, BLACK),
        pieces(ROOK,   WHITE),
        pieces(ROOK,   BLACK),
        pieces(QUEEN,  WHITE),
        pieces(QUEEN,  BLACK),
        pieces(KING,   WHITE),
        pieces(KING,   BLACK),
    };
    const std::vector<char> chars = { 'P', 'p', 'N', 'n', 'B', 'b', 'R', 'r','Q', 'q', 'K', 'k' };

    std::vector<std::tuple<u64, char>> zipped;

    std::transform(bb_all.begin(), bb_all.end(), chars.begin(), std::back_inserter(zipped), [](const auto &a, const auto &b) {
        return std::make_tuple(a, b);
    });

    for (auto [bitboard, c] : zipped)
    {
        for (const auto &idx : indeces_set(bitboard))
        {
            auto const [rank, file] = Util::rank_file_from_square(Square(idx));
            board[rank][file] = c;
        }
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