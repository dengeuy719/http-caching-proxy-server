TARGETS=Proxy
CFLAGS=-std=gnu++14 -pedantic -Werror -Wall -ggdb
OBJS=$(patsubst %,%.o,$(TARGETS)) Socket.o HTTPRequest.o

all: $(TARGETS)
clean:
	rm -f *~ $(TARGETS) $(OBJS)

Proxy: $(OBJS)
	g++ -o $@ $^

%.o: %.cpp
	g++ $(CFLAGS) -c $<
