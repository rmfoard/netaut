# Average maximum in-degree, C vs R machines, all initial graph sizes
# (depends on the 'sdavgs' table [avg-sd.sql])
#
p <- ggplot(data=filter(sd)) +
    geom_freqpoly(
        mapping=aes(x=maxindegree, color=machinetype),
        binwidth=20
    ) +
    facet_wrap(~ factor(initnrnodes)) +
    scale_color_manual(values=c("blue", "red")) +
    theme(
        plot.title = element_text(hjust = 0.5, size=16),
        plot.subtitle = element_text(hjust = 0.5),
        legend.title.align = 0.5
    ) +
    labs(
        title=paste("Distribution of maximum in-degrees, C and R machines"),
        subtitle=paste("[Cd]"),
        x="Maximum in-degree",
        y="Number of runs"
    ) +
    labs(color="Machine")
print(p)
dev.copy(png, filename="figCd.png")
dev.off()
