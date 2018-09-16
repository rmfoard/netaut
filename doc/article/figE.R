p <- ggplot(filter(idesummary, initnrnodes >= 256)) +
    geom_boxplot(
        mapping=aes(x=factor(initnrnodes), y=indegreeentropy)
    ) +
    facet_wrap(~ factor(entrytype, levels=c('I', 'R', 'C'))) +
    theme(
        plot.title = element_text(hjust = 0.5, size=16),
        plot.subtitle = element_text(hjust = 0.5),
        legend.title.align = 0.5
    ) +
    labs(
        title=paste("In-degree entropy for initial, R-terminal and C-terminal graphs"),
        subtitle=paste("[E]"),
        x="Initial graph size (nodes)",
        y="In-degree entropy"
    )
print(p)
dev.copy(png, filename="figE.png")
dev.off()
