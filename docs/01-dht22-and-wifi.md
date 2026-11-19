# 01 – DHT22 and wifi

The first day was about getting the ESP32 to read the DHT22 and get onto the network.

## Wiring

I started out in Wokwi, an online simulator, to figure out how to wire the DHT22 before touching the real board. That helped me understand the idea, but the pin layout in the simulator was not the same as on my real board (a 38-pin ESP-32S). I counted pins the way the simulator showed them and ended up on the wrong pin, so the sensor only gave me `NaN`.

What finally worked:

| DHT22 | ESP32 |
|-------|-------|
| `+`   | 3V3 |
| `out` | GPIO 27 |
| `−`   | GND |

I also had the ESP32 crash in a boot loop and get warm. The reason was the breadboard: every hole in the same row is connected, and I had put the DHT22's pins side by side in one row. That shorted 3V3 to GND. Turning the module so each pin got its own row fixed it.

## The code

I wrote the sketch in Arduino (C++). It does two things:

1. **Connects to wifi.** The ESP32 joins my phone's hotspot and prints its IP address in Serial Monitor. The wifi name and password are in `secrets.h`, which is in `.gitignore`, so they never end up on GitHub. If one of them is missing, the sketch refuses to compile and tells me which one.
2. **Reads the DHT22.** It reads temperature (Celsius and Fahrenheit) and humidity and prints them. If the reading fails (`isnan`), it skips that round.

## Things that went wrong

- The ESP32 only talks 2.4 GHz wifi.
- Calling `WiFi.reconnect()` too often interrupts the connection attempt that is already running, so it never gets to finish.

First readings: around 24.8 °C and 49 % humidity.
