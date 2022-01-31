/* excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: board.h
 * DATE: January 31st, 2022
 * DESCRIPTION: A board representation contains bitboards of each piece type of each color.
 * The board can handle move generation of a position
 */

#pragma once

#include <string>

#include "bitboard.h"

class Board
{
public:
    Board();

    std::string to_string() const;
private:
    Bitboard wpawns,   bpawns;
    Bitboard wknights, bknights;
    Bitboard wbishops, bbishops;
    Bitboard wrooks,   brooks;
    Bitboard wqueens,  bqueens;
    Bitboard wking,    bking;

    void reset();
};
