# Graph collapse vs. cycle outcomes for various initial graph sizes, machine C
# TODO: Show proportions instead of absolute counts.
# TODO: Add a descriptive title and improve axis labels
print("(for the model_c database, open as 'con')")
dbGetQuery(con, "select initnrnodes, cyclelength, '' as outcome into tmp from s where machinetype = 'C'")
dbGetQuery(con, "update tmp set outcome = 'collapse' where cyclelength = -1")
dbGetQuery(con, "update tmp set outcome = 'cycle' where cyclelength <> -1")
tmp <- dbGetQuery(con, "select * from tmp")
dbGetQuery(con, "drop table tmp")
p <- ggplot(data=tmp) + geom_bar(mapping=aes(x=factor(initnrnodes), fill=outcome))
rm(tmp)
dev.copy(png, "collVcyc.png")
dev.off()
print(p)
rm(p)
