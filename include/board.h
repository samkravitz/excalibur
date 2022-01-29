#pragma once

#include <stack>
#include <string>
#include <vector>

#include "move.h"
#include "piece.h"

class Board
{
public:
    Board();

    Piece *board[8][8];
    Piece *pieces[32];

    std::stack<Move> moves;

    bool piece_on_square(Square);
    bool is_in_check(Color);
    std::vector<Square> get_squares_attacked(Color);
    std::vector<Move> get_legal_moves(Color);

    void make_move(Move const &);
    void undo_last_move();

    std::string to_string();
    Color to_play();

    static bool is_in_bounds(Square square)
    {
        return (
            (square.rank >= 0) &&
            (square.file >= 0) &&
            (square.rank < 8) &&
            (square.file < 8));
    }

private:
    Piece *last_move_captured_piece = nullptr;
    Piece *last_move_promoted_pawn = nullptr;
    void reset();
};