# 
# 
#
p <- ggplot(data=filter(sd)) +
    geom_freqpoly(
        mapping=aes(x=nrindegrees, color=machinetype),
        binwidth=1
    ) +
    facet_wrap(~ factor(initnrnodes)) +
    scale_color_manual(values=c("blue", "red")) +
    theme(
        plot.title = element_text(hjust = 0.5, size=16),
        plot.subtitle = element_text(hjust = 0.5),
        legend.title.align = 0.5
    ) +
    labs(
        title=paste("Distribution of # of in-degrees, C and R machines"),
        subtitle=paste("[Bd]"),
        x="Number of distinct in-degrees",
        y="Number of runs"
    ) +
    labs(color="Machine")
print(p)
dev.copy(png, filename="figBd.png")
dev.off()
