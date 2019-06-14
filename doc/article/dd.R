p <- ggplot() + geom_smooth(data=filter(deg12typ, indegreesize <=14), mapping=aes(x=indegreesize, y=indegreecount), color="blue") + scale_y_continuous(trans="log10") +
geom_smooth(data=filter(deg12run, indegreesize <= 14), mapping=aes(x=indegreesize, y=indegreecount), color="green")
