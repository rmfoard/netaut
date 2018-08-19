CREATE TABLE runId (
    runId               varchar(256) NOT NULL REFERENCES runs(runId),
    inDegreeSize        integer,
    inDegreeCount       integer
);
