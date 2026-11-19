# 02 – OLED

I added a small SSD1306 OLED screen (128×64) so I can see the readings without having the laptop connected.

## Wiring

The OLED talks I2C, which uses two wires: SDA (data) and SCK (clock). On the ESP32 these are GPIO 21 and GPIO 22 by default.

| OLED | ESP32 |
|------|-------|
| VDD  | 3V3 (through the + rail on the breadboard) |
| GND  | GND (through the − rail) |
| SCK  | GPIO 22 |
| SDA  | GPIO 21 |

The ESP32 only has one 3V3 pin, so I use the breadboard rails to share power between the DHT22 and the OLED. I learned from the DHT22 mistake and turned the OLED so each pin has its own row.

## The code

I used the `Adafruit_SSD1306` library (with Adafruit GFX). The screen's I2C address is `0x3C`. `&Wire` tells the library which I2C bus the screen is on.

Nothing shows up on the screen until `display()` is called. Everything before that is only drawn in memory.

The screen shows:

- the ESP32's IP address, so I know where my client should connect, since it can change every time the ESP32 joins the hotspot
- the temperature
- the humidity

`setTextSize(1, 2)` gives normal width and double height. That way the IP address fits on one line and the text is still easy to read.
