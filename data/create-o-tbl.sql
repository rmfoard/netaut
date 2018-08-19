CREATE TABLE runId (
    runId           varchar(256) NOT NULL REFERENCES runs(runId),
    outDegreeSize   integer,
    outDegreeCount  integer
);
