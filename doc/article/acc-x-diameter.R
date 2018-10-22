ggplot(data=filter(sd, machinetype == "C" | machinetype == "R", initnrnodes == 1031)) +
  geom_point(mapping=aes(x=diameter, y=avgclustcoef, color=mach_outc), position="jitter")
