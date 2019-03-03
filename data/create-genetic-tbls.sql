-----------------
-- gs table: genetic search run summary
--
-----------------
CREATE TABLE gs (
    grunId               varchar(256) NOT NULL PRIMARY KEY,
    cumFitnessExp       double precision,
    cycleCheckDepth     integer,
    initNrNodes         integer,
    machineType         varchar(4),
    maxGenerations      integer,
    maxIterations       integer,
    nrDistinctRules     integer,
    nrFitRules          integer,
    nrTotRules          integer,
    poolSize            integer,
    probMutation        double precision,
    randseed            integer,
    statMax             double precision,
    statMin             double precision,
    statMinuend         double precision,
    statName            varchar(32),
    tapeStructure       varchar(32),
    termAvgFitness      double precision,
    topoStructure       varchar(32)
);

-----------------
-- gp table: accumulative rule path
--
-----------------
CREATE TABLE gp (
    grunId               varchar(256) NOT NULL,
    fitness             double precision,
    generationNr        integer,
    ruleNr              bigint
);
