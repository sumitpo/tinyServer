BIN=client server
CFLAGS += -w
LDFLAGS += -lpthread

all:${BIN}

client:client.o util.o threadPool.o
server:server.o util.o threadPool.o
clean:
	rm -f *.o ${BIN} test
