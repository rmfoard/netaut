CREATE TABLE t (
    runId               varchar(256)  NOT NULL REFERENCES s(runId),
    triad               integer,
    triadCount          integer
);
