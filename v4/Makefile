BIN=client server
CFLAGS += -w

all:${BIN}

client:client.o util.o
server:server.o util.o
serverPs:serverPs.o util.o
clean:
	rm -f *.o ${BIN} test
