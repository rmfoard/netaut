select rulenr, randseed into r_stepdown_ids from s where machinetype = 'R' and initnrnodes = 512 and nriterations = 10 limit 16;
