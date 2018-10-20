p <- ggplot(data=filter(sd, initnrnodes==1031, machinetype!='RM' & machinetype!='CM', outcome!='UND', machinetype=='C' | outcome!='CYC')) + 
   geom_density(mapping=aes(x=finnrnodes, color=machinetype)) + facet_wrap(~ outcome)
print(p)
dev.copy(png, filename="distn-endsize.png")
dev.off()
