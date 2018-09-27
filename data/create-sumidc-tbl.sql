select
    machinetype,
    initnrnodes,
    indegreesize,
    sum(indegreecount) as nodecount
into sumidc
from sd, i
where
    sd.runid = i.runid
group by
    machinetype,
    initnrnodes,
    indegreesize;
