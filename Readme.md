# simple tcpserver

this is a simple tcp server when I learn the book Unix Network Programming
Volumn1: The Sockets Networking API.

## The first version:
tcpserver is based on simple socket api, and disconnect after send the
client current time.

## the second version:
the second version is based on simple socket api, and disconnect after send the
client current time.


## The third version:
the third version is based on multi thread, every connection is handled by a thread

## The forth version:
the forth version is based on single process and select, listen file descriptor and
connected file descriptors are handled by the select.

## The fifth version:
the fifth version is based on single process and epoll.

## The sixth version:
the sixth version is based on thread pool and epoll, new connection and connected ones
are handled by epoll, process of client message is delivered to work queue and threads
get the jobs.

## compilation and test
to compile the server, simple run `make`.

to run the server, choose a port and run `./server [port]`.

the client is not implemented, to test the server, run `nc [ip of the server]
[port of the server]`

The server is tested in Manjaro 20.2.1 Nibia, x86_64 Linux 5.10.19, other
systems have not been tested yet.
