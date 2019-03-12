import csv
import json
import sys


def main():
    if len(sys.argv) != 2:
        print "usage: python gjson_to_csv.py <json file base name (w/o '.json'or '_suffixes')>"
        sys.exit(1)
    basename = sys.argv[1]

    # List of fields to be included in the main (_gs.)csv file (in order)
    gs_names = [
        'grunId',
        'cumFitnessExp',
        'cycleCheckDepth',
        'initNrNodes',
        'machineType',
        'maxGenerations',
        'maxIterations',
        'nrDistinctRules',
        'nrFitRules',
        'nrTotRules',
        'poolSize',
        'probMutation',
        'randSeed',
        'statMax',
        'statMin',
        'statMinuend',
        'statName',
        'tapeStructure',
        'termAvgFitness',
        'topoStructure',
        'version'
    ]

    gp_names = [
        'grunId',
        'fitness',
        'generationNr',
        'ruleNr'
    ]

    # Pass the main (_s.) json file writing _gs.csv (run parameters and stats)
    with open(basename + '_s.json', 'r') as jsonfile:
        with open(basename + '_gs.csv', 'w') as gs_f:
            gs_writer = csv.DictWriter(gs_f, fieldnames=gs_names, extrasaction='ignore')
            gs_writer.writeheader()

            line_number = 1
            for line in jsonfile:
                line_number += 1
                if len(line) < 2:
                    continue
                gs_dct = json.loads(line)
                gs_writer.writerow(gs_dct)

    # Pass the accumulative rulepath file writing _gp.csv
    with open(basename + '_p.txt', 'r') as rulePathFile:
        with open(basename + '_gp.csv', 'w') as gp_f:
            gp_writer = csv.DictWriter(gp_f, fieldnames=gp_names, extrasaction='ignore')
            gp_writer.writeheader()

            line_number = 1
            for line in rulePathFile:
                line_number += 1
                if len(line) < 2:
                    continue
                gp_dct = {}
                fields = line[:-1].split(' ')
                gp_dct['grunId'] = fields[0]
                gp_dct['generationNr'] = fields[1]
                gp_dct['ruleNr'] = fields[2]
                gp_dct['fitness'] = fields[3]
                gp_writer.writerow(gp_dct)


if __name__ == '__main__':
    main()
