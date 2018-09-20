alter table c rename to tmpc;
select s.runid, nriterations-1 as iterationnr, ccsize, cccount into c
from s, tmpc
where s.runid = tmpc.runid;
--drop table tmpc;
--
alter table i rename to tmpi;
select s.runid, nriterations-1 as iterationnr, indegreesize, indegreecount into i
from s, tmpi
where s.runid = tmpi.runid;
--drop table tmpi;
--
alter table o rename to tmpo;
select s.runid, nriterations-1 as iterationnr, outdegreesize, outdegreecount into o
from s, tmpo
where s.runid = tmpo.runid;
--drop table tmpo;
