# Average number of iterations
# 'crm' db

p <- ggplot() +
    geom_line(data=filter(
        sdavgs,
        mach_outc == 'C->COL' | mach_outc == 'C->CYC' | mach_outc == 'R->COL',
        initnrnodes != 384 & initnrnodes != 640 & initnrnodes != 896,
        initnrnodes < 2048
      ),
      mapping=aes(
        x=initnrnodes,
        y=anriterations,
        color=machinetype,
        linetype=outcome
      )
    ) +
    scale_color_manual(values=c("blue", "red")) +
    theme(
        legend.title=element_text(size=18),
        legend.text=element_text(size=14),
        axis.title=element_text(size=20),
        legend.title.align=0.5,
        axis.line = element_line(color="gray"),
        panel.background=element_blank()
    ) +
    labs(
        x="Initial graph size (nodes)",
        y="Average number of iterations"
    )

print(p)
dev.copy(png, filename="../outcomes-nriter-avg.png")
dev.off()
