TARGETS=Proxy
CFLAGS=-std=gnu++11 -pedantic -Werror -Wall -ggdb
OBJS=$(patsubst %,%.o,$(TARGETS)) Socket.o

all: $(TARGETS)
clean:
	rm -f *~ $(TARGETS) $(OBJS)

Proxy: Proxy.o Socket.o
	g++ -o $@ $^

%.o: %.cpp
	g++ $(CFLAGS) -c $<
