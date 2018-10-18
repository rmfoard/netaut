# Average terminal clustering coefficient

p <- ggplot() +
    geom_line(data=filter(
        sdavgs,
        mach_outc == 'C->COL' | mach_outc == 'C->CYC' | mach_outc == 'R->COL' | mach_outc == 'CM->CYC',
        initnrnodes != 384 & initnrnodes != 640 & initnrnodes != 896,
        initnrnodes < 2048
      ),
      mapping=aes(
        x=initnrnodes,
        y=aavgclustcoef,
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
        title=paste("Average terminal clustering coefficient"),
        subtitle=paste("[outcomes-acc]"),
        x="Initial graph size (nodes)",
        y="Average terminal clustering coefficient"
    )

print(p)
dev.copy(png, filename="outcomes-acc.png")
dev.off()
