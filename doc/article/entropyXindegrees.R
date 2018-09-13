p <- ggplot(data=sdx) + geom_point(mapping=aes(x=nrindegrees, y=indegreeentropy, color=machinetype)) + facet_wrap(~ initnrnodes)
print(p)
