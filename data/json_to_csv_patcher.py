import csv
import json
import sys


def main():
    if len(sys.argv) != 2:
        print "usage: python json_to_csv.py <json file base name (w/o '.json')>"
        sys.exit(1)
    basename = sys.argv[1]

    NR_NODE_STATES = 2
    NR_TRIAD_STATES = NR_NODE_STATES * NR_NODE_STATES * NR_NODE_STATES

    # List of fields to be included in the 'runtr' csv file (in order)
    tr_names = [
        'runId',
        'triad',
        'triadCount'
    ]
    tr_dct = {}

    # Pass the json file writing the patch csv.
    with open(basename + '.json', 'r') as jsonfile:
        with open(basename + '_tr.csv', 'w') as tr_f:
            tr_writer = csv.DictWriter(tr_f, fieldnames=tr_names, extrasaction='ignore')
            tr_writer.writeheader()

            for line in jsonfile:
                if len(line) < 2:
                    continue
                main_dct = json.loads(line)
                run_id = main_dct['runId']

                tr_dct['runId'] = run_id
                for triad in xrange(NR_TRIAD_STATES):
                    if triad > 5:
                        tr_dct['triad'] = triad
                        tr_dct['triadCount'] = main_dct['triadOccurrences'][triad]
                        tr_writer.writerow(tr_dct)

if __name__ == '__main__':
    main()
