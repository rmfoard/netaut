"""Program to generate a list of rules that preserve a single black node

Each rule number appears on a line followed by '1' (intended for use
as a random number generator seed when feeding the list to scripts that
expect it).
"""

import sys


def compose_rule(part_list):
    rule = part_list[0]
    for exp_nr in xrange(7):
        rule = rule * 72 + part_list[exp_nr + 1]
    return rule


def main():
    node001 = [1, 0, 0]
    node010 = [0, 1, 0]
    node100 = [0, 0, 1]

    nr_rules = 0
    for pass_nr in [0, 1, 2]:
        for topo000 in xrange(36):
            for topo001 in xrange(36):
                for topo010 in xrange(36):
                    for topo100 in xrange(36):
                        part000 = topo000 * 2 + 0
                        part001 = topo001 * 2 + node001[pass_nr]
                        part010 = topo010 * 2 + node010[pass_nr]
                        part011 = 0
                        part100 = topo100 * 2 + node100[pass_nr]
                        part101 = 0
                        part110 = 0
                        part111 = 0
                        print str(compose_rule([part111, part110, part101, part100, part011, part010, part001, part000])) + ' 1'

                        nr_rules += 1


if __name__ == '__main__':
    main()
