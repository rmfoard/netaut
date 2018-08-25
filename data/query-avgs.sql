select machineType as Machine, (cycleLength = -1) as Collapsing, initNrNodes as GraphSize,
    avg(nrNodes) as TeminalNodes,
    avg(nrIterations) as Iterations,
    avg(avgClustCoef) as AvgClustCoef,
    avg(nrOpenTriads) as OpenTriads,
    avg(nrClosedTriads) as ClosedTriads,
    avg(nrCcs) as ConnComponentss,
    avg(nrInDegrees) as InDegrees,
    avg(inDegreeEntropy) as InDegreeEntropy
into avgstats
from sdxcrpt
group by GraphSize, Machine, Collapsing
order by GraphSize, Machine, Collapsing;
