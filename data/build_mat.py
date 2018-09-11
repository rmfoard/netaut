import sys
from math import sqrt
import numpy as np


class Matrix:

    def __init__(self):
        pass

    def read_mat(self, filename):
        # Count rows and columns.
        self.nrows = 0
        self.ncols = 0
        valid = True
        with open(filename, 'r') as f:
            for line in f:
                nc = 0
                for ns in line.split():
                    nc += 1
                if self.ncols == 0:
                    self.ncols = nc
                elif self.ncols != nc:
                    print 'differing number of columns at row ' + str(self.nrows)
                    valid = False
                self.nrows += 1

        if not valid:
            sys.exit(1)

        # Allocate an array.
        self.mat = np.ndarray((self.nrows, self.ncols))

        # Read values.
        with open(filename, 'r') as f:
            row = 0
            for line in f:
                col = 0
                for ns in line.split():
                    self.mat[row, col] = float(ns)
                    col += 1
                row += 1

        if row != self.nrows:
            print 'row count error'
            sys.exit(1)

    def write(self, filename):
        with open(filename, 'w') as f:
            for i in xrange(self.nrows):
                line = ''
                for j in xrange(self.ncols):
                    line += str(self.e(i, j))
                    if j != self.ncols - 1:
                        line += ' '
                f.write(line + '\n')

    def allocate(self, nrows, ncols):
        self.nrows = nrows
        self.ncols = ncols
        self.mat = np.ndarray((nrows, ncols))

    def e(self, i, j):
        if j <= i:
            return self.mat[i, j]
        else:
            return self.mat[j, i]


def dist(vs, v1, v2):
    sum = 0.0
    for elt in xrange(vs.ncols):
        sum += (vs.mat[v1, elt] - vs.mat[v2, elt]) ** 2
    return sqrt(sum)


def main():
    if len(sys.argv) != 3:
        print "usage: {} <vector list> <distance matrix>".format(sys.argv[0])
        sys.exit(1)

    # Read vectors into the rows of matrix 'vs'.
    vs = Matrix()
    vs.read_mat(sys.argv[1])
    print str(vs.nrows) + ' vectors, ' + str(vs.ncols) + ' long'

    # Fill (half) the inter-vector distance matrix 'dm'
    dm = Matrix()
    dm.allocate(vs.nrows, vs.nrows)
    for v1 in xrange(vs.nrows):
        dm.mat[v1, v1] = 0.0 # diagonal
        for v2 in xrange(v1):
            dm.mat[v1, v2] = dist(vs, v1, v2)

    # Write the result.
    for i in xrange(vs.nrows):
        for j in xrange(vs.nrows):
            print '[' + str(i) + ', ' + str(j) + '] = ' + str(dm.e(i, j))
    dm.write(sys.argv[2])

    sys.exit(0)


if __name__ == '__main__':
    main()
