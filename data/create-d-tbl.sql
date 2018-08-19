CREATE TABLE d (
    runId               varchar(256) NOT NULL PRIMARY KEY,
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
