# Modbus request

Today I built the Modbus request in my client. The protocol decides which fields there are, how big they are and in which order they come. So I just write the bytes in that order in a `uint8_t` array. One element is one byte, so 12 elements = 12 bytes.

I used this page to understand the fields: https://simplymodbus.ca/learn-tcp.html

## The 12 bytes

```
00 01  transaction id
00 00  protocol id (always 0 for Modbus)
00 06  length (the 6 bytes that come after this field)
01     unit id
03     function code (read holding registers)
00 00  start address
00 04  number of registers
```

## Function code

The function code doesn't say how many registers I want, it says which table to read from. `0x03` is holding registers, and that's where all 4 of my registers are on the ESP32 (`addHreg`). 0x04 is the input registers that i don't need to read.

## Response buffer

I changed the buffer from char to uint8_t and called it `responseBuffer`. The request only asks for the values, the values themselves come back in the response. But to retrieve the reponse, i need to retrieve them as integers, since the protocol is retrevied with the datatype of integers and not characters.

## Next
Send the request, read the answer into `responseBuffer` and print it byte by byte in hex.
