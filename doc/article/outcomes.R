p <- ggplot() +
    geom_line(data=filter(sdavgs, machinetype == 'C', outcome == 'Y', initnrnodes < 2048),
      mapping=aes(x=initnrnodes, y=anrindegrees),
      color="blue"
    ) +
    geom_line(data=filter(sdavgs, machinetype == 'C', outcome == 'L', initnrnodes < 2048),
      mapping=aes(x=initnrnodes, y=anrindegrees),
      color="brown"
    ) +
    geom_line(data=filter(sdavgs, machinetype == 'CM', outcome == 'Y', initnrnodes < 2048),
      mapping=aes(x=initnrnodes, y=anrindegrees),
      color="green"
    ) +
    geom_line(data=filter(sdavgs, machinetype == 'R', outcome == 'L', initnrnodes < 2048),
      mapping=aes(x=initnrnodes, y=anrindegrees),
      color="red"
    )
print(p)
