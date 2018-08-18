SELECT
    runId,
    cycleCheckDepth,
    cycleLength,
    hashCollisions,
    machineType,
    maxIterations,
    nrActualIterations as nrIterations,
    nrNodes as initNrNodes,
    randseed,
    ruleNr,
    runTimeMs,
    tapePctBlack,
    tapeStructure,
    topoStructure,
    version
INTO s FROM runs;
ALTER TABLE s ADD PRIMARY KEY (runId);
