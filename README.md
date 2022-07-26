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

- Stop both server and client if an EOF (^D) is found
- Process kills (^C) sent by the client on the server's side
- 🚧 Support more than one client during runtime
- 🚧 Use fork, pipe and execl to pass commands from server
- ✅ Support --log
- ✅ Support --host
- ✅ Log buffer before and after compression
- ✅ Support --port for both server and client
- ✅ Pass input from client to server through socket
- ✅ Use zlib to compress input from client and output from server (--compress)
- ❌ Use multi buffer system for better memory allocation

---

## Problems Encountered

### Problems:

1. **Using popen() instead of forking;**
2. **Killing terminal process using ^C**

Possible Solutions:

1. [Read Data From Pipe in C](https://zditect.com/guide/c/pipe-in-c.html);
1. Implement Problem 1 solution;

### Solved Problems:

1. **First command from client always comes with some junk;**
    - Solution = We don't know exactly what changed, but it suddenly works. Even though fflush(stdin) is there, it always was there in the first place, so its weird that now it simply works.

2. **Can only read either first or last line from pipe output;**
    - The solution was to change a line to line approach using fgets() to a read the file as a whole approach using fread().

---

## Limitations

- Can't use interactive terminal commands (anything using sudo, htop, etc.).

---

## Demo

### WIP

---

## Presentation

### WIP

---

## References

### WIP

---