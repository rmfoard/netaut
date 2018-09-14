# Average maximum in-degree, C vs R machines, all initial graph sizes
# (depends on the 'sdavgs' table [avg-sd.sql])
#
p <- ggplot(data=sdavgs) +
    geom_line(
        mapping=aes(x=initnrnodes, y=amaxindegree, color=machinetype),
        size=2
    ) +
    geom_point(
        mapping=aes(x=initnrnodes, y=amaxindegree, color=machinetype),
        size=2
    ) +
    scale_color_manual(values=c("blue", "red")) +
    theme(
        plot.title = element_text(hjust = 0.5, size=16),
        plot.subtitle = element_text(hjust = 0.5),
        legend.title.align = 0.5
    ) +
    labs(
        title=paste("Average maximum in-degree, C and R machines"),
        subtitle=paste("[C]"),
        x="Initial graph size (nodes)",
        y="Average maximum in-degree"
    ) +
    labs(color="Machine")
print(p)
dev.copy(png, filename="figC.png")
dev.off()
