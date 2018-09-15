# Average average clustering coefficient, C vs R machines, all initial graph sizes
# (depends on the 'sdavgs' table [avg-sd.sql])
#
p <- ggplot(data=sdavgs) +
    geom_line(
        mapping=aes(x=initnrnodes, y=aavgclustcoef, color=machinetype),
        size=2
    ) +
    geom_point(
        mapping=aes(x=initnrnodes, y=aavgclustcoef, color=machinetype),
        size=2
    ) +
    scale_color_manual(values=c("blue", "red")) +
    theme(
        plot.title = element_text(hjust = 0.5, size=16),
        plot.subtitle = element_text(hjust = 0.5),
        legend.title.align = 0.5
    ) +
    labs(
        title=paste("Average clustering coefficient, C and R machines"),
        subtitle=paste("[D]"),
        x="Initial graph size (nodes)",
        y="Average clustering coefficient"
    ) +
    labs(color="Machine") +
    coord_cartesian(ylim=c(0, 1.0))
print(p)
dev.copy(png, filename="figD.png")
dev.off()
