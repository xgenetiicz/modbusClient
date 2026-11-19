# 03 – Modbus TCP server

This is where the ESP32 becomes a Modbus TCP server, so my C client can ask it for the readings.

## Switching libraries

I started with Arduino's own `ArduinoModbus` library. With that one I had to do more myself: create a `WiFiServer` on port 502, accept the client, hand it over to the Modbus server and call `poll()` while the client was connected. That was a good way to see what actually happens.

The problem was that `ArduinoModbus` pulls in `ArduinoRS485` (Modbus over a serial cable), and that library does not support the ESP32. My sketch would not even compile:

```
'SERIAL_PORT_HARDWARE' was not declared in this scope
```

So I switched to `modbus-esp8266` by emelianov, which works on the ESP32. It turned out to be simpler: the library runs the TCP server on port 502 itself, so I only need to:

- start the server with `server()`
- create the registers with `addHreg()`
- update them with `Hreg()`
- call `task()` in the loop, which accepts clients and answers their requests

## Registers

This is what made it click for me: think of the registers as an array.

```c
uint16_t registers[4];
```

Each register is 16 bits (2 bytes), and the address is just the index, starting at 0. I have 4 registers because I have 4 values to share, not because of the 16 bits:

| Register | Value |
|----------|-------|
| 0 | Test value, always 1234 |
| 1 | Uptime (not done yet) |
| 2 | Temperature × 10 |
| 3 | Humidity × 10 |

`addHreg(0, 1234)` is the same idea as `registers[0] = 1234`.

Register 0 never changes. If the client reads 1234, I know the whole chain works. 1234 is also `0x04D2` in hex, so if I mix up the byte order I get `0xD204` = 53764 and notice right away.

## Why I typecast

A register can only hold a whole number, but the DHT22 gives me a `float`. So I multiply by 10 first (22.6 °C becomes 226) and then turn it into an integer.

I also use `roundf()` before the cast. A float can't store 22.6 exactly, it might be 22.59999. Times 10 that is 225.9999, and a cast always cuts instead of rounding, so I would send 225, which is wrong. `roundf()` gives 226. It does not round the temperature, it only removes the float noise.

## Signed and unsigned

- **Temperature** is cast to `int16_t` (signed), because it can go below zero. −5.0 °C becomes −50, which is `0xFFCE` as bits.
- **Humidity** is cast to `uint16_t` (unsigned), because it is always between 0 and 100 %, so 0 to 1000.

Modbus itself does not know about signs. A register is just 16 bits. It's the client that decides whether to read them as signed or unsigned, based on the register map. That's why the register map matters so much.
