x <- dbGetQuery(con, "set search_path to ccyc_stepdown")
steps <- dbGetQuery(con, "select * from steps")
p <- ggplot(data=steps) +
geom_step(mapping=aes(x=iterationnr, y=nrnodes)) +
facet_wrap(~ ruleid)
rm(steps)
print(p)
