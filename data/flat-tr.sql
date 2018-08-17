select
    (sum(triadcount) from paredtr where triad = 0) as tr0,
    (sum(triadcount) from paredtr where triad = 1) as tr1,
    (sum(triadcount) from paredtr where triad = 2) as tr2,
    (sum(triadcount) from paredtr where triad = 3) as tr3,
    (sum(triadcount) from paredtr where triad = 4) as tr4,
    (sum(triadcount) from paredtr where triad = 5) as tr5,
    (sum(triadcount) from paredtr where triad = 6) as tr6,
    (sum(triadcount) from paredtr where triad = 7) as tr7
into flattr;
