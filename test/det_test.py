#! /usr/bin/env python3

from io import TextIOWrapper
from typing import List
import random, sys
import os
import time 

def take_answer(file_name):
    f = open(file_name, "r")
    answer = float(f.read())
    return answer


def flt_cmp(f1: float, f2: float):
    return abs(f1 - f2) < (abs(f1) + abs(f2) + 1) * 1e-3

def take_det(run_file_name, matrix_file):
    os.system("./" + run_file_name + " < " + matrix_file + " > out")
    f = open("out", "r")
    det_act = float(f.read())
    return det_act

def main():
    for i in range(2, len(sys.argv)):
        det_test = take_answer(sys.argv[i] + "_det")
        print("Start run...")
        det_act  = take_det(sys.argv[1], sys.argv[i] + "_mat")
        print("Finish run... time:" + str(time.monotonic()))
        if flt_cmp(det_test, det_act) == True:
            print(str(i - 1) + ": Succes")
        else:
            print(str(i - 1) + ": Error")
            print("det from run file: " + str(det_act) + "\ndet form test: " + str(det_test))

main()

