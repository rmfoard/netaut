select * into sd from (s natural join (select * from d where iterationnr <> 0) as y) as x;
alter table sd rename column nrnodes to finnrnodes;
alter table sd add column outcome varchar(3);
alter table sd add column mach_outc varchar(7);
update sd set outcome = 'CYC';
update sd set outcome = 'COL' where cyclelength = -1;
update sd set outcome = 'UND' where cyclelength = 0;
update sd set mach_outc = machinetype || '->' || outcome;
