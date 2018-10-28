select machinetype, initnrnodes, outcome, rulepart, count(*) as nrruns
into partruns
from partpcts
group by machinetype, initnrnodes, outcome, rulepart;
