print("verify database 'primes':")
print(database)
p <- ggplot(data=filter(sd, machinetype=="C", outcome!="UND")) +
    geom_smooth(mapping=
        aes(y=nriterations, x=finnrnodes, linetype=outcome),
      color="blue"
    ) +
    facet_wrap(~ factor(initnrnodes)) +
    theme(
        legend.title=element_text(size=18),
        legend.text=element_text(size=14),
        axis.title=element_text(size=20),
        legend.title.align=0.5,
        axis.line = element_line(color="gray"),
        panel.background=element_blank()
    ) +
    labs(
        x="Terminal graph size (nodes)",
        y="Number of iterations"
    )

print(p)
dev.copy(png, filename="../nriter-x-finnn-smooth.png")
dev.off()
