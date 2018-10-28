select s.runid, rulenr, sum(triadcount) as tottriads
into triadtots
from t, s
where t.runid = s.runid
group by s.runid, rulenr;
