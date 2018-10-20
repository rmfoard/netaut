fcs <- function(schema) {
x <- dbGetQuery(con, paste("set search_path to", schema))
steps <- dbGetQuery(con, "select * from steps")
p <- ggplot(data=steps) +
geom_step(mapping=aes(x=iterationnr, y=nrnodes)) +
facet_wrap(~ ruleid) +
    theme(
        plot.title = element_text(hjust = 0.5, size=16),
        plot.subtitle = element_text(hjust = 0.5),
        legend.title.align = 0.5,
        panel.background = element_blank()
    ) +
    labs(
        x="Iteration",
        y="Graph Size (nodes)"
    )
rm(steps)
print(p)
dev.copy(png, filename=paste(schema, ".png", sep=""))
dev.off()
}
print("defined fcs(schema)")
