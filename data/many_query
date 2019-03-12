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
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrCcSizes', 1, 64, (select (select count(*) from sd where initnrnodes = 257 and 1 <= nrCcSizes and nrCcSizes <= 64) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrCcSizes', 64, 128, (select (select count(*) from sd where initnrnodes = 257 and 64 <= nrCcSizes and nrCcSizes <= 128) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrCcSizes', 128, 192, (select (select count(*) from sd where initnrnodes = 257 and 128 <= nrCcSizes and nrCcSizes <= 192) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrCcSizes', 192, 256, (select (select count(*) from sd where initnrnodes = 257 and 192 <= nrCcSizes and nrCcSizes <= 256) * 1.0 / (select count from n)));
--
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrInDegrees', 1, 64, (select (select count(*) from sd where initnrnodes = 257 and 1 <= nrInDegrees and nrInDegrees <= 64) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrInDegrees', 64, 128, (select (select count(*) from sd where initnrnodes = 257 and 64 <= nrInDegrees and nrInDegrees <= 128) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrInDegrees', 128, 192, (select (select count(*) from sd where initnrnodes = 257 and 128 <= nrInDegrees and nrInDegrees <= 192) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrInDegrees', 192, 256, (select (select count(*) from sd where initnrnodes = 257 and 192 <= nrInDegrees and nrInDegrees <= 256) * 1.0 / (select count from n)));
--
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrCcs', 1, 64, (select (select count(*) from sd where initnrnodes = 257 and 1 <= nrCcs and nrCcs <= 64) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrCcs', 64, 128, (select (select count(*) from sd where initnrnodes = 257 and 64 <= nrCcs and nrCcs <= 128) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrCcs', 128, 192, (select (select count(*) from sd where initnrnodes = 257 and 128 <= nrCcs and nrCcs <= 192) * 1.0 / (select count from n)));
insert into fracrand (statname, statmin, statmax, fracrand) values ('nrCcs', 192, 256, (select (select count(*) from sd where initnrnodes = 257 and 192 <= nrCcs and nrCcs <= 256) * 1.0 / (select count from n)));
