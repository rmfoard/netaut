select rulenr, randseed into r_stepdown_ids from s where machinetype = 'S' and initnrnodes = 512 and nriterations = 16 limit 16;
