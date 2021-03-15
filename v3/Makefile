BIN=client server
LDFLAGS=-lpthread
CFLAGS += -w

all:${BIN}

client:client.o util.o
server:server.o util.o
test:test.o util.o
clean:
	rm -f *.o ${BIN} test
