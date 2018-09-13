p <- ggplot(data=avgstats) +
    geom_point(mapping=aes(x=factor(graphsize), y=iterations, color=machine, shape=collapsing), size=4) +
    scale_shape_manual(values=c(1, 13, 2))
print(p)
