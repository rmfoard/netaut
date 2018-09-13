# Distribution of run lengths, machine C vs. machine R
print("(for the model_c database, open as 'con')")
dbGetQuery(con, "select machinetype, initnrnodes, nriterations, cyclelength into tmp from s where cyclelength = -1")
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
        title=paste("Average run lengths are smaller for a random machine."),
        subtitle=paste("rlDistCR [Machines C, R]"),
        x="Run length (iterations)",
        y="Number of runs"
    ) +
    facet_wrap(~ machinetype, nrow=2, ncol=1)
rm(tmp)
print(p)
dev.copy(png, filename="rlDistCR.png")
dev.off()
