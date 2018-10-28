drop table if exists tmp;
select
    t.runid,
    mod(rulenr / cast(72^triad as bigint), 72) as rulepart,
    triadcount,
    (cast(triadcount as float) / tottriads) * 100.0 as triadpct
into tmp
from t, triadtots
where t.runid = triadtots.runid;
select
    pp.runid,
    sd.machinetype,
    sd.initnrnodes,
    sd.outcome,
    pp.rulepart,
    pp.triadcount,
    pp.triadpct
into partpcts
from tmp as pp, sd
where pp.runid = sd.runid;
drop table tmp;
