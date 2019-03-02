-----------------
-- gs table: genetic search run summary
--
-----------------
CREATE TABLE gs (
    grunId              varchar(256) NOT NULL PRIMARY KEY,
    cycleCheckDepth     integer,
    cycleLength         integer,
    initNrNodes         integer,
    machineType         varchar(4),
    maxIterations       integer,
    nrIterations        integer,
    randseed            integer,
    tapeStructure       varchar(32),
    topoStructure       varchar(32),
    poolSize            integer,
    probMutation        double precision,
    maxGenerations      integer,
    nrGenerations       integer,
    cumFitnessExp       double precision,
    statName            varchar(32),
    statMin             double precision,
    statMax             double precision,
    statMinuend         double precision,
    nrTotRules          integer,
    nrDistinctRules     integer,
    nrFitRules          integer
);

-----------------
-- gg table: pool rules for each generation
--
-----------------
CREATE TABLE gg (
    grunId              varchar(256) NOT NULL REFERENCES gs(grunid),
    generationNr        integer,
    ruleNr              bigint,
    statValue           double precision
);
