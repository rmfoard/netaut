# Average number of iterations

p <- ggplot() +
    geom_line(data=filter(
        sdavgs,
        mach_outc == 'C->COL' | mach_outc == 'C->CYC' | mach_outc == 'R->COL' | mach_outc == 'CM->CYC',
        initnrnodes < 2048
      ),
      mapping=aes(
        x=initnrnodes,
        y=anriterations,
        color=machinetype,
        linetype=outcome
      )
    ) +
    scale_color_manual(values=c("blue", "gray50", "red")) +
    theme(
        plot.title=element_text(hjust=0.5, size=16),
        plot.subtitle=element_text(hjust=0.5),
        legend.title.align=0.5
    ) +
    labs(
        title=paste("Average number of iterations"),
        subtitle=paste("[outcomes-nriter]"),
        x="Initial graph size (nodes)",
        y="Average number of iterations"
    )

print(p)
dev.copy(png, filename="outcomes-nriter.png")
dev.off()
