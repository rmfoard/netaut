import csv
import json
import sys


def main():
    if len(sys.argv) != 2:
        print "usage: python json_to_csv.py <json file base name (w/o '.json')>"
        sys.exit(1)
    basename = sys.argv[1]

    # List of fields to be included in the main csv file (in order)
    main_names = [
        'allowSelfEdges',
        'avgClustCoef',
        'cycleCheckDepth',
        'cycleLength',
        'diameter',
        'effDiameter90Pctl',
        'hashCollisions',
        'machineType',
        'maxIterations',
        'multiEdgesAvoided',
        'nrActualIterations',
        'nrCcSizes',
        'nrCcs',
        'nrClosedTriads',
        'nrInDegrees',
        'nrNodes',
        'nrOpenTriads',
        'randseed',
        'ruleNr',
        'runId',
        'runTimeMs',
        'selfEdgesAvoided',
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

    tr_names = [
        'runId',
        'triad',
        'triadCount'
    ]
    tr_dct = {}

    # Pass the json file writing the four derived csv's.
    with open(basename + '.json', 'r') as jsonfile:
        with open(basename + '.csv', 'w') as main_f:
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

                            tr_dct['runId'] = run_id
                            for triad in xrange(6):
                                tr_dct['triad'] = triad
                                tr_dct['triadCount'] = main_dct['triadOccurrences'][triad]
                                tr_writer.writerow(tr_dct)

if __name__ == '__main__':
    main()
