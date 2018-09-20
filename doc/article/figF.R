# Distribution of in-degrees, C cycling vs C collapsing
#
tmp <- dbGetQuery(con, "select indegreesize, outcome, sum(sum) as sumnrnodes from degsums where machinetype = 'C' and initnrnodes = 1024 group by indegreesize, outcome")
p <- ggplot(data=tmp) +
    geom_smooth(
        mapping=aes(x=indegreesize, y=sumnrnodes, color=outcome)
    ) +
    scale_y_continuous(trans='log')
rm(tmp)
print(p)
dev.copy(png, filename="figF.png")
dev.off()
