/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: search.cpp
 * DATE: February 16th, 2022
 * DESCRIPTION: Search & evaluation functionality
 */

#include "search.h"

#include <bit>
#include <limits>

#include "board.h"
#include "constants.h"
#include "move.h"
#include "movegen.h"
#include "util.h"

/**
 * @brief search a position for the best move using the negamax algorithm
 * @param depth ply number of ply to search
 * @return evaluation
 */
float negamax(int depth, float alpha, float beta)
{   
    if (depth == 0)
        return evaluate();
    
    float max = -std::numeric_limits<float>::infinity();
    auto legal_moves = generate_moves();
    for (const auto mv : legal_moves)
    {
        board.make_move(mv);
        float score = -negamax(depth - 1, 0, 0);
        board.undo_move(mv);
        if (score > max)
            max = score;
    }

    return max;
}

/**
 * @brief search a position for the best move using the alpha-beta algorithm
 * @param depth number of ply into the future to search
 * @return evaluation
 */
float alphabeta(int depth, float alpha, float beta)
{
    if (depth == 0)
        return evaluate();

    auto legal_moves = generate_moves();

    for (const auto mv : legal_moves)
    {
        board.make_move(mv);
        float score = -alphabeta(depth - 1, -beta, -alpha);
        board.undo_move(mv);
        if (score >= beta)
            return beta;
        
        if (score > alpha)
            alpha = score;
    }

    return alpha;
};

/**
 * @brief use an algorithm f to search positions
 * f can either be negamax or alphabeta
 * @param depth number of ply into the future to search
 * @return tuple of <best_move, evaluation>
 */
std::tuple<Move, float> search(int depth, std::function<float(int, float, float)> f)
{
    Move best_move;
    auto legal_moves = generate_moves();
    float max = -std::numeric_limits<float>::infinity();

    for (const auto mv : legal_moves)
    {
        board.make_move(mv);
        float score = -f(depth - 1, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
        board.undo_move(mv);
        
        if (score > max)
        {
            max = score;
            best_move = mv;
        }
    }

    return std::make_tuple(best_move, max);
}

/**
 * @brief evaluate a position
 * @return the evaluation - it is relative to the player to move so a positive score is
 * winning for the player while a negative score is winning for the opponent
 */
float evaluate()
{
    float eval = 0.0f;

    eval += std::popcount(board.pieces(PAWN, WHITE))   * Constants::PIECE_VALUE[PAWN];
    eval += std::popcount(board.pieces(KNIGHT, WHITE)) * Constants::PIECE_VALUE[KNIGHT];
    eval += std::popcount(board.pieces(BISHOP, WHITE)) * Constants::PIECE_VALUE[BISHOP];
    eval += std::popcount(board.pieces(ROOK, WHITE))   * Constants::PIECE_VALUE[ROOK];
    eval += std::popcount(board.pieces(QUEEN, WHITE))  * Constants::PIECE_VALUE[QUEEN];

    eval -= std::popcount(board.pieces(PAWN, BLACK))   * Constants::PIECE_VALUE[PAWN];
    eval -= std::popcount(board.pieces(KNIGHT, BLACK)) * Constants::PIECE_VALUE[KNIGHT];
    eval -= std::popcount(board.pieces(BISHOP, BLACK)) * Constants::PIECE_VALUE[BISHOP];
    eval -= std::popcount(board.pieces(ROOK, BLACK))   * Constants::PIECE_VALUE[ROOK];
    eval -= std::popcount(board.pieces(QUEEN, BLACK))  * Constants::PIECE_VALUE[QUEEN];

    int perspective = board.mover() == WHITE ? 1 : -1;

    return eval * perspective;
}