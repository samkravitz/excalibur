#pragma once

#include <vector>

#include "board.h"
#include "move.h"

std::vector<Move> pseudolegal(Board const &, Color);
std::vector<Move> legal(Board const &, Color);

inline std::vector<Move> pseudolegal(Board const &board) { return pseudolegal(board, board.mover()); }
inline std::vector<Move> legal(Board const &board) { return legal(board, board.mover()); }
