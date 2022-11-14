/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: zobrist.cpp
 * DATE: November 13th, 2022
 * DESCRIPTION: zobrist hashing algorithms from http://hgm.nubati.net/book_format.html
 */

#include "zobrist.h"

#include "types.h"
#include "util.h"

static u64 piece(Board const &board)
{
	u64 result = 0;

	for (int i = 0; i < 64; i++)
	{
		auto square = static_cast<Square>(i);
		auto piece_type = board.piece_on(square);

		if (piece_type == NONE)
			continue;

		int kind_of_piece = -1;
		if (board.pieces(PAWN, BLACK) & square)
			kind_of_piece = 0;
		else if (board.pieces(PAWN, WHITE) & square)
			kind_of_piece = 1;

		else if (board.pieces(KNIGHT, BLACK) & square)
			kind_of_piece = 2;
		else if (board.pieces(KNIGHT, WHITE) & square)
			kind_of_piece = 3;

		else if (board.pieces(BISHOP, BLACK) & square)
			kind_of_piece = 4;
		else if (board.pieces(BISHOP, WHITE) & square)
			kind_of_piece = 5;

		else if (board.pieces(ROOK, BLACK) & square)
			kind_of_piece = 6;
		else if (board.pieces(ROOK, WHITE) & square)
			kind_of_piece = 7;

		else if (board.pieces(QUEEN, BLACK) & square)
			kind_of_piece = 8;
		else if (board.pieces(QUEEN, WHITE) & square)
			kind_of_piece = 9;

		else if (board.pieces(KING, BLACK) & square)
			kind_of_piece = 10;
		else if (board.pieces(KING, WHITE) & square)
			kind_of_piece = 11;
		
		auto [rank, file] = Util::rank_file_from_square(square);

		int offset_piece = 64 * kind_of_piece + 8 * rank + file;
		result ^= random64[offset_piece];
	}

	return result;
}

static u64 castle(Board const &board)
{
	u64 result = 0;
	if (board.get_castle_rights(WHITE, KINGSIDE))
		result ^= random64[CASTLE_OFFSET + 0];
	if (board.get_castle_rights(WHITE, QUEENSIDE))
		result ^= random64[CASTLE_OFFSET + 1];
	if (board.get_castle_rights(BLACK, KINGSIDE))
		result ^= random64[CASTLE_OFFSET + 2];
	if (board.get_castle_rights(BLACK, QUEENSIDE))
		result ^= random64[CASTLE_OFFSET + 3];

	return result;
}

static u64 enpassant(Board const &board)
{
	auto ep_sq = board.get_ep_sq();
	if (ep_sq == EP_NONE)
		return 0;

	auto [rank, file] = Util::rank_file_from_square(ep_sq);

	Square pawn_square;

	if (rank == RANK_3)
		pawn_square = Util::square_from_rank_file(RANK_4, file);
	else
		pawn_square = Util::square_from_rank_file(RANK_5, file);


	// moved pawn is not on A file
	if (file != FILE_A)
	{
		Square one_left = static_cast<Square>(pawn_square - 1);

		if (board.pieces(PAWN, board.mover()) & one_left)
			return random64[ENPASSANT_OFFSET + file];
	}

	// moved pawn is not on H file
	if (file != FILE_H)
	{
		Square one_right = static_cast<Square>(pawn_square + 1);

		if (board.pieces(PAWN, board.mover()) & one_right)
			return random64[ENPASSANT_OFFSET + file];
	}

	return 0;
}

static u64 turn(Board const &board)
{
	return board.mover() == WHITE ? random64[TURN_OFFSET] : 0;
}

u64 zobrist(Board const &board)
{
	return piece(board) ^ castle(board) ^ enpassant(board) ^ turn(board);
}
