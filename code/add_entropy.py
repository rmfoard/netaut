import json
import math
import sys


class Entropier (object):

    def __init__(self, inDegreeCount):
        self.total_freq = 0.0
        self.deg_to_freq = {}
        for pair in inDegreeCount:
            self.total_freq += pair[1]
            self.deg_to_freq[pair[0]] = pair[1]

    def entropy(self):
        sum = 0.0
        for k, v in self.deg_to_freq.iteritems():
            pk = v / self.total_freq
            sum += pk * math.log(pk, 2)
        return -sum


def main():
    if len(sys.argv) < 3:
        print "usage: add_entropy in_jsonfile out_jsonfile"
        sys.exit(1)

    with open(sys.argv[1], 'r') as in_f:
        with open(sys.argv[2], 'w') as out_f:
            for line in in_f:
                dct = json.loads(line)
                e = Entropier(dct['inDegreeCount'])
                dct['degListEntropy'] = e.entropy()
                out_f.write(json.dumps(dct, sort_keys=True) + '\n')


if __name__ == '__main__':
    main()
    sys.exit(0)
