/* excalibur
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

Bitboard::Bitboard(u64 bitboard)
    : bitboard(bitboard)
{

}

void Bitboard::set(u64 bitboard)
{
    bitboard = bitboard;
}
