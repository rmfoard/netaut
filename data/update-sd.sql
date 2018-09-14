select s.runid, sum(cccount * ccsize) into finnrnodes from s, c where s.runid = c.runid group by s.runid;
alter table finnrnodes rename column sum to nrnodes;
create index finnrnodes_idx_runid on finnrnodes(runid);
create index sd_idx_runid on sd(runid);
update sd set nrnodes = finnrnodes.nrnodes from finnrnodes where sd.runid = finnrnodes.runid;
drop table finnrnodes;
--
select runid, max(indegreesize) into tmpmid from i group by runid;
alter table sd add column maxindegree integer;
update sd set maxindegree = max from tmpmid where sd.runid = tmpmid.runid;
drop table tmpmid;
