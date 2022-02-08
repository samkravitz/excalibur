# excalibur
# License: GPLv2
# See LICENSE for full license text
# Author: Sam Kravitz
# 
# FILE: meta/dirgen.py
# DATE: February 7th, 2022
# DESCRIPTION: generates look up tables for the direction from 
# one square to another square

from common import *

def gen_direction_table():
    for to_idx in range(0, 64):
        to_rank, to_file = rank_file_from_idx(to_idx)
        print('{')
        for from_idx in range(0, 64):
            from_rank, from_file = rank_file_from_idx(from_idx)
            if from_idx == to_idx:
                print('\tNORTH,')
            elif from_rank == to_rank:
                if from_file > to_file:
                    print('\tEAST,')
                else:
                    print('\tWEST,')
            elif from_file == to_file:
                if from_rank > to_rank:
                    print('\tNORTH,')
                else:
                    print('\tSOUTH,')
            else:
                if from_rank > to_rank and from_file > to_file:
                    print('\tNORTHEAST,')
                elif from_rank > to_rank and from_file < to_file:
                    print('\tNORTHWEST,')
                elif from_rank < to_rank and from_file > to_file:
                    print('\tSOUTHEAST,')
                else:
                    print('\tSOUTHWEST,')
        print('},')

gen_direction_table()