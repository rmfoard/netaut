select rulenr, randseed into sens_ids from (select rulenr, randseed from s where initnrnodes = 512 and machinetype = 'C' and cyclelength <> 0 order by randseed, rulenr) as x limit 100;
