-----------------
-- o table: out-degree details
--
-----------------
CREATE TABLE o (
    runId           varchar(256) NOT NULL REFERENCES s(runId),
    iterationNr     integer,
    outDegreeSize   integer,
    outDegreeCount  integer
);
