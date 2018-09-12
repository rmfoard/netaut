select
    initnrnodes,
    machinetype,
    avg(nriterations) as anriterations,
    avg(avgclustcoef) as aavgclustcoef,
    avg(diameter) as adiameter,
    avg(effdiameter90pctl) as aeffdiameter90pctl,
    avg(indegreeentropy) as aindegreeentropy,
    avg(nrccsizes) as anrccsizes,
    avg(nrccs) as anrccs,
    avg(nrclosedtriads) as anrclosedtriads,
    avg(nrindegrees) as anrindegrees,
    avg(nrnodes) as anrnodes,
    avg(nropentriads) as anropentriads
into sdavgs
from sd
where cyclelength = -1
group by initnrnodes, machinetype
order by initnrnodes, machinetype;
