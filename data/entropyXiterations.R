p <- ggplot(data=filter(sdx, nriterations <= 300)) + geom_point(mapping=aes(x=nriterations, y=indegreeentropy, color=machinetype)) + facet_wrap(~ initnrnodes)
print(p)
