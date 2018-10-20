fcs <- function(schema) {
x <- dbGetQuery(con, paste("set search_path to", schema))
steps <- dbGetQuery(con, "select * from steps")
p <- ggplot(data=steps) +
geom_step(mapping=aes(x=iterationnr, y=nrnodes)) +
facet_wrap(~ ruleid)
rm(steps)
print(p)
}
print("defined fcs(schema)")
