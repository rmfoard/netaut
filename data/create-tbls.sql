-----------------
-- s table: simulator runs
--
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
-----------------
-- d table: run iteration details
--
-----------------
CREATE TABLE d (
    runId               varchar(256) NOT NULL REFERENCES s(runid),
    avgClustCoef        double precision,
    diameter            integer,
    effDiameter90Pctl   double precision,
    inDegreeEntropy     double precision,
    iterationNr         integer,
    nrCcSizes           integer,
    nrCcs               integer,
    nrClosedTriads      bigint,
    nrInDegrees         integer,
    nrNodes             integer,
    nrOpenTriads        bigint,
    nrOutDegrees        integer,
    outDegreeEntropy    double precision
);
-----------------
-- c table: connected component details
--
CREATE TABLE c (
    runId               varchar(256) NOT NULL REFERENCES s(runId),
    ccSize              integer,
    ccCount             integer
);
-----------------
-- t table: triad counts
--
-----------------
CREATE TABLE t (
    runId               varchar(256)  NOT NULL REFERENCES s(runId),
    triad               integer,
    triadCount          integer
);
-----------------
-- i table: in-degree details
--
CREATE TABLE i (
    runId               varchar(256) NOT NULL REFERENCES s(runId),
    inDegreeSize        integer,
    inDegreeCount       integer
);
-----------------
-- o table: out-degree details
--
-----------------
CREATE TABLE o (
    runId           varchar(256) NOT NULL REFERENCES s(runId),
    outDegreeSize   integer,
    outDegreeCount  integer
);
