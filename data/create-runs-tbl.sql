CREATE TABLE runs (
    allowSelfEdges      integer,
    avgClustCoef        double precision,
    cycleCheckDepth     integer,
    cycleLength         integer,
    diameter            integer,
    effDiameter90Pctl   double precision,
    hashCollisions      integer,
    maxIterations       integer,
    multiEdgesAvoided   integer,
    nrActualIterations  integer,
    nrCcSizes           integer,
    nrCcs               integer,
    nrClosedTriads      bigint,
    nrInDegrees         integer,
    nrNodes             integer,
    nrOpenTriads        bigint,
    randseed            integer,
    ruleNr              bigint,
    runId               varchar(256) NOT NULL PRIMARY KEY,
    runTimeMs           integer,
    selfEdgesAvoided    integer,
    tapePctBlack        integer,
    tapeStructure       varchar(32),
    topoStructure       varchar(32),
    version             varchar(64)
);
