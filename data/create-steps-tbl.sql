select mod(s.rulenr, 65536) as ruleid, d.iterationnr, d.nrnodes into steps from s, d where s.runid = d.runid order by ruleid, iterationnr;
