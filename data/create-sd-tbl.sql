select * into sd from (s natural join (select * from d where iterationnr <> 0) as y) as x;
alter table sd rename column nrnodes to finnrnodes;
alter table sd add column outcome character;
update sd set outcome = 'Y';
update sd set outcome = 'L' where cyclelength = -1;
update sd set outcome = 'U' where cyclelength = 0;
