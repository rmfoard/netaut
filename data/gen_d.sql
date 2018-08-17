SELECT
    runId,
    avgClustCoef,
    diameter,
    effDiameter90Pctl,
    finInDegreeEntropy as inDegreeEntropy,
    nrActualIterations-1 as iterationNr,
    nrCcSizes,
    nrCcs,
    nrClosedTriads,
    nrInDegrees,
    nrNodes,
    nrOpenTriads,
    nrOutDegrees,
    finOutDegreeEntropy as outDegreeEntropy
INTO d FROM runs;
ALTER TABLE d ADD CONSTRAINT d_ref_s_runId FOREIGN KEY (runId) REFERENCES s(runId);
