#!usr/bin/end python3

from typing import List
import random, sys

BIG_NUMBER: int = 1024

def generate_diagonal(mat_sz: int, min_num: int, max_num: int):
    lst: List[float] = []
    for i in range(mat_sz):
        lst[i] = float(random.randint(min_num, max_num))
    return lst

def make_matrix(diag: List[float]):
    mat: List[List[float]] = []
    for i in range(len(diag)):
        mat.append()
        for _ in range(len(diag)):
            mat[i].append()
    return mat

def variate_matrix(mat: List[List[float]]):
    for _ in range(BIG_NUMBER):
        for i in range(len(mat)):
            j = random.randint(len(mat))
            if i != j:
                coef: float = float(random.randint(-16, 16))
                for k in range(len(mat)):
                    mat[i][k] += coef * mat[j][k]

def calc_det(diag: List[float]):
    res: float = 1
    for i in range(len(diag)):
        res *= diag[i]
    return res

def flt_cmp(f1: float, f2: float):
    return abs(f1 - f2) < (abs(f1) + abs(f2)) * 1e-10

def calc_rang(diag: List[float]):
    res: int = 0
    for i in range(len(diag)):
        if flt_cmp(diag[i], 0) == False:
            res += 1
    return res

def main():

