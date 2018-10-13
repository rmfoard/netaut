p <- ggplot() +
    geom_line(data=filter(sdavgs, machinetype == 'C', outcome == 'CYC', initnrnodes < 2048),
      mapping=aes(x=initnrnodes, y=afinnrnodes),
      color="blue"
    ) +
    geom_line(data=filter(sdavgs, machinetype == 'C', outcome == 'COL', initnrnodes < 2048),
      mapping=aes(x=initnrnodes, y=afinnrnodes),
      color="brown"
    ) +
    geom_line(data=filter(sdavgs, machinetype == 'CM', outcome == 'CYC', initnrnodes < 2048),
      mapping=aes(x=initnrnodes, y=afinnrnodes),
      color="green"
    ) +
    geom_line(data=filter(sdavgs, machinetype == 'R', outcome == 'COL', initnrnodes < 2048),
      mapping=aes(x=initnrnodes, y=afinnrnodes),
      color="red"
    )
print(p)
