/* excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: bitboard.h
 * DATE: January 31st, 2022
 * DESCRIPTION: bitboard representation
 * 
 * A bitboard is a bit-array representation of every square on a chess board.
 * Since there are 64 squares on a chess board, we can use a 64 bit integer
 * to represent every square in a single CPU word. To keep track of the entire
 * board, we will keep track of bitboards for every piece of every type, ie
 * a bitboard for all the white pawns, for the black king, etc.
 * https://www.chessprogramming.org/Bitboards
 */

#pragma once
