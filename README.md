# Compressed Network Communication

Application to demonstrate what was learned during Operational Systems classes with [Herbert Rocha](https://github.com/hbgit).

This application connects client and server using a socket, and a terminal to the server using a pipe. The client can pass commands to execute on the server's terminal, and then the output will be received by the client. It is similar to a chat app.

Made by [Eduardo Henrique](https://github.com/ed-henrique), [Rosialdo Vidinho](https://github.com/Rosialdo) and [Venícius Jacob](https://github.com/veniciusjacob).

## Table of Contents

- [Compressed Network Communication](#compressed-network-communication)
  - [Table of Contents](#table-of-contents)
  - [Goals](#goals)
  - [Problems Encountered](#problems-encountered)
    - [Passing SIGINT signal from client to server to kill terminal process in pipe](#passing-sigint-signal-from-client-to-server-to-kill-terminal-process-in-pipe)
    - [Solved Problems](#solved-problems)
  - [Limitations](#limitations)
  - [Presentation](#presentation)
  - [References](#references)

## Goals

| Status | Goal                     | Description                           |
| :----: | :----------------------: | :------------------------------------ |
| ✅     | Support logging          | use --log to log to an specified file |
| ✅     | Support host change      | use --host to change default host     |
| ✅     | Support port change      | use --port to change default port     |
| ✅     | Support data compression | use --compress to compress data       |
| ✅     | Support multiple clients |                                       |

## Problems Encountered

### Passing SIGINT signal from client to server to kill terminal process in pipe

- We don't know how to pass the signal, although killing the process is fairly easy;

### Solved Problems

- **First command from client always comes with some weird characters**

> Solution:
>
> We don't know exactly what changed, but it suddenly works. Even though fflush(stdin) is there, it was always there to begin with, so it's weird that now it simply works.

- **Can only read either first or last line from pipe output**

> Solution:
>
> Change a line to line approach using fgets() to a read the file as a whole approach using fread().

- **Using popen() instead of forking (requested by professor)**

> Implemented a solution using fork(), pipe() and execl(). To get output from execl, dup2() was used;

## Limitations

- Can't use any command that requests user input, e.g., htop and sudo (if not root);
- Passing SIGINT signal from client to server to kill terminal process in pipe;
- There is a limit to the number of clients that the server can support at once.

## Presentation

- [Report](https://github.com/ed-henrique/EduardoRosialdoVenicius-_FinalProject_OS_RR_2022/blob/main/presentation/Projeto%20Final%20SO.pdf)
- [Canva Presentation (with effects)](https://www.canva.com/design/DAFHpdmghg8/GNoinHOJhnPx7lMZw-pLQw/view)

## References

- [Chatroom in C](https://github.com/nikhilroxtomar/Chatroom-in-C) by [Nikhil Toma r](https://github.com/nikhilroxtomar)
