# Enumerate all Machine C ruletexts in which all rule-parts are the same.


def main():
    edge_dests = ['L', 'LL', 'LR', 'R', 'RL', 'RR']
    node_states = ['B', 'W']

    for lact in edge_dests:
        for ract in edge_dests:
            for nstate in node_states:
                rule_part = lact + ',' + ract + ',' + nstate
                ruletext = ''
                for part in xrange(8):
                    ruletext += rule_part
                    if part != 7:
                        ruletext += ';'
                print ruletext


if __name__ == '__main__':
    main()
