p <- ggplot(data=sdx) + geom_point(mapping=aes(x=avgclustcoef, y=indegreeentropy, color=machinetype)) + facet_wrap(~ initnrnodes)
print(p)
