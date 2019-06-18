p <- ggplot(data=data) +
    geom_bar(mapping=aes(x=range_qtr, y=nrrules, fill=method), stat="identity", position="dodge") +
    scale_fill_manual(values=c("blue", "red")) +
    facet_wrap(~ statname, nrow=1, ncol=5, strip.position="bottom") +
    theme(
        aspect.ratio=1.75,
        plot.title = element_text(hjust = 0.5, size=16),
        plot.subtitle = element_text(hjust = 0.5),
        legend.title.align = 0.5,
        panel.background = element_blank()
    ) +
    labs(
        x="Value Quartile",
        y="# Rules Found",
        fill="Search Type"
    )
print(p)
dev.copy(png, filename="cmp-search.png")
dev.off()
