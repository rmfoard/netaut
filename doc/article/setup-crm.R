library("RPostgreSQL")
library("tidyverse")
pg = dbDriver("PostgreSQL")
con = dbConnect(pg, user="richard", port=5432, dbname="crm")
print("Remember to set the search_path to ... if necessary.")
