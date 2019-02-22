import sys

def main():
    if len(sys.argv) != 2:
        print "usage: python {} <wolfram rule nr>".format(sys.argv[0])
        sys.exit(1)

    wolfram_rule = int(sys.argv[1])
    divisor = 1
    wolfram_binary = ''
    while divisor <= 128:
        wolfram_binary += str((wolfram_rule / divisor) % 2)
        divisor *= 2

    LEDGE = 0
    REDGE = 3
    c_rule = 0
    multiplier = 72 * 72 * 72 * 72 * 72 * 72 * 72
    for d in wolfram_binary:
        c_rule += (((LEDGE * 6) + REDGE) * 2 + (1 - int(d))) * multiplier
        multiplier /= 72
    print str(c_rule)

    sys.exit(0)


if __name__ == '__main__':
    main()
