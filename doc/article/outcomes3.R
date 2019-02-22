p <- ggplot() +
    geom_line(data=filter(sdavgs, machinetype == 'C', outcome == 'CYC', initnrnodes < 2048),
      mapping=aes(x=initnrnodes, y=aindegreeentropy),
      color="blue"
    ) +
    geom_line(data=filter(sdavgs, machinetype == 'C', outcome == 'COL', initnrnodes < 2048),
      mapping=aes(x=initnrnodes, y=aindegreeentropy),
      color="brown"
    ) +
    geom_line(data=filter(sdavgs, machinetype == 'CM', outcome == 'CYC', initnrnodes < 2048),
      mapping=aes(x=initnrnodes, y=aindegreeentropy),
      color="green"
    ) +
    geom_line(data=filter(sdavgs, machinetype == 'R', outcome == 'COL', initnrnodes < 2048),
      mapping=aes(x=initnrnodes, y=aindegreeentropy),
      color="red"
    )
print(p)
