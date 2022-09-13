bin=cas.$(shell uname -m)

all: $(bin)

$(bin): cas.c
	g++ -O3 -lm -pthread -o $@ $<

clean:
	rm -rf *~ *.o

mrproper: clean
	rm -rf $(bin)

.PHONY: all clean mrproper
