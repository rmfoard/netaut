drop table if exists c_cyc_runs_by_part;
select rulepart, sum(nrruns) as cycruns into c_cyc_runs_by_part from partruns where machinetype = 'C' and outcome = 'CYC' group by rulepart;
--
drop table if exists c_col_runs_by_part;
select rulepart, sum(nrruns) as colruns into c_col_runs_by_part from partruns where machinetype = 'C' and outcome = 'COL' group by rulepart;
--
drop table if exists c_runs_by_part;
select cyc.rulepart, cycruns, colruns
into c_runs_by_part
from c_cyc_runs_by_part as cyc, c_col_runs_by_part as col
where cyc.rulepart = col.rulepart
order by cycruns;
--
drop table c_cyc_runs_by_part;
drop table c_col_runs_by_part;
