import csv
import json
import sys


def main():
    if len(sys.argv) != 3:
        print "usage: python json_to_csv.py <jsonfile> <csvfile>"
        sys.exit(1)

    # List of fields to be included in the csv file (in order)
    fieldnames = [
        'runId',
        'ruleNr',
        'ruleText',
        'multiEdges',
        'selfEdges',
        'nrNodes',
        'nrIterations',
        'nrCcSizes',
        'nrCcs',
        'nrInDegrees',
        'diameter',
        'effDiameter90Pctl',
        'nrClosedTriads',
        'nrOpenTriads',
        'avgClustCoef',
        'version'
    ]

    # Pass the json file writing csv rows.
    with open(sys.argv[1], 'r') as jsonfile:
        with open(sys.argv[2], 'w') as csvfile:
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames, extrasaction='ignore')
            writer.writeheader()
            for line in jsonfile:
                if len(line) > 1:
                    dct = json.loads(line)
                    if 'noMultiEdges' in dct:  # need to invert legacy item?
                        dct['multiEdges'] = 1 - dct['noMultiEdges']
                    writer.writerow(dct)


if __name__ == '__main__':
    main()
