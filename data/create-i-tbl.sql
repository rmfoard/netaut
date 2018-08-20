CREATE TABLE i (
    runId               varchar(256) NOT NULL REFERENCES s(runId),
    inDegreeSize        integer,
    inDegreeCount       integer
);
