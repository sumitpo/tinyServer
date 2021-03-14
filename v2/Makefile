BIN=client server server
CFLAGS += -w

all:${BIN}

client:client.o util.o signals.o
server:server.o util.o signals.o
test:test.o util.o
clean:
	rm -f *.o ${BIN} test
