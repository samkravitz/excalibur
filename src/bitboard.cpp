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

#include <bit>

Bitboard::Bitboard(u64 bitboard)
    : bitboard(bitboard)
{

}

void Bitboard::set(u64 bitboard)
{
    bitboard = bitboard;
}

/**
 * @brief iterate over all set bits of a bitboard, applying a callback function to each set bit
 * 
 * @param callback function to apply to each set bit of a bitboard. The argument to the callback is
 * the board-index (8 * rank + file) of each set bit.
 *
 * @return None
 */
void Bitboard::iterate(std::function<void(int)> callback) const
{
	// agorithm adopted from https://lemire.me/blog/2018/02/21/iterating-over-set-bits-quickly, 
	// with some changes by me
    u64 bitset = bitboard;
    while (bitset)
    {
    	u64 t = bitset & -bitset;
    	int idx = std::countr_zero(bitset); 
		callback(idx);
		bitset ^= t;
    }
}
