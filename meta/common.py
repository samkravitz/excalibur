# excalibur
# License: GPLv2
# See LICENSE for full license text
# Author: Sam Kravitz
# 
# FILE: meta/common.py
# DATE: February 2nd, 2022
# DESCRIPTION: utilities shared throughout python scripts
def rank_file_from_idx(idx):
    return int(idx / 8), int(idx % 8)

def set_bit(bitboard, idx):
    bitboard |= (1 << idx)
    return int(bitboard)

def in_bounds(rank, file):
    return rank >= 0 and rank < 8 and file >= 0 and file < 8

def print_table(table):
    for i in table:
        print("{0:#018x}".format(i))