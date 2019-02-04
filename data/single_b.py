def compose_rule(part_list):
    rule = part_list[0]
    for exp_nr in xrange(7):
        rule = rule * 72 + part_list[exp_nr + 1]
    return rule


def main():
    pl = [1, 2, 3, 4, 5, 6, 7, 8]
    print 'rule(' + str(pl) + '): ' + str(compose_rule(pl))


if __name__ == '__main__':
    main()
