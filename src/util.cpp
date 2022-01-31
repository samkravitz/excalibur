/* excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: util.cpp
 * DATE: January 31st, 2022
 * DESCRIPTION: Utility functions not fitting in another class
 */

#include "util.h"

/**
 * @brief gets the rank & file of a square on the board
 * @param square board-index of square to get
 * @return tuple of { rank, file } of square
 * 
 * example invocation:
 * auto [rank, file] = rank_file_from_square(11) => [1, 3]
 */
std::tuple <int, int> Util::rank_file_from_square(Square square)
{
    int rank = square / 8;
    int file = square % 8;
    return std::make_tuple(rank, file); 
}

/**
 * @brief gets the algebraic notation representation of a square
 * @param square board-index of square to turn to algebraic notation
 * @return the algebraic representation of the square
 * 
 * example invocation:
 * auto algebraic = to_algebraic(E4) => "e4"
 */
std::string Util::to_algebraic(Square square)
{
    std::string res = "";
    auto [rank, file] = rank_file_from_square(square);
    res += file + 'a';
    res += std::to_string(rank + 1);
    return res;    
}