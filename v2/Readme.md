# version 2

version 2 tcpserver use forking child process to process the client input,
and we handle the following three questions.

- Catching the SIGCHLD signal when forking child processes.
- Handling interrupted system calls when catch signals.
- A SIGCHLD handler must be coded correctly using waitpid to prevent any
zombies from being left around.
