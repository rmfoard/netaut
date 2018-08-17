SELECT
    runId,
    cycleCheckDepth,
    cycleLength,
    hashCollisions,
    machineType,
    maxIterations,
    nrActualIterations as nrIterations,
    randseed,
    ruleNr,
    runTimeMs,
    tapePctBlack,
    tapeStructure,
    topoStructure,
    version
INTO s FROM runs;
ALTER TABLE s ADD PRIMARY KEY (runId);
