#!/usr/bin/env python3

from io import TextIOWrapper
from typing import List
import random, sys
import os

# matrix size       - 1 arg
# matrix max num    - 2 arg
# file name to dump - 3 arg

BIG_NUMBER: int = 64

def generate_diagonal(mat_sz: int, min_num: int, max_num: int):
    lst: List[float] = []
    for _ in range(mat_sz):
        number = float(random.randint(min_num, max_num))
        if number == 0.0:
            number = 1
        lst.append(number)
    for i in range(mat_sz):
        lst[i] *= 0.75
    return lst

def make_matrix(diag: List[float]):
    mat: List[List[float]] = []

    for i in range(len(diag)):
        mat.append([])
        for _ in range(len(diag)):
            mat[i].append(0)
    
    for i in range(len(diag)):
        mat[i][i] = diag[i]

    return variate_matrix(mat)

def variate_matrix(mat: List[List[float]]):
    for _ in range(len(mat) * 16):
        i = int(random.randint(0, len(mat) - 1))
        j = int(random.randint(0, len(mat) - 1))
        if i!= j:
            coef: float = float(random.randint(-2, 2))
            for k in range(len(mat)):
                mat[i][k] += coef * mat[j][k]    
    return mat

def calc_det(diag: List[float]):
    res: float = 1
    for i in range(len(diag)):
        res *= diag[i]
    return res

def print_mat(mat: List[List[float]], file: TextIOWrapper):
    file.write(str(len(mat)) + '\n')
    for i in range(len(mat)):
        for j in range(len(mat[i])):
            file.write(str(mat[i][j]) + ' ')
    file.write('\n')

def main():
    mat_sz  = int(sys.argv[1])
    max_num = int(sys.argv[2])
    min_num = - max_num
    diag = generate_diagonal(mat_sz, min_num, max_num)
    det  = calc_det(diag)
    mat  = make_matrix(diag)
    
    path, file_name = os.path.split(sys.argv[3])
    if len(path) != 0:
        path += '/'

    mat_file_name = path + file_name + '_mat'
    mat_file: TextIOWrapper = open(mat_file_name, 'w')
    print_mat(mat, mat_file)
    mat_file.close()

    det_file_name = path + file_name + '_det'
    det_file: TextIOWrapper = open(det_file_name, 'w')
    det_file.write(str(det) + '\n')
    det_file.close()

main()
    
    