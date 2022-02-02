/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: bitboard.cpp
 * DATE: January 31st, 2022
 * DESCRIPTION: implementation of bitboard class
 * 
 * https://www.chessprogramming.org/Bitboards
 */

#include "bitboard.h"

#include <iostream>

#include "types.h"

bitset_iter indeces_set(u64 bitboard) { return bitset::indices_on(std::bitset<64>(bitboard)); }

void print_bitboard(u64 bitboard)
{
    for (int rank = RANK_8; rank >= RANK_1; --rank)
    {
        for (int file = FILE_A; file <= FILE_H; ++file)
        {
            Square s = static_cast<Square>(rank * 8 + file);
            char c = bitboard & s ? '1' : '*';
            std::cout << c << ' ';
        }
        std::cout << '\n';
    }
}