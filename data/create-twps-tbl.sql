drop table if exists tmp;
select s.rulenr, t.runid, t.triad, t.triadcount into tmp from s, t where s.runid = t.runid;
select runid, triad, triadcount, mod(rulenr / cast(72^triad as bigint), 72) as rulepart into twps from tmp;
drop table tmp;
