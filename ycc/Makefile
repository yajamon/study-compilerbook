CFLAGS=-Wall -std=c11
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

ycc: $(OBJS)

$(OBJS): ycc.h

test: ycc
	./test.sh

clean:
	rm -f ycc *.o *~ tmp*
