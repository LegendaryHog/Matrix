#! /usr/bin/env python3

from io import TextIOWrapper
from typing import List
import random, sys
import os
import time
import math

# path to run file                     - 1 arg
# names of tests without _mat and _det - other args

def take_answer(file_name):
    f = open(file_name, "r")
    answer = float(f.read())
    return answer

def take_det(run_file_name, matrix_file):
    os.system("./" + run_file_name + " < " + matrix_file + " > out")
    f = open("out", "r")
    det_act = float(f.read())
    os.system("rm out")
    return det_act

def main():
    for i in range(2, len(sys.argv)):
        det_test = take_answer(sys.argv[i] + "_det")
        print("Start run...")
        det_act  = take_det(sys.argv[1], sys.argv[i] + "_mat")
        print("Finish run... time:" + str(time.perf_counter()))
        abs_error = abs(det_act - det_test)
        string_to_put = ""
        if math.isclose(abs(det_act), 0, rel_tol=1e-2):
            string_to_put = "abs_error = " + str(abs_error)
        else:
            string_to_put = "rel_error = " + str(abs_error/abs(det_act))
        if math.isclose(det_act, det_test, rel_tol=1e-2) == True:
            print(str(i - 1) + ": Succes: " + string_to_put)
        else:
            print(str(i - 1) + ": Error: " + string_to_put)
            print("det from run file: " + str(det_act) + "\ndet form test: " + str(det_test))

main()

