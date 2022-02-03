# excalibur
# License: GPLv2
# See LICENSE for full license text
# Author: Sam Kravitz
# 
# FILE: meta/tablegen.py
# DATE: January 31st, 2022
# DESCRIPTION: generates look up tables for pawn, knight, and king
# moves so that excalibur doesn't need to compute them at runtime

from common import *

def gen_pawn_pushes(color):
    table = []

    perspective = 1 if color == 'white' else -1
    home_rank = 1 if color == 'white' else 6
    for idx in range(0, 64):
        bitboard = 0
        rank, file = rank_file_from_idx(idx)
        if rank == 0 or rank == 7:
            pass
        elif rank == home_rank:
            bitboard = set_bit(bitboard, idx + 8  * perspective) # push 1 square
            bitboard = set_bit(bitboard, idx + 16 * perspective) # push 2 squares
        else:
            bitboard = set_bit(bitboard, idx + 8  * perspective) # push 1 square
        table.append(bitboard)
    return table

def gen_pawn_attacks(color):
    table = []

    perspective = 1 if color == 'white' else -1
    for idx in range(0, 64):
        bitboard = 0
        rank, file = rank_file_from_idx(idx)
        
        candidates = [
            (rank + perspective, file + 1),
            (rank + perspective, file - 1),
        ]

        for candidate in candidates:
            new_idx = candidate[0] * 8 + candidate[1]
            if not in_bounds(candidate[0], candidate[1]):
                continue
                
            bitboard = set_bit(bitboard, new_idx)

        table.append(bitboard)
    return table

def gen_knight_moves():
    table = []
    for idx in range(0, 64):
        bitboard = 0
        rank, file = rank_file_from_idx(idx)
        candidates = [
            (rank + 1, file + 2),
            (rank + 1, file - 2),
            (rank + 2, file + 1),
            (rank + 2, file - 1),
            (rank - 1, file + 2),
            (rank - 1, file - 2),
            (rank - 2, file + 1),
            (rank - 2, file - 1),
        ]

        for candidate in candidates:
            new_idx = candidate[0] * 8 + candidate[1]
            if not in_bounds(candidate[0], candidate[1]):
                continue
                
            bitboard = set_bit(bitboard, new_idx)

        table.append(bitboard)
    return table

def gen_king_moves():
    table = []
    for idx in range(0, 64):
        bitboard = 0
        rank, file = rank_file_from_idx(idx)
        candidates = [
            (rank + 1, file + 1),
            (rank + 1, file - 1),
            (rank - 1, file + 1),
            (rank - 1, file - 1),
            (rank + 1, file),
            (rank - 1, file),
            (rank, file + 1),
            (rank, file - 1),
        ]

        for candidate in candidates:
            new_idx = candidate[0] * 8 + candidate[1]
            if not in_bounds(candidate[0], candidate[1]):
                continue
                
            bitboard = set_bit(bitboard, new_idx)

        table.append(bitboard)
    return table
