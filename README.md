# My class Matrix
My classes MatrixArithmetic and MatrixContainer.
Base class is MatrixContainer: he has responsibitility to save memmory and give acces to two dimensional array.
Derived class MatrixArithmetic: he has resposibiility to make arithmetical operations with matrix like summary, difference, determinant, inverse and other.

# How to build?

```
cmake -B build/
cmake --build unit_tests              # unit tests
cmake --build task                    # task
```

# How to test?

You have example of build unit_tests. To test determinat u can do this:

Generate test like this:
```
cd test
chmod "+x" testgen.py      # to made script from testgen.py

./testgen.py [SIZE] [NUM] [NAME] 
```
SIZE - side of square matrix

NUM - max number on diagonal od upper triangular version of this matrix

NAME - in file NAME_mat script will put SIZE and numbers of matrix in row order and value of determinat in NAME_det

And test like this:
```
cd test
chmod "+x" det_test.py

./det_test.py [RUN_FILE] [FILES] 
```
RUN_FILE - run file with task

FILES - sequence od NAMEs to test