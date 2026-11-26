# Modbus response

```
Sending Modbus Request..
Modbus buffer:
00 01 00 00 00 0B 01 03 08 04 D2 00 00 00 D3 02 0B
```

This is the first result i got from the program, just the raw bytes.

## Reading the response

Now i need a for loop to print all the values and the hexadecimals from the responseBuffer. `valread` is how many bytes `read()` put in the responseBuffer, so the loop goes to `valread` and prints every byte with `%02X`.

Each group of two hex digits is one byte.

- The mbap header is 7 bytes. i = 0 -> 00 | i = 6 -> 01. So these are 7 elements = 7 bytes.
- The function code is i = 7 -> 03. 1 byte
- i = 8 -> 08 = 1 byte. This is a byte count, that counts the databytes which will follow. I have 4 registers, so 2 bytes x 4 registers are 8 bytes.
- From i = 9 -> 04 are the 4 registers from the esp32. 04D2 = testvalue, 0000 = uptime, 00D3 = temperature, 020B = humidity.

So the response in total has 7 (mbap header) + 1 (function code) + 1 (byte count) + 8 bytes from the different registers = 17 bytes in total.

## From bytes to values

The first result I got from the program was just raw bytes. Before I knew what to do next, I first had to understand what I was actually reading. There is a large comment in the code, and the reason I wrote it was that it helped me reason through the code and what I was actually reading. When it all made sense, I figured out really fast how to write the code, since I understood what I was actually looking at.
Then I had to figure out how to shift the high byte to the front and put the low byte at the back, and then print the values.

Shifting them was not hard for this project, since one byte is two hex digits, and in the Modbus protocol the high byte always comes first. So I just shifted it 8 places to the left, and used `|` (OR) to combine it with the low byte, so all its bits are copied in. And now I knew exactly what each element had and what it stood for, so it was really easy to write the result.

```c
uint16_t testValue = (responseBuffer[9] << 8 | responseBuffer[10]);
```
High byte first (big-endian): shift it 8 bits to the left, then OR in the low byte, so both end up in one 16 bit value. OR gives 1 if either side is 1, and the lower 8 bits of the shifted byte are all 0, so the low byte is copied in unchanged.

- testValue, i = 9 and 10.
- upTime, i = 11 and 12. Right now they are just 00 00, I haven't written the code in the arduino.
- temperature, i = 13 and 14. This is signed since the temperature can have negative values.
- humidity, i = 15 and 16. This is unsigned since humidity cannot be negative.

Temperature and humidity are divided by 10.0 and printed with `%.1f`.

## Result

```
Sending Modbus Request..
Modbus buffer:
00 01 00 00 00 0B 01 03 08 04 D2 00 00 00 DA 02 00
1234
0
21.8
51.2
```

**I am actually very pleased with the result, especially for not using libmodbus or any other library, and actually solving this with raw sockets, a request array and a response buffer.**

**I have gotten so much out of this. Being able to manipulate bits, understanding how TCP works under the hood, Modbus itself, and how to read and understand a protocol feels really good. It has been very rewarding, and it has been a long time since I have felt this kind of mastery. I had to figure out what hardware I needed, and at the same time build a server on the ESP32 with the right power and everything.**

**This has been one of the best projects I have worked on. It might not be big compared to what they do in industry, but it is a step in the right direction.**
