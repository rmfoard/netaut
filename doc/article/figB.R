# Number of in-degrees, C vs R machines, all initial graph sizes
# (depends on the 'sdavgs' table [avg-sd.sql])
#
p <- ggplot(data=filter(sdavgs, initnrnodes <= 1024, machinetype == 'C' | machinetype == 'R')) +
    geom_line(
        mapping=aes(x=initnrnodes, y=anrindegrees, color=machinetype),
        size=2
    ) +
    geom_point(
        mapping=aes(x=initnrnodes, y=anrindegrees, color=machinetype),
        size=2
    ) +
    scale_color_manual(values=c("blue", "red", "green", "gray")) +
    theme(
        plot.title = element_text(hjust = 0.5, size=16),
        plot.subtitle = element_text(hjust = 0.5),
        legend.title.align = 0.5
    ) +
    labs(
        title=paste("Average # of in-degrees, C and R machines"),
        subtitle=paste("[B]"),
        x="Initial graph size (nodes)",
        y="Number of in-degrees"
    ) +
    labs(color="Machine")
print(p)
dev.copy(png, filename="figB.png")
dev.off()
