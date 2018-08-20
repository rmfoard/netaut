CREATE TABLE s (
    runId               varchar(256) NOT NULL PRIMARY KEY,
    cycleCheckDepth     integer,
    cycleLength         integer,
    hashCollisions      integer,
    initNrNodes         integer,
    machineType         varchar(4),
    maxIterations       integer,
    nrIterations        integer,
    randseed            integer,
    ruleNr              bigint,
    runTimeMs           integer,
    tapePctBlack        integer,
    tapeStructure       varchar(32),
    topoStructure       varchar(32),
    version             varchar(64)
);
