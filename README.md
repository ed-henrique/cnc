# Compressed Network Communication

Application to demonstrate what was learned during Operational Systems classes with Herbert Rocha.

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

-  Support more than one client during runtime
-  Use zlib to compress output from server (--compress)
- 🚧 Support --host
- 🚧 Use pipe to pass commands from server
- ✅ Support --log
- ✅ Support --port
- ✅ Pass input from client to server through socket

---

## Problems Encountered

Problems:

1. Using popen() instead of forking;
2. First command from client always comes with some junk;
3. Can only read either first or last line from pipe output;

Possible Solutions:

1. [Read Data From Pipe in C](https://zditect.com/guide/c/pipe-in-c.html);
2. Flush stdin before taking input;
3. Read pipe char by char, passing them to buffer;

## Demo

WIP

---

## Presentation

WIP

---

## References

WIP

---