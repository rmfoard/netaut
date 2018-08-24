select sx.runid, sum(cccount * ccsize) into finnrnodes from sdxcrpt as sx, c where sx.runid = c.runid group by sx.runid;
alter table finnrnodes rename column sum to nrnodes;
create index finnrnodes_idx_runid on finnrnodes(runid);
create index sdxcrpt_idx_runid on sdxcrpt(runid);
update sdxcrpt set nrnodes = finnrnodes.nrnodes from finnrnodes where sdxcrpt.runid = finnrnodes.runid;
