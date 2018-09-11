import sys
from sklearn import svm, metrics


def main():

    if len(sys.argv) != 3:
        print 'usage: python ' + sys.argv[0] + ' <datafile> <targetfile>'
        sys.exit(1)
    print 'data from: ' + sys.argv[1] + ', targets from: ' + sys.argv[2]

    # Read the data.
    n_samples = 0
    data = []
    with open(sys.argv[1], "r") as df:
        for line in df:
            parts = []
            for token in line.split():
               parts.append(int(token))
            data.append(parts)
            n_samples += 1

    # Read the "targets."
    targets = []
    with open(sys.argv[2]) as tf:
        for line in tf:
            targets.append(int(line[:-1]))

    # Create a classifier: a support vector classifier
    classifier = svm.SVC(C=10, gamma=0.01)

    # We learn the digits on the first half of the digits
    classifier.fit(data[:n_samples // 2], targets[:n_samples // 2])

    # Now predict the value of the digit on the second half:
    expected = targets[n_samples // 2:]
    predicted = classifier.predict(data[n_samples // 2:])

    print 'predicted:'
    print predicted

    print("Classification report for classifier %s:\n%s\n"
          % (classifier, metrics.classification_report(expected, predicted)))
    print("Confusion matrix:\n%s" % metrics.confusion_matrix(expected, predicted))

    sys.exit(0)


if __name__ == '__main__':
    main()
