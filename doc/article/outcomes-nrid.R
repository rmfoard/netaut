p <- ggplot() +
    geom_line(data=filter(
        sdavgs,
        mach_outc == 'C->COL' | mach_outc == 'C->CYC' | mach_outc == 'R->COL' | mach_outc == 'CM->CYC',
        initnrnodes < 2048
      ),
      mapping=aes(
        x=initnrnodes,
        y=anrindegrees,
        color=machinetype,
        linetype=outcome
      )
    ) +
    scale_color_manual(values=c("blue", "gray50", "red"))
print(p)
