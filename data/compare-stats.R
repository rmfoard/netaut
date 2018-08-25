p <- ggplot(data=avgstats) + geom_point(mapping=aes(x=factor(graphsize), y=avgclustcoef, color=machine, shape=collapsing))
print(p)
