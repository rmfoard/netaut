# Average terminal clustering coefficient

print("Confirm database 'primes':")
print(database)
p <- ggplot() +
    geom_density(data=filter(
        sd,
        mach_outc == 'C->COL' | mach_outc == 'C->CYC' | mach_outc == 'R->COL',
        initnrnodes == 1031
      ),
      mapping=aes(
        x=avgclustcoef,
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
        x="Terminal average clustering coefficient"
    )

print(p)
dev.copy(png, filename="../outcomes-acc-dist.png")
dev.off()
