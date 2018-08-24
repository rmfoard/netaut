ggplot() +
    geom_freqpoly(data=filter(sdx, initnrnodes==512, cyclelength==-1, iterationnr != 0, nriterations<15),
        mapping=aes(x=nriterations, color=machinetype),
        binwidth=1)
print() 
