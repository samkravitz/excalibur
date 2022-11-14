/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: movelist.cpp
 * DATE: February 16th, 2022
 * DESCRIPTION: wrapper class for holding all the possible moves in a position
 */

#include "movelist.h"

class Board;
extern Board board;

#include <algorithm>

int score(Move const &mv, Board const &board)
{
	int guess = 0;
	auto piece_type = board.piece_on(mv.from());
	auto capture_piece_type = board.piece_on(mv.to());

	if (piece_type != NONE)
		guess = 10 * Constants::PIECE_VALUE[capture_piece_type] - Constants::PIECE_VALUE[piece_type];

	if (mv.is_promotion())
		guess += Constants::PIECE_VALUE[QUEEN];

	auto opponent = ~board.mover();
	if (Constants::pawn_attack_table[opponent][mv.to()] & board.pieces(PAWN, opponent))
		guess -= Constants::PIECE_VALUE[piece_type];

	return guess;
}

Movelist::Movelist()
{
	m_size = 0;
}

void Movelist::add(Move mv)
{
	movelist[m_size++] = mv;
}

void Movelist::order()
{
	std::sort(movelist.begin(),
	          movelist.begin() + size(),
	          [](Move const &lhs, Move const &rhs) { return score(lhs, board) < score(rhs, board); });
}
