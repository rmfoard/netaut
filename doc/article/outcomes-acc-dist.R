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
        plot.title=element_text(hjust=0.5, size=16),
        plot.subtitle=element_text(hjust=0.5),
        legend.title.align=0.5
    ) +
    labs(
        title=paste("Distribution of terminal clustering coefficient"),
        subtitle=paste("[outcomes-acc-dist]"),
        x="Terminal average clustering coefficient"
    )

print(p)
dev.copy(png, filename="../outcomes-acc-dist.png")
dev.off()
