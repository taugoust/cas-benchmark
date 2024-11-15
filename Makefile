bin=cas.$(shell uname -m)

all: $(bin)

$(bin): cas.c
	$(CC) -O3 -lm -pthread -fno-PIE -no-pie -o $@ $<

clean:
	rm -rf *~ *.o

mrproper: clean
	rm -rf $(bin)

.PHONY: all clean mrproper
