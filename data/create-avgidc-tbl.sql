select
    machinetype,
    initnrnodes,
    indegreesize,
    avg(indegreecount) as avgidc,
    variance(indegreecount) as varidc
into avgidc
from sd, i
where
    sd.runid = i.runid
group by
    machinetype,
    initnrnodes,
    indegreesize;
