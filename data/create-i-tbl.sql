-----------------
-- i table: in-degree details
--
-----------------
CREATE TABLE i (
    runId               varchar(256) NOT NULL REFERENCES s(runId),
    iterationNr         integer,
    inDegreeSize        integer,
    inDegreeCount       integer
);
