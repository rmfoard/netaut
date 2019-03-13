set search_path to rings, public;
drop table if exists n;
select count(*) into n from sd where initnrnodes = 257;
--
insert into fracrand (statname, statmin, statmax, fracrand) values ('avgClustCoef', 0.0, 0.25, (select (select count(*) from sd where initnrnodes = 257 and 0.0 <= avgClustCoef and avgClustCoef <= 0.25) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('avgClustCoef', 0.25, 0.50, (select (select count(*) from sd where initnrnodes = 257 and 0.25 <= avgClustCoef and avgClustCoef <= 0.50) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('avgClustCoef', 0.50, 0.75, (select (select count(*) from sd where initnrnodes = 257 and 0.50 <= avgClustCoef and avgClustCoef <= 0.75) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('avgClustCoef', 0.75, 1.0, (select (select count(*) from sd where initnrnodes = 257 and 0.75 <= avgClustCoef and avgClustCoef <= 1.00) * 1.0 / (select count from n)));
--
insert into fracrand (statname, statmin, statmax, fracrand) values ('diameter', 1, 64, (select (select count(*) from sd where initnrnodes = 257 and 1 <= diameter and diameter <= 64) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('diameter', 64, 128, (select (select count(*) from sd where initnrnodes = 257 and 64 <= diameter and diameter <= 128) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('diameter', 128, 192, (select (select count(*) from sd where initnrnodes = 257 and 128 <= diameter and diameter <= 192) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('diameter', 192, 256, (select (select count(*) from sd where initnrnodes = 257 and 192 <= diameter and diameter <= 256) * 1.0 / (select count from n)));
--
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrCcSizes', 1, 6, (select (select count(*) from sd where initnrnodes = 257 and 1 <= nrCcSizes and nrCcSizes <= 6) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrCcSizes', 6, 12, (select (select count(*) from sd where initnrnodes = 257 and 6 <= nrCcSizes and nrCcSizes <= 12) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrCcSizes', 12, 18, (select (select count(*) from sd where initnrnodes = 257 and 12 <= nrCcSizes and nrCcSizes <= 18) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrCcSizes', 18, 24, (select (select count(*) from sd where initnrnodes = 257 and 18 <= nrCcSizes and nrCcSizes <= 24) * 1.0 / (select count from n)));
--
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrInDegrees', 1, 6, (select (select count(*) from sd where initnrnodes = 257 and 1 <= nrInDegrees and nrInDegrees <= 6) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrInDegrees', 6, 12, (select (select count(*) from sd where initnrnodes = 257 and 6 <= nrInDegrees and nrInDegrees <= 12) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrInDegrees', 12, 18, (select (select count(*) from sd where initnrnodes = 257 and 12 <= nrInDegrees and nrInDegrees <= 18) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrInDegrees', 18, 24, (select (select count(*) from sd where initnrnodes = 257 and 18 <= nrInDegrees and nrInDegrees <= 24) * 1.0 / (select count from n)));
--
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrCcs', 1, 64, (select (select count(*) from sd where initnrnodes = 257 and 1 <= nrCcs and nrCcs <= 64) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrCcs', 64, 128, (select (select count(*) from sd where initnrnodes = 257 and 64 <= nrCcs and nrCcs <= 128) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrCcs', 128, 192, (select (select count(*) from sd where initnrnodes = 257 and 128 <= nrCcs and nrCcs <= 192) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrCcs', 192, 256, (select (select count(*) from sd where initnrnodes = 257 and 192 <= nrCcs and nrCcs <= 256) * 1.0 / (select count from n)));
