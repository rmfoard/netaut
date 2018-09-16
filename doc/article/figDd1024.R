# 
# 
#
p <- ggplot(data=filter(sd, initnrnodes == 1024)) +
    geom_freqpoly(
        mapping=aes(x=avgclustcoef, color=machinetype),
        binwidth=0.025,
        size=2
    ) +
    scale_color_manual(values=c("blue", "red")) +
    theme(
        plot.title = element_text(hjust = 0.5, size=16),
        plot.subtitle = element_text(hjust = 0.5),
        legend.title.align = 0.5
    ) +
    labs(
        title=paste("Distribution of clustering coefficients, C and R machines"),
        subtitle=paste("(1024 initial nodes) [Dd1024]"),
        x="Clustering coefficient",
        y="Number of runs"
    ) +
    labs(color="Machine")
print(p)
dev.copy(png, filename="figDd1024.png")
dev.off()
