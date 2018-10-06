select
    initnrnodes,
    machinetype,
    outcome,
    mach_outc,
    avg(nriterations) as anriterations,
    avg(cyclelength) as acyclelength,
    avg(avgclustcoef) as aavgclustcoef,
    avg(diameter) as adiameter,
    avg(effdiameter90pctl) as aeffdiameter90pctl,
    avg(indegreeentropy) as aindegreeentropy,
    avg(nrccsizes) as anrccsizes,
    avg(nrccs) as anrccs,
    avg(nrclosedtriads) as anrclosedtriads,
    avg(nrindegrees) as anrindegrees,
    avg(finnrnodes) as afinnrnodes,
    avg(nropentriads) as anropentriads
into sdavgs
from sd
group by initnrnodes, machinetype, outcome, mach_outc
order by initnrnodes, machinetype, outcome;
