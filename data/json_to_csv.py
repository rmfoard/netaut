import csv
import json
import sys


def main():
    if len(sys.argv) != 2:
        print "usage: python json_to_csv.py <json file base name (w/o '.json'or '_suffixes')>"
        sys.exit(1)
    basename = sys.argv[1]

    NR_NODE_STATES = 2
    NR_TRIAD_STATES = NR_NODE_STATES * NR_NODE_STATES * NR_NODE_STATES

    # List of fields to be included in the main csv file (in order)
    s_names = [
        'runId',
        'cycleCheckDepth',
        'cycleLength',
        'hashCollisions',
        'initNrNodes',
        'machineType',
        'maxIterations',
        'nrIterations',
        'randseed',
        'ruleNr',
        'runTimeMs',
        'tapePctBlack',
        'tapeStructure',
        'topoStructure',
        'version'
    ]

    c_names = [
        'runId',
        'iterationNr',
        'ccSize',
        'ccCount'
    ]
    c_dct = {}

    i_names = [
        'runId',
        'iterationNr',
        'inDegree',
        'inDegreeCount'
    ]
    i_dct = {}

    o_names = [
        'runId',
        'iterationNr',
        'outDegree',
        'outDegreeCount'
    ]
    o_dct = {}

    t_names = [
        'runId',
        'triad',
        'triadCount'
    ]
    t_dct = {}

    d_names = [
        'runId',
        'avgClustCoef',
        'diameter',
        'effDiameter90Pctl',
        'inDegreeEntropy',
        'iterationNr',
        'nrCcSizes',
        'nrCcs',
        'nrClosedTriads',
        'nrInDegrees',
        'nrNodes',
        'nrOpenTriads',
        'nrOutDegrees',
        'outDegreeEntropy'
    ]

    # Pass the main (_s.) json file writing s.csv (run parameters and stats) and t.csv (triad counts)
    with open(basename + '_s.json', 'r') as jsonfile:
        with open(basename + '_s.csv', 'w') as s_f:
            s_writer = csv.DictWriter(s_f, fieldnames=s_names, extrasaction='ignore')
            s_writer.writeheader()

            with open(basename + '_t.csv', 'w') as t_f:
                t_writer = csv.DictWriter(t_f, fieldnames=t_names, extrasaction='ignore')
                t_writer.writeheader()

                line_number = 1
                for line in jsonfile:
                    line_number += 1
                    if len(line) < 2:
                        continue
                    s_dct = json.loads(line)
                    run_id = s_dct['runId']
                    s_writer.writerow(s_dct)

                    t_dct['runId'] = run_id
                    for triad in xrange(NR_TRIAD_STATES):
                        t_dct['triad'] = triad
                        t_dct['triadCount'] = s_dct['triadOccurrences'][triad]
                        t_writer.writerow(t_dct)

    # Pass the iteration detail (_d.) json file writing d, c, i, and o .csv files.
    with open(basename + '_d.json', 'r') as jsonfile:
        with open(basename + '_d.csv', 'w') as d_f:
            d_writer = csv.DictWriter(d_f, fieldnames=d_names, extrasaction='ignore')
            d_writer.writeheader()

            with open(basename + '_c.csv', 'w') as c_f:
                c_writer = csv.DictWriter(c_f, fieldnames=c_names, extrasaction='ignore')
                c_writer.writeheader()

                with open(basename + '_i.csv', 'w') as i_f:
                    i_writer = csv.DictWriter(i_f, fieldnames=i_names, extrasaction='ignore')
                    i_writer.writeheader()

                    with open(basename + '_o.csv', 'w') as o_f:
                        o_writer = csv.DictWriter(o_f, fieldnames=o_names, extrasaction='ignore')
                        o_writer.writeheader()

                        line_number = 1
                        for line in jsonfile:
                            line_number += 1
                            if len(line) < 2:
                                continue
                            d_dct = json.loads(line)
                            run_id = d_dct['runId']
                            iteration_nr = d_dct['iterationNr']
                            d_writer.writerow(d_dct)

                            c_dct['runId'] = run_id
                            for pair in d_dct['ccSizeCount']:
                                c_dct['iterationNr'] = iteration_nr
                                c_dct['ccSize'] = pair[0]
                                c_dct['ccCount'] = pair[1]
                                c_writer.writerow(c_dct)

                            i_dct['runId'] = run_id
                            for pair in d_dct['inDegreeCount']:
                                i_dct['iterationNr'] = iteration_nr
                                i_dct['inDegree'] = pair[0]
                                i_dct['inDegreeCount'] = pair[1]
                                i_writer.writerow(i_dct)

                            o_dct['runId'] = run_id
                            for pair in d_dct['outDegreeCount']:
                                o_dct['iterationNr'] = iteration_nr
                                o_dct['outDegree'] = pair[0]
                                o_dct['outDegreeCount'] = pair[1]
                                o_writer.writerow(o_dct)


if __name__ == '__main__':
    main()
