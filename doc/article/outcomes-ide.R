# Average terminal in-degree entropy

p <- ggplot() +
    geom_line(data=filter(
        sdavgs,
        mach_outc == 'C->COL' | mach_outc == 'C->CYC' | mach_outc == 'R->COL' | mach_outc == 'CM->CYC',
        initnrnodes < 2048
      ),
      mapping=aes(
        x=initnrnodes,
        y=aindegreeentropy,
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
        title=paste("Average terminal in-degree entropy"),
        subtitle=paste("[outcomes-ide]"),
        x="Initial graph size (nodes)",
        y="Average terminal in-degree entropy"
    )

print(p)
dev.copy(png, filename="outcomes-ide.png")
dev.off()
