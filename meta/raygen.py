# excalibur
# License: GPLv2
# See LICENSE for full license text
# Author: Sam Kravitz
# 
# FILE: meta/raygen.py
# DATE: February 2nd, 2022
# DESCRIPTION: generates ray attack bitboards for each of the 8 directions
# (NORTH, NORTHEAST, etc.) on each of the 64 squares for the sliding pieces.
#
# Here is a partial example of a ray attack bitboard for a rook on d4:
# East                North
# . . . . . . . .     . . . 1 . . . .
# . . . . . . . .     . . . 1 . . . .
# . . . . . . . .     . . . 1 . . . .
# . . . . . . . .     . . . 1 . . . .
# . . . R 1 1 1 1     . . . R . . . .
# . . . . . . . .     . . . . . . . .
# . . . . . . . .     . . . . . . . .
# . . . . . . . .     . . . . . . . .
#
# https://www.chessprogramming.org/Classical_Approach

from enum import Enum
from common import *

class Direction(Enum):
    NORTH     = 1
    NORTHEAST = 2
    EAST      = 3
    SOUTHEAST = 4
    SOUTH     = 5
    SOUTHWEST = 6
    WEST      = 7
    NORTHWEST = 8


def gen_ray_attacks(dir):
    table = []
    vector = (0, 0)

    if dir == Direction.NORTH:
        vector = (1, 0)
    elif dir == Direction.NORTHEAST:
        vector = (1, 1)
    elif dir == Direction.EAST:
        vector = (0, 1)
    elif dir == Direction.SOUTHEAST:
        vector = (-1, 1)
    elif dir == Direction.SOUTH:
        vector = (-1, 0)
    elif dir == Direction.SOUTHWEST:
        vector = (-1, -1)
    elif dir == Direction.WEST:
        vector = (0, -1)
    elif dir == Direction.NORTHWEST:
        vector = (1, -1)
    
    for idx in range(0, 64):
        bitboard = 0
        rank, file = rank_file_from_idx(idx)
        rank += vector[0]
        file += vector[1]
        while in_bounds(rank, file):
            bitboard = set_bit(bitboard, rank * 8 + file)
            rank += vector[0]
            file += vector[1]
        
        table.append(bitboard)

    return table

table = gen_ray_attacks(Direction.NORTHWEST)
print_table(table)