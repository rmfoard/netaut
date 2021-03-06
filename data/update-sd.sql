select s.runid, sum(cccount * ccsize) into finnrnodes from s, c where s.runid = c.runid and c.iterationnr <> 0 group by s.runid;
alter table finnrnodes rename column sum to nrnodes;
create index finnrnodes_idx_runid on finnrnodes(runid);
create index sd_idx_runid on sd(runid);
update sd set finnrnodes = finnrnodes.nrnodes from finnrnodes where sd.runid = finnrnodes.runid;
drop table finnrnodes;
