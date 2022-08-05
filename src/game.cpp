/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: game.cpp
 * DATE: February 16th, 2022
 * DESCRIPTION: Functions that affect the global state of the game
 */

#include "game.h"

#include <iostream>
#include <sstream>

#include "board.h"
#include "util.h"

void parse_uci_moves(std::string const &moves)
{
	std::stringstream ss(moves);
	std::string move = "";
	while (std::getline(ss, move, ' '))
		parse_uci_move(move);
}

void parse_uci_move(std::string const &move)
{
	Square from = Util::from_algebraic(move.substr(0, 2));
	Square to = Util::from_algebraic(move.substr(2, 2));

	MoveFlags flags = QUIET_MOVE;

	// check if move is capture
	bool is_capture = board.piece_on(to) != NONE;

	// check if move is promotion
	bool is_promotion = false;
	if (move.length() == 5)
	{
		is_promotion = true;
		char promo   = move.at(4);
		switch (promo)
		{
			case 'q': flags = QUEEN_PROMOTION;  break;
			case 'k': flags = KNIGHT_PROMOTION; break;
			case 'b': flags = BISHOP_PROMOTION; break;
			case 'r': flags = ROOK_PROMOTION;   break;
			default:
				std::cerr << "Invalid promotion move: " << move << "\n";
		}
	}

	// check if move is castle
	if (board.piece_on(from) == KING)
	{
		// white kingside castle
		if (move == "e1g1")
			flags = KINGSIDE_CASTLE;
		// white queenside castle
		else if (move == "e1c1")
			flags = QUEENSIDE_CASTLE;
		// black kingside castle
		else if (move == "e8g8")
			flags = KINGSIDE_CASTLE;
		// black queenside castle
		else if (move == "e8c8")
			flags = QUEENSIDE_CASTLE;
	}

	if (is_capture)
	{
		// capture promotion
		if (is_promotion)
		{
			switch (flags)
			{
				case QUEEN_PROMOTION:  flags = QUEEN_PROMO_CAPTURE;   break;
				case KNIGHT_PROMOTION: flags = KNIGHT_PROMO_CAPTURE; break;
				case BISHOP_PROMOTION: flags = BISHOP_PROMO_CAPTURE; break;
				case ROOK_PROMOTION:   flags = ROOK_PROMO_CAPTURE;     break;
			}
		}

		// regular capture
		else
		{
			flags = CAPTURE;
		}
	}

	// check if move is enpassant
	if (board.piece_on(from) == PAWN && to == board.get_ep_sq())
		flags = ENPASSANT;

	// check if move is double pawn push
	if (board.piece_on(from) == PAWN && (to == static_cast<Square>(from + 16) || to == static_cast<Square>(from - 16)))
		flags = DOUBLE_PAWN_PUSH;

	Move mv(from, to, flags);
	board.make_move(mv);
}

void load_fen(std::string const &fen)
{
	board.clear();

	std::stringstream ss(fen);
	std::string section = "";
	std::vector<std::string> sections;
	while (std::getline(ss, section, ' '))
		sections.push_back(section);

	// section 1 - piece placement
	int rank = 7, file = 0;
	for (auto c : sections[0])
	{
		if (std::isalpha(c))
		{
			switch (c)
			{
				case 'P': board.set_piece(PAWN,   Util::square_from_rank_file(rank, file), WHITE); break;
				case 'p': board.set_piece(PAWN,   Util::square_from_rank_file(rank, file), BLACK); break;
				case 'N': board.set_piece(KNIGHT, Util::square_from_rank_file(rank, file), WHITE); break;
				case 'n': board.set_piece(KNIGHT, Util::square_from_rank_file(rank, file), BLACK); break;
				case 'B': board.set_piece(BISHOP, Util::square_from_rank_file(rank, file), WHITE); break;
				case 'b': board.set_piece(BISHOP, Util::square_from_rank_file(rank, file), BLACK); break;
				case 'R': board.set_piece(ROOK,   Util::square_from_rank_file(rank, file), WHITE); break;
				case 'r': board.set_piece(ROOK,   Util::square_from_rank_file(rank, file), BLACK); break;
				case 'Q': board.set_piece(QUEEN,  Util::square_from_rank_file(rank, file), WHITE); break;
				case 'q': board.set_piece(QUEEN,  Util::square_from_rank_file(rank, file), BLACK); break;
				case 'K': board.set_piece(KING,   Util::square_from_rank_file(rank, file), WHITE); break;
				case 'k': board.set_piece(KING,   Util::square_from_rank_file(rank, file), BLACK); break;
				default:
					std::cerr << "Invalid FEN: " << sections[0] << "\n";
					return;
			}

			file += 1;
		}

		else if (std::isdigit(c))
			file += c - '0';

		else if (c == '/')
		{
			rank -= 1;
			file = 0;
		}

		else
		{
			std::cerr << "Invalid FEN: " << sections[0] << "\n";
			return;
		}
	}

	// section 2 - side to move
	board.set_to_move(sections[1] == "w" ? WHITE : BLACK);

	// section 3 - castle rights
	for (auto c : sections[2])
	{
		switch (c)
		{
			case 'K': board.set_castle_rights(WHITE, KINGSIDE);  break;
			case 'Q': board.set_castle_rights(WHITE, QUEENSIDE); break;
			case 'k': board.set_castle_rights(BLACK, KINGSIDE);  break;
			case 'q': board.set_castle_rights(BLACK, QUEENSIDE); break;
		}
	}

	// section 4 - en passant target square
	if (sections[3] != "-")
		board.set_ep_sq(Util::from_algebraic(sections[3]));
}
