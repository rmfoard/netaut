import math

def nCr(n,r):
    f = math.factorial
    return f(n) / f(r) / f(n-r)


def p(k, n):
    return nCr(2*n - 1, k-2) * pow(1.0/n, k-2) * pow((n-1)/float(n), 2*n - k + 1)


if __name__ == '__main__':
    print nCr(6,2)
    for k in [3, 4, 5, 6, 7, 8, 9, 10]:
        print 'p(' + str(k) + ') => ' + str(p(k, 256))
