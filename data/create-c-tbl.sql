-----------------
-- c table: connected component details
--
CREATE TABLE c (
    runId               varchar(256) NOT NULL REFERENCES s(runId),
    iterationNr         integer,
    ccSize              integer,
    ccCount             integer
);
