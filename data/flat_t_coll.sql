select
    (select sum(p.triadcount) from pared_t as p, prunids_coll as s
        where p.runid = s.runid and triad = 0) as tr0,
    (select sum(p.triadcount) from pared_t as p, prunids_coll as s
        where p.runid = s.runid and triad = 1) as tr1,
    (select sum(p.triadcount) from pared_t as p, prunids_coll as s
        where p.runid = s.runid and triad = 2) as tr2,
    (select sum(p.triadcount) from pared_t as p, prunids_coll as s
        where p.runid = s.runid and triad = 3) as tr3,
    (select sum(p.triadcount) from pared_t as p, prunids_coll as s
        where p.runid = s.runid and triad = 4) as tr4,
    (select sum(p.triadcount) from pared_t as p, prunids_coll as s
        where p.runid = s.runid and triad = 5) as tr5,
    (select sum(p.triadcount) from pared_t as p, prunids_coll as s
        where p.runid = s.runid and triad = 6) as tr6,
    (select sum(p.triadcount) from pared_t as p, prunids_coll as s
        where p.runid = s.runid and triad = 7) as tr7
into flat_t_coll;
