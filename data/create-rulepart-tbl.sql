---------------
-- rulepart table: relates rulenr to its 8 rule-parts
--
---------------
CREATE TABLE crulepart (
    rulenr      bigint NOT NULL PRIMARY KEY,
    rp7        integer,
    rp6        integer,
    rp5        integer,
    rp4        integer,
    rp3        integer,
    rp2        integer,
    rp1        integer,
    rp0        integer
);
