select * into sd from (s natural join (select * from d where iterationnr <> 0) as y) as x;
alter table sd rename column nrnodes to finnrnodes;
