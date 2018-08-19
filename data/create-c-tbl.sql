CREATE TABLE runCc (
    runId               varchar(256) NOT NULL REFERENCES runs(runId),
    ccSize              integer,
    ccCount             integer
);
