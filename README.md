# Compressed Network Communication

Application to demonstrate what was learned during Operational Systems classes with [Herbert Rocha](https://github.com/hbgit).

</br>

This application connects client and server using a socket, and a terminal to the server using a pipe. The client can pass commands to execute on the server's terminal, and then the output will be received by the client. It is similar to a chat app.

</br>

Made by [Eduardo Henrique](https://github.com/ed-henrique), [Rosialdo Vidinho](https://github.com/Rosialdo) and [Venícius Jacob](https://github.com/veniciusjacob).

</br>

---

## Table of Contents

- [Goals](#goals)
- [Problems Encountered](#problems-encountered)
- [Limitations](#limitations)
- [Demo](#demo)
- [Presentation](#presentation)
- [References](#references)

---

## Goals

✅ means done.

🚧 means doing.

❌ means won't do.

- ✅ Support --log
- ✅ Support --host
- ✅ Log buffer before and after compression
- ✅ Support --port for both server and client
- ✅ Support more than one client during runtime
- ✅ Pass input from client to server through socket
- ✅ Use fork, pipe and execl to pass commands from server
- ✅ Stop client and disconnect from server if an EOF (^D) is found
- ✅ Use zlib to compress input from client and output from server (--compress)
- ❌ Process kills (^C) sent by the client on the server's side
- ❌ Use multi buffer system for better memory allocation

---

## Problems Encountered

### Problems:

1. **Passing SIGINT signal from client to server to kill terminal process in pipe;**

Possible Solutions:

1. We don't know how to pass the signal, although killing the process is fairly easy;

### Solved Problems:

1. **First command from client always comes with some junk**
    - Solution = We don't know exactly what changed, but it suddenly works. Even though fflush(stdin) is there, it always was there in the first place, so its weird that now it simply works.

2. **Can only read either first or last line from pipe output**
    - The solution was to change a line to line approach using fgets() to a read the file as a whole approach using fread().

3. **Using popen() instead of forking**
    - Implemented a solution using fork(), pipe() and execl(). To get output from execl, dup2() was used;

---

## Limitations

- Can't use interactive terminal commands (anything using sudo, htop, etc.).
- There is a limit to the number of clients that the server can support at once.

---

## Demo

### WIP

---

## Presentation

[WIP - Canva]()

---

## References

### WIP

---