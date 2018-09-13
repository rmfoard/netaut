# Graph collapse vs. cycle outcomes for various initial graph sizes, machine C
# TODO: Show proportions instead of absolute counts.
print("(for the model_c database, open as 'con')")
dbGetQuery(con, "select initnrnodes, cyclelength, '' as outcome into tmp from s where machinetype = 'C'")
dbGetQuery(con, "update tmp set outcome = 'collapse' where cyclelength = -1")
dbGetQuery(con, "update tmp set outcome = 'cycle' where cyclelength <> -1")
tmp <- dbGetQuery(con, "select * from tmp")
dbGetQuery(con, "drop table tmp")
p <- ggplot(data=tmp) +
    geom_bar(
        mapping=aes(x=factor(initnrnodes), fill=outcome),
        position="dodge"
    ) +
    theme(
        plot.title = element_text(hjust = 0.5),
        plot.subtitle = element_text(hjust = 0.5)
    ) +
    labs(
        title=paste("Collapses outnumber cycles."),
        subtitle=paste("collVcyc [Machine C]"),
        x="Initial graph size",
        y="Number of runs"
    )
rm(tmp)
print(p)
dev.copy(png, filename="collVcyc.png")
dev.off()
