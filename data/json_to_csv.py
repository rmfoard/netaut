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
    main_names = [
        'runId',
        'cycleCheckDepth',
        'cycleLength',
        'hashCollisions',
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

    cc_names = [
        'runId',
        'ccSize',
        'ccCount'
    ]
    cc_dct = {}

    id_names = [
        'runId',
        'inDegree',
        'inDegreeCount'
    ]
    id_dct = {}

    od_names = [
        'runId',
        'outDegree',
        'outDegreeCount'
    ]
    od_dct = {}

    tr_names = [
        'runId',
        'triad',
        'triadCount'
    ]
    tr_dct = {}

    it_names = [
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

    # Pass the main json file writing the five derived csv files.
    with open(basename + '_summ.json', 'r') as jsonfile:
        with open(basename + '_summ.csv', 'w') as main_f:
            main_writer = csv.DictWriter(main_f, fieldnames=main_names, extrasaction='ignore')
            main_writer.writeheader()

            with open(basename + '_cc.csv', 'w') as cc_f:
                cc_writer = csv.DictWriter(cc_f, fieldnames=cc_names, extrasaction='ignore')
                cc_writer.writeheader()

                with open(basename + '_id.csv', 'w') as id_f:
                    id_writer = csv.DictWriter(id_f, fieldnames=id_names, extrasaction='ignore')
                    id_writer.writeheader()

                    with open(basename + '_tr.csv', 'w') as tr_f:
                        tr_writer = csv.DictWriter(tr_f, fieldnames=tr_names, extrasaction='ignore')
                        tr_writer.writeheader()

                        with open(basename + '_od.csv', 'w') as od_f:
                            od_writer = csv.DictWriter(od_f, fieldnames=od_names, extrasaction='ignore')
                            od_writer.writeheader()

                            line_number = 1
                            for line in jsonfile:
                                line_number += 1
                                if len(line) < 2:
                                    continue
                                main_dct = json.loads(line)
                                run_id = main_dct['runId']
                                main_writer.writerow(main_dct)

                                cc_dct['runId'] = run_id
                                for pair in main_dct['ccSizeCount']:
                                    cc_dct['ccSize'] = pair[0]
                                    cc_dct['ccCount'] = pair[1]
                                    cc_writer.writerow(cc_dct)

                                id_dct['runId'] = run_id
                                for pair in main_dct['inDegreeCount']:
                                    id_dct['inDegree'] = pair[0]
                                    id_dct['inDegreeCount'] = pair[1]
                                    id_writer.writerow(id_dct)

                                od_dct['runId'] = run_id
                                for pair in main_dct['outDegreeCount']:
                                    od_dct['outDegree'] = pair[0]
                                    od_dct['outDegreeCount'] = pair[1]
                                    od_writer.writerow(od_dct)

                                tr_dct['runId'] = run_id
                                for triad in xrange(NR_TRIAD_STATES):
                                    tr_dct['triad'] = triad
                                    tr_dct['triadCount'] = main_dct['triadOccurrences'][triad]
                                    tr_writer.writerow(tr_dct)

    # Pass the iteration detail json file writing a csv file.
    with open(basename + '_iter.json', 'r') as jsonfile:
        with open(basename + '_iter.csv', 'w') as it_f:
            it_writer = csv.DictWriter(it_f, fieldnames=it_names, extrasaction='ignore')
            it_writer.writeheader()
            line_number = 1
            for line in jsonfile:
                line_number += 1
                if len(line) < 2:
                    continue
                it_dct = json.loads(line)
                it_writer.writerow(it_dct)


if __name__ == '__main__':
    main()
