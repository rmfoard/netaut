# Average terminal number of in-degrees

p <- ggplot() +
    geom_line(data=filter(
        sdavgs,
        mach_outc == 'C->COL' | mach_outc == 'C->CYC' | mach_outc == 'R->COL' | mach_outc == 'CM->CYC',
        initnrnodes != 384 & initnrnodes != 640 & initnrnodes != 896,
        initnrnodes < 2048
      ),
      mapping=aes(
        x=initnrnodes,
        y=anrindegrees,
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
        title=paste("Average terminal number of in-degrees"),
        subtitle=paste("[outcomes-nrid]"),
        x="Initial graph size (nodes)",
        y="Average terminal number of in-degrees"
    )

print(p)
dev.copy(png, filename="outcomes-nrid.png")
dev.off()
