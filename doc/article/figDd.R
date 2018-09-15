# 
# 
#
p <- ggplot(data=filter(sd)) +
    geom_freqpoly(
        mapping=aes(x=avgclustcoef, color=machinetype),
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
        title=paste("Distribution of clustering coefficients, C and R machines"),
        subtitle=paste("[Dd]"),
        x="Clustering coefficient",
        y="Number of runs"
    ) +
    labs(color="Machine")
print(p)
dev.copy(png, filename="figDd.png")
dev.off()
