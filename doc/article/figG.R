# Distribution of in-degrees, C cycling vs C collapsing
#
tmp <- dbGetQuery(con, "select machinetype, indegreesize, outcome, sum(sum) as sumnrnodes from degsums where outcome = 'C' and initnrnodes = 1024 group by machinetype, indegreesize, outcome")
p <- ggplot(data=tmp) +
    geom_smooth(
        mapping=aes(x=indegreesize, y=sumnrnodes, color=machinetype)
    ) +
    scale_y_continuous(trans='log')
rm(tmp)
print(p)
dev.copy(png, filename="figG.png")
dev.off()
