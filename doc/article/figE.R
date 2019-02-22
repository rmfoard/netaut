p <- ggplot(filter(idesummary, initnrnodes >= 256, entrytype == 'I' | entrytype == 'C' | entrytype == 'R',
        initnrnodes != 384 & initnrnodes != 640 & initnrnodes != 896 & initnrnodes < 2000)) +
    geom_boxplot(
        mapping=aes(x=factor(initnrnodes), y=indegreeentropy)
    ) +
    facet_wrap(~ factor(entrytype, levels=c('I', 'R', 'C'))) +
    theme(
        plot.title = element_text(hjust = 0.5, size=16),
        plot.subtitle = element_text(hjust = 0.5),
        legend.title.align = 0.5,
        axis.line = element_line(color="black"),
        panel.background = element_blank()
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
