import csv
import json
import sys


def main():
    if len(sys.argv) != 3:
        print "usage: python json_to_csv.py <jsonfile> <csvfile>"
        sys.exit(1)

    # List of fields to be included in the csv file (in order)
    fieldnames = [
        'allowSelfEdges',
        'avgClustCoef',
        'cycleCheckDepth',
        'cycleLength',
        'diameter',
        'effDiameter90Pctl',
        'hashCollisions',
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

    # Pass the json file writing csv rows.
    with open(sys.argv[1], 'r') as jsonfile:
        with open(sys.argv[2], 'w') as csvfile:
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames, extrasaction='ignore')
            writer.writeheader()
            for line in jsonfile:
                dct = json.loads(line)
                writer.writerow(dct)


if __name__ == '__main__':
    main()
