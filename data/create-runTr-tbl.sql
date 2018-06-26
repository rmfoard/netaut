CREATE TABLE runTr (
    runId               varchar(256)  NOT NULL REFERENCES runs(runId),
    triad               integer,
    triadCount          integer
);
