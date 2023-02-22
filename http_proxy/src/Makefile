TARGETS=Proxy
CFLAGS=-std=gnu++14 -pedantic -Werror -Wall -ggdb -pthread
LDFLAGS=-pthread
OBJS=$(patsubst %,%.o,$(TARGETS)) Proxy.o HTTPRequest.o

all: $(TARGETS)
clean:
	rm -f *~ $(TARGETS) $(OBJS)

Proxy: $(OBJS)
	g++ -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	g++ $(CFLAGS) -c $<
