TARGETS=proxy
CFLAGS=-std=gnu++17 -pedantic -Werror -Wall -ggdb -pthread
LDFLAGS=-pthread
OBJS=Proxy.o HTTPRequest.o Cache.o HTTPResponse.o Log.o TimeParser.o

all: $(TARGETS)
clean:
	rm -f *~ $(TARGETS) $(OBJS)

proxy: $(OBJS)
	g++ -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	g++ $(CFLAGS) -c $<
