CREATE TABLE c (
    runId               varchar(256) NOT NULL REFERENCES s(runId),
    ccSize              integer,
    ccCount             integer
);
