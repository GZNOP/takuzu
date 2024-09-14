.PHONY=all help clean report cleanreport

all:
	make -C ./src/ all
help:
	@echo "'make rep' : compile the report"
	@echo "'make cleanreport' : clean the compilation file of the report"
	make -C ./src/ help
rep: 
	make -C ./report/ report

cleanreport:
	make -C ./report/ clean

clean:
	make -C ./src/ clean;

