#!usr/bin/end python3

from io import TextIOWrapper
from typing import List
import random, sys
from pathlib import Path
import os 

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
    
    for i in range(len(diag)):
        mat[i][i] = diag[i]
    return variate_matrix(mat)

def variate_matrix(mat: List[List[float]]):
    for _ in range(BIG_NUMBER):
        for i in range(len(mat)):
            j = random.randint(len(mat))
            if i != j:
                coef: float = float(random.randint(-16, 16))
                for k in range(len(mat)):
                    mat[i][k] += coef * mat[j][k]
    return mat

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

def print_mat(mat: List[List[float]], file: TextIOWrapper):
    file.write(str(len(mat)) + '\n')
    for i in range(len(mat)):
        for j in range(len(mat[i])):
            file.write(str(mat[i][j]) + ' ')
    file.write('\n')

def main():
    mat_sz = int(sys.argv[1])
    max_num = int(sys.argv[2])
    min_num = - max_num
    diag = generate_diagonal(mat_sz, min_num, max_num)
    det  = calc_det(diag)
    rang = calc_rang(diag)
    mat  = make_matrix(diag)
    i: int = 3
    if sys.argv[3] == '--rang':
        i += 1

    path, file_name: str = os.path.split(sys.argv[i])
    if len(path) != 0:
        path += '/'

    mat_file_name = path + file_name + '_mat'
    mat_file: TextIOWrapper = open(mat_file_name)
    print_mat(mat, mat_file)
    mat_file.close()

    if i == 4:
        rang_file_name = path + file_name + '_rang'
        rang_file: TextIOWrapper = open(rang_file_name)
        rang_file.write(str(rang))
        rang_file.close()
    
    det_file_name = path + file_name + '_det'
    det_file: TextIOWrapper = open(det_file_name)
    det_file.write(str(det))
    det_file.close()
    
    