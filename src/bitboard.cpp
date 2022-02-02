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

bitset_iter indeces_set(u64 bitboard) { return bitset::indices_on(std::bitset<64>(bitboard)); }