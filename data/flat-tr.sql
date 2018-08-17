select
    (select sum(triadcount) from paredtr where triad = 0) as tr0,
    (select sum(triadcount) from paredtr where triad = 1) as tr1,
    (select sum(triadcount) from paredtr where triad = 2) as tr2,
    (select sum(triadcount) from paredtr where triad = 3) as tr3,
    (select sum(triadcount) from paredtr where triad = 4) as tr4,
    (select sum(triadcount) from paredtr where triad = 5) as tr5,
    (select sum(triadcount) from paredtr where triad = 6) as tr6,
    (select sum(triadcount) from paredtr where triad = 7) as tr7
into flattr;
