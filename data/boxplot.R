p <- ggplot(data=sd) + geom_boxplot(mapping=aes(x=initnrnodes, y=nrindegrees, group=initnrnodes)) + facet_wrap(~ machinetype)
print(p)
