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
        'randseed',
        'statMax',
        'statMin',
        'statMinuend',
        'statName',
        'tapeStructure',
        'topoStructure'
    ]

    gg_names = [
        'grunId',
        'generationNr',
        'ruleNr',
        'statValue'
    ]

    # Pass the main (_s.) json file writing _gs.csv (run parameters and stats)
    with open(basename + '_s.json', 'r') as jsonfile:
        with open(basename + '_s.csv', 'w') as gs_f:
            gs_writer = csv.DictWriter(gs_f, fieldnames=gs_names, extrasaction='ignore')
            gs_writer.writeheader()

            line_number = 1
            for line in jsonfile:
                line_number += 1
                if len(line) < 2:
                    continue
                gs_dct = json.loads(line)
                grun_id = gs_dct['grunId']
                gs_writer.writerow(gs_dct)

    # Pass the by-generation pool contents file writing _gg.csv
    with open(basename + '_g.txt', 'r') as genPoolFile:
        with open(basename + '_gg.csv', 'w') as gg_f:
            gg_writer = csv.DictWriter(gg_f, fieldnames=gg_names, extrasaction='ignore')
            gg_writer.writeheader()

            line_number = 1
            for line in genPoolFile:
                line_number += 1
                if len(line) < 2:
                    continue
                gg_dct = {}
                fields = line.split(' ')
                gg_dct['grunId'] = fields[0]
                gg_dct['generationNr'] = fields[1]
                gg_dct['rulenr'] = fields[2]
                gg_dct['statValue'] = fields[3]
                gg_writer.writerow(gg_dct)


if __name__ == '__main__':
    main()
