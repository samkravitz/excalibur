
/* excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: types.h
 * DATE: January 30th, 2022
 * DESCRIPTION: common types used throughout excalibur
 */

#pragma once

#include <cstdint>

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

enum Color
{
    WHITE,
    BLACK,
};

inline Color operator~(Color c) { return c == WHITE ? BLACK : WHITE; }

enum Square : int
{
  A1, B1, C1, D1, E1, F1, G1, H1,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A8, B8, C8, D8, E8, F8, G8, H8,
};

enum Rank : int
{
    RANK_1 = 0,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8,
};

enum File : int
{
    FILE_A = 0,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H,
};

enum Direction : int
{
    NORTH     =  8,
    NORTHEAST =  9,
    EAST      =  1,
    SOUTHEAST = -7,
    SOUTH     = -8,
    SOUTHWEST = -9,
    WEST      = -1,
    NORTHWEST =  7,
};


enum MoveFlags : int
{
    QUIET_MOVE,
    DOUBLE_PAWN_PUSH,
    KINGSIDE_CASTLE,
    QUEENSIDE_CASTLE,
    CAPTURE,
    ENPASSANT,
    KNIGHT_PROMOTION,
    BISHOP_PROMOTION,
    ROOK_PROMOTION,
    QUEEN_PROMOTION,
    KNIGHT_PROMO_CAPTURE,
    BISHOP_PROMO_CAPTURE,
    ROOK_PROMO_CAPTURE,
    QUEEN_PROMO_CAPTURE,
};
