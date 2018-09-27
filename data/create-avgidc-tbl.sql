select
    machinetype,
    initnrnodes,
    indegreesize,
    avg(indegreecount) as avgidc
into avgidc
from sd, i
where
    sd.runid = i.runid
group by
    machinetype,
    initnrnodes,
    indegreesize;
