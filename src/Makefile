CFLAGS=-Wall -Wextra -g -O3
CPPFLAGS=-I../include/
LDFLAGS=
HEADPATH=../include/

.PHONY=all help clean

all:../takuzu

../takuzu : utils.o takuzu.o grid.o euristic.o backtracking.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

backtracking.o: backtracking.c $(HEADPATH)backtracking.h $(HEADPATH)euristic.h $(HEADPATH)grid.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

euristic.o: euristic.c $(HEADPATH)euristic.h $(HEADPATH)grid.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

utils.o : utils.c $(HEADPATH)utils.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

grid.o : grid.c $(HEADPATH)grid.h $(HEADPATH)utils.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@	

takuzu.o : takuzu.c $(HEADPATH)takuzu.h $(HEADPATH)utils.h $(HEADPATH)grid.h $(HEADPATH)euristic.h $(HEADPATH)backtracking.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

help:
	@echo "Usage :"
	@echo "'make all' to compile everything"
	@echo "'make takuzu' to create takuzu executable file"
	@echo "'make clean' to remove object file"

clean:
		rm *.o
