# Mingw64 Kernel Sockets

Requires [mingw-w64-dpp](https://github.com/utoni/mingw-w64-dpp) and Windows 10. Windows 11 may or may not work, not tested yet.

Did you ever ask yourself the question: how-the-hell can I open a socket in kernel mode with a Mingw-w64 compiled driver?
Well, this is the solution. `mingw-w64-ksocket` simplifies the use of stream/datagram client/server sockets.
The API is similiar to the BSD socket API.
Greetings to [KSOCKET](https://github.com/wbenny/KSOCKET) for this sweet approach.

Compatible with C and C++ drivers.

# HowTo

Build it:

```shell
make DPP_ROOT="[path-to-mingw-w64-dpp-template-dir]" all

```

To install & (self-)sign the driver:

```shell
make DPP_ROOT="[path-to-mingw-w64-dpp-template-dir]" DESTDIR="[path-to-install-dir]" install
```

The directory `[path-to-install-dir]` should now contain three new files:

 * `driver.bat` / `driver-protobuf-c.bat`: setup the driver service, start it, stop it when it's done and delete it
 * `driver.sys`: example driver that uses kernel sockets
 * `userspace_client.exe`: example userspace application which communicates with the driver via TCP socket
 * `driver-protobuf-c.sys`: example driver that make use of protobuf-c (local, no TCP/IP)

Start `driver.bat` as `Administrator` and then `userspace_client.exe`.

If everything works fine, there should be a text displayed in `userspace_client.exe` console window, received from the driver.

For more debug output, it is recommended to use a debugger or log viewer like `dbgview`.
