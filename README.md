# modbusClient

A small project where I'm building my own tool for talking to hardware over Modbus TCP.

An ESP32 with a DHT22 sensor measures the temperature and humidity in my room and exposes the values as Modbus registers. The ESP32 runs 
Arduino (C++), so I have real hardware to test against. The client on my laptop is going to be written in C by looking at Beej's Guide.
Later the readings will be stored in InfluxDB within a docker container, that is the goal.

The point is to understand what happens between two devices on a network: sockets, bytes, big-endian, and the Modbus protocol itself.

```
DHT22  →  ESP32 (Modbus TCP server)  →  C client on my laptop  →  InfluxDB. 
```

Later I might send the data to my home server through Tailscale, but I’ll see if that’s needed.

There are also photos included of the hardware with the Oled! Please look them up at: [Photos](/Photos/fullBuild.jpeg)

## Status

- [x] ESP32 reads the DHT22 and connects to wifi
- [ ] ESP32 runs a Modbus TCP server
- [ ] C client reads the registers 
- [ ] Readings stored in InfluxDB

## Hardware

- ESP32 dev board (ESP-32S, 38 pins)
- DHT22 module (3 pins: `+`, `out`, `−`)
- Breadboard and jumper wires
- SSD1306 OLED

## Register map

| Register | Value |
|----------|-------|
| 0 | Test value, always 1234 |
| 1 | Uptime |
| 2 | Temperature × 10 (235 = 23.5 °C) |
| 3 | Humidity × 10 (451 = 45.1 %) |

**C client:**

```
gcc -Wall -Wextra -o modbus main.c
./modbus
```
