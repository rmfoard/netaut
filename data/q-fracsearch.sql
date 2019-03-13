select statname, statmin, statmax, sum(nrfitrules)*1.0 / sum(nrdistinctrules) as fracsearch, cumfitnessexp
into p_acc from gs where statname = 'avgClustCoef' and cumfitnessexp = 0.6
group by statmin, statmax order by statmin, statmax;
--
select statname, statmin, statmax, sum(nrfitrules)*1.0 / sum(nrdistinctrules) as fracsearch, cumfitnessexp
into p_d from gs where statname = 'diameter' and cumfitnessexp = 1.4
group by statmin, statmax order by statmin, statmax;
--
select statname, statmin, statmax, sum(nrfitrules)*1.0 / sum(nrdistinctrules) as fracsearch, cumfitnessexp
into p_ncc from gs where statname = 'nrCcs' and cumfitnessexp = 1.0
group by statmin, statmax order by statmin, statmax;
--
select statname, statmin, statmax, sum(nrfitrules)*1.0 / sum(nrdistinctrules) as fracsearch, cumfitnessexp
into p_nccs from gs where statname = 'nrCcSizes' and cumfitnessexp = 1.4
group by statmin, statmax order by statmin, statmax;
--
select statname, statmin, statmax, sum(nrfitrules)*1.0 / sum(nrdistinctrules) as fracsearch, cumfitnessexp
into p_ni from gs where statname = 'nrInDegrees' and cumfitnessexp = 1.0
group by statmin, statmax order by statmin, statmax;
