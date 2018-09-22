drop table if exists tmpd;
select * into tmpd from d where d.iterationnr = (select max(iterationnr) from d as md where d.runid = md.runid);
select * into sd from s natural join tmpd;
drop table tmpd;
