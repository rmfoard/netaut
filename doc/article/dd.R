p <- ggplot() +
    geom_smooth(data=filter(deg12typ, indegreesize <=14), mapping=aes(x=indegreesize, y=indegreecount), color="blue") +
    geom_smooth(data=filter(deg12run, indegreesize <= 14), mapping=aes(x=indegreesize, y=indegreecount), color="green") +
    scale_y_continuous(trans="log10") +
    theme(
        legend.title=element_text(size=18),
        legend.text=element_text(size=14),
        axis.title=element_text(size=20),
        legend.title.align=0.5,
        axis.line = element_line(color="gray"),
        panel.background=element_blank()
    ) +
    labs(
        x="In-degree",
        y="Number of nodes (log scale)"
    )
print(p)
dev.copy(png, filename="dd.png")
dev.off()
