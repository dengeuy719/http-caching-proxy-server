TARGETS=Proxy
CFLAGS=-std=gnu++14 -pedantic -Werror -Wall -ggdb -I /opt/homebrew/Cellar/boost/1.81.0_1/include -L /opt/homebrew/Cellar/boost/1.81.0_1/lib -lboost_filesystem -lboost_system
OBJS=$(patsubst %,%.o,$(TARGETS)) Proxy.o HTTPRequest.o

all: $(TARGETS)
clean:
	rm -f *~ $(TARGETS) $(OBJS)

Proxy: $(OBJS)
	g++ -o $@ $^

%.o: %.cpp
	g++ $(CFLAGS) -c $<
