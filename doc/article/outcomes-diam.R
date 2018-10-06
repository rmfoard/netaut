# Average terminal diameter

p <- ggplot() +
    geom_line(data=filter(
        sdavgs,
        mach_outc == 'C->COL' | mach_outc == 'C->CYC' | mach_outc == 'R->COL' | mach_outc == 'CM->CYC',
        initnrnodes < 2048
      ),
      mapping=aes(
        x=initnrnodes,
        y=adiameter,
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
        title=paste("Average terminal diameter"),
        subtitle=paste("[outcomes-diam]"),
        x="Initial graph size (nodes)",
        y="Average terminal diameter"
    )

print(p)
dev.copy(png, filename="outcomes-diam.png")
dev.off()
