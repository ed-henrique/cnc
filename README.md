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
- [Demo](#demo)
- [Presentation](#presentation)
- [References](#references)

---

## Goals

- Use multi buffer system for better memory allocation
- 🚧 Support more than one client during runtime
- 🚧 Use zlib to compress output from server (--compress)
- 🚧 Use fork, pipe and execl to pass commands from server
- ✅ Support --log
- ✅ Support --host
- ✅ Support --port
- ✅ Pass input from client to server through socket

---

## Problems Encountered

### Problems:

1. **Using popen() instead of forking;**

Possible Solutions:

1. [Read Data From Pipe in C](https://zditect.com/guide/c/pipe-in-c.html);

### Solved Problems:

1. **First command from client always comes with some junk;**
    - Solution = We don't know exactly what changed, but it suddenly works. Even though fflush(stdin) is there, it always was there in the first place, so its weird that now it simply works.

2. **Can only read either first or last line from pipe output;**
    - The solution was to change a line to line approach using fgets() to a read the file as a whole approach using fread().

## Demo

### WIP

---

## Presentation

### WIP

---

## References

### WIP

---