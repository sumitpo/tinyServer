BIN=client server serverPs
CFLAGS += -w

all:${BIN}

client:client.o util.o signals.o
server:server.o util.o signals.o
serverPs:serverPs.o util.o signals.o
test:test.o util.o
clean:
	rm -f *.o ${BIN} test
