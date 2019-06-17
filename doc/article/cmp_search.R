ggplot(data=data) +
    geom_bar(mapping=aes(x=range_qtr, y=nrrules, fill=method), stat="identity", position="dodge") +
    facet_wrap(~ statname)
