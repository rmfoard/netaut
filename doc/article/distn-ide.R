p <- ggplot(data=filter(sd, initnrnodes==1031, machinetype!='RM' & machinetype!='CM', outcome!='UND', machinetype=='C' | outcome!='CYC')) + 
   geom_density(mapping=aes(x=indegreeentropy, fill=machinetype)) + facet_wrap(~ outcome)
print(p)
dev.copy(png, filename="distn-ide.png")
dev.off()
