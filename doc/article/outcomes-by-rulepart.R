xf <- reorder(c_runs_by_part$rulepart, -c_runs_by_part$colruns)
p <- ggplot(data=c_runs_by_part) +
    geom_point(mapping=aes(x=xf, y=cycruns), color="orange") +
    geom_point(mapping=aes(x=xf, y=colruns), color="green")
print(p)
dev.copy(png, "outcomes-by-rulepart.png")
dev.off()
