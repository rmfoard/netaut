select * into idesummary from
    (select s.runid, 'I' as entrytype, initnrnodes, d.indegreeentropy from s, d where s.runid = d.runid and iterationnr = 0) as a1
union
    (select s.runid, machinetype as entrytype, initnrnodes, d.indegreeentropy from s, d where s.runid = d.runid and iterationnr <> 0);
