# Distribution of run lengths, machine C
print("(for the model_c database, open as 'con')")
dbGetQuery(con, "select initnrnodes, nriterations, cyclelength, 'cycling' as outcome into tmp from s where machinetype = 'C' and cyclelength != 0")
dbGetQuery(con, "update tmp set outcome = 'collapsing' where cyclelength = -1")
tmp <- dbGetQuery(con, "select * from tmp")
dbGetQuery(con, "drop table tmp")
p <- ggplot(data=tmp) +
    geom_freqpoly (
        mapping=aes(x=nriterations, color=factor(initnrnodes)),
        binwidth=1
    ) +
    coord_cartesian (
        xlim=c(0, 30)
    ) +
    theme(
        plot.title = element_text(hjust = 0.5),
        plot.subtitle = element_text(hjust = 0.5)
    ) +
    labs(
        title=paste("Average run length increases with initial graph size, and is larger in the cycling case."),
        subtitle=paste("rlDist [Machine C]"),
        x="Run length (iterations)",
        y="Number of runs"
    ) +
    facet_wrap(~ outcome, nrow=2, ncol=1)
rm(tmp)
print(p)
dev.copy(png, filename="rlDist.png")
dev.off()
