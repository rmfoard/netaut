CREATE TABLE o (
    runId           varchar(256) NOT NULL REFERENCES s(runId),
    outDegreeSize   integer,
    outDegreeCount  integer
);
