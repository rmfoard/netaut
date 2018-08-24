p = ggplot(data=filter(sdx, iterationnr != 0, nriterations<30)) +
    geom_freqpoly(
        mapping=aes(x=nriterations, color=machinetype),
        binwidth=1) +
    facet_wrap(~ initnrnodes)
print(p) 
