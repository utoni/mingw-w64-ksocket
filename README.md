# Mingw64 C++ Kernel Sockets

Requires [mingw-w64-dpp](https://github.com/utoni/mingw-w64-dpp).

Did you ever ask yourself the question: how-the-hell can I open a socket in kernel mode with a Mingw-w64 compiled driver?
Well, this is the solution. `mingw-w64-ksocket` simplifies the use of stream/datagram client/server sockets.
The API is similiar to the BSD socket API.
Greetings to [KSOCKET](https://github.com/wbenny/KSOCKET) for this sweet approach.

# HowTo

```shell
make -C . DPP_ROOT="[path-to-mingw-w64-ddk-template-dir]" clean all
```
