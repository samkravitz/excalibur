/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: engine.cpp
 * DATE: February 2nd, 2022
 * DESCRIPTION: Search & evaluation functionality
 */

#include "engine.h"

#include <algorithm>
#include <bit>
#include <cctype>
#include <functional>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

#include "constants.h"
#include "move.h"
#include "movegen.h"
#include "util.h"

/**
 * @brief evaluate a position
 * @return the evaluation - a more positive score is better for white while a more negative score is better for black
 */
float Engine::evaluate()
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

/**
 * @brief search a position for the best move using the negamax algorithm
 * @param depth ply number of ply to search
 * @return tuple of <best_move, evaluation>
 */
std::tuple<Move, float> Engine::negamax(int depth)
{   
    std::function<float(int)> helper = [&](int depth) -> float
    {
        if (depth == 0)
            return evaluate();
        
        float max = -std::numeric_limits<float>::infinity();
        auto legal_moves = generate_moves();
        for (const auto mv : legal_moves)
        {
            board.make_move(mv);
            float score = -helper(depth - 1);
            board.undo_move(mv);
            if (score > max)
                max = score;
        }
        return max;
    };

    Move best_move;
    auto legal_moves = generate_moves();
    float max = -std::numeric_limits<float>::infinity();
    for (const auto mv : legal_moves)
    {
        board.make_move(mv);
        float score = helper(depth - 1);
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
 * @brief use the alpha-beta pruning algorithm to search positions
 * @param depth number of ply into the future to search
 * @return tuple of <best_move, evaluation>
 */
std::tuple<Move, float> Engine::alphabeta(int depth)
{
    std::function<float(int, float, float)> helper = [&](int depth, float alpha, float beta) -> float
    {
        if (depth == 0)
            return evaluate();

        auto legal_moves = generate_moves();

        for (const auto mv : legal_moves)
        {
            board.make_move(mv);
            float score = -helper(depth - 1, -beta, -alpha);
            board.undo_move(mv);
            if (score >= beta)
                return beta;
            
            if (score > alpha)
                alpha = score;
        }

        return alpha;
    };

    Move best_move;
    auto legal_moves = generate_moves();
    float max = -std::numeric_limits<float>::infinity();
    for (const auto mv : legal_moves)
    {
        board.make_move(mv);
        float score = helper(depth - 1, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
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
 * @brief find the best move in a position
 * @return tuple of <best_move, evaluation>
 */
std::tuple<Move, float> Engine::best_move()
{
    auto legal_moves = generate_moves();
    Color to_move = board.mover();
    auto best_evalutation = to_move == WHITE ? -1000 : 1000;
    std::vector<std::tuple<Move, float>> considered_moves;

    const auto ge = [](float a, float b) { return std::greater<float>{}(a, b); };
    const auto le = [](float a, float b) { return std::less<float>{}(a, b); };

    auto cmp_func = to_move == WHITE ? +ge : +le;

    for (auto const &mv : legal_moves)
    {
        board.make_move(mv);
        float eval = evaluate();

        if (eval == best_evalutation)
            considered_moves.push_back(std::make_tuple(mv, eval));

        else if (cmp_func(eval, best_evalutation))
        {
            considered_moves.clear();
            considered_moves.push_back(std::make_tuple(mv, eval));
            best_evalutation = eval;
        }

        board.undo_move(mv);
    }

    return Util::get_random_element(considered_moves);
}

void Engine::parse_uci_moves(std::string const &moves)
{
    std::stringstream ss(moves);
    std::string move = "";
    while (std::getline(ss, move, ' '))
        parse_uci_move(move);
}

void Engine::parse_uci_move(std::string const &move)
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
        char promo = move.at(4);
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
                case QUEEN_PROMOTION:  flags = QUEEN_PROMO_CAPTURE;  break;
                case KNIGHT_PROMOTION: flags = KNIGHT_PROMO_CAPTURE; break;
                case BISHOP_PROMOTION: flags = BISHOP_PROMO_CAPTURE; break;
                case ROOK_PROMOTION:   flags = ROOK_PROMO_CAPTURE;   break;
            }
        }

        // regular capture
        else
        {
            flags = CAPTURE;
        }
    }

    // check if move is enpassant
    if (!is_capture && board.piece_on(to) == PAWN && (to == static_cast<Square>(from + 7) || to == static_cast<Square>(from + 9) || from == static_cast<Square>(from - 7) || from == static_cast<Square>(to - 9)))
        flags = ENPASSANT;
    
    // check if move is double pawn push
    if (board.piece_on(from) == PAWN && (to == static_cast<Square>(from + 16) || to == static_cast<Square>(from - 16)))
        flags = DOUBLE_PAWN_PUSH;

    Move mv(from, to, flags);
    board.make_move(mv);
}

void Engine::load_fen(std::string const &fen)
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
            default: break;
        }
    }

    // section 4 - en passant target square
    if (sections[3] != "-")
        board.set_ep_sq(Util::from_algebraic(sections[3]));
}
