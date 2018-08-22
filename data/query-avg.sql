select
    initnrnodes,
    avg(avgClustCoef) as aavgClustCoef,
    avg(diameter) as adiameter,
    avg(inDegreeEntropy) as ainDegreeEntropy,
    avg(nrCcSizes) as anrCcSizes,
    avg(nrCcs) as anrCcs,
    avg(nrClosedTriads) as  anrClosedTriads,
    avg(nrInDegrees) as anrInDegrees,
    avg(finNrNodes) as afinNrNodes,
    avg(nrOpenTriads) as anrOpenTriads
from sd
where iterationNr <> 0
group by initnrnodes;
