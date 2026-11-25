# TCP socket client

Before I could send any Modbus I needed a client that can talk TCP. So this is the first version of my client, before I started with the Modbus bytes.

I used the example from GeeksforGeeks as a starting point (https://www.geeksforgeeks.org/c/socket-programming-cc/) and changed it so it fits my project. The biggest change was that the IP address comes from the command line instead of being hardcoded.

## How to run it

```
gcc -Wall -Wextra -o modbus main.c
./modbus <ip-address>
```

## Arguments

`argv[0]` is always the program name, so the IP ends up in `argv[1]` and `argc` is 2.

I check `argc` before doing anything else:
- if there are more than 2 arguments the program says it only takes an IP address and stops
- if there are less than 2 it asks for the IP of the Modbus server and stops

This way I don't have to change the code every time the ESP32 gets a new IP on another network.

## The steps

1. `socket()` creates the socket. `client_fd` is the number that holds the socket. `AF_INET` is for IPv4 addresses and `SOCK_STREAM` is TCP, so a reliable connection.
2. Then I fill in the address in `struct sockaddr_in`:
   - `memset` sets everything to 0 first so there is no random stuff left in memory
   - `sin_family` is `AF_INET`
   - `sin_port` is 502, because that's the default Modbus port. `htons()` = host to network short, it puts the port in network byte order (big-endian, high byte first)
   - `sin_addr` is the IP from `argv[1]`. `inet_pton()` = presentation to network, it converts the IP from text to binary. If the IP is not valid the program stops.
3. `connect()` connects to the server.
4. `send()` sends a test message, "Hello from Client!".
5. `read()` reads the answer into a buffer.
6. `close()` closes the socket.

If something fails the program prints what went wrong and returns -1.

## What I learned

- I don't write TCP myself. The operating system does that, I just use it through `socket`, `connect`, `send`, `read` and `close`.
- The ESP32 is the server and my PC is the client. The ESP32 waits on port 502 and my client connects and asks for the values.
- Numbers bigger than one byte are sent with the high byte first. This is important later for all the Modbus fields.

## Why the hello message can't stay

The hello message is text, and I used `strlen()` for the length and `printf("%s")` to print the answer. That works for text but not for Modbus:
- the Modbus request has a lot of `0x00` bytes, and `strlen()` stops counting at the first `0x00`
- `printf("%s")` also stops at `0x00`, so the answer would not show
- a Modbus server doesn't understand text anyway, it just closes the connection