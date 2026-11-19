# 04 – millis() instead of delay()

## The problem

My loop used `delay(3000)` so the DHT22 had time between readings (it needs about 2 seconds).

But `delay()` stops the whole ESP32. During those 3 seconds it does nothing, not even answer Modbus requests. So a client asking for the values could have to wait up to 3 seconds, even though the last reading was already sitting in the registers.

## The fix

I used the same pattern as Arduino's BlinkWithoutDelay example, with two global variables:

```c
unsigned long previousMillis = 0;  // when the DHT22 was last read
const long interval = 3000;        // 3 seconds between readings
```

They have to be global, otherwise they would reset every time `loop()` runs.

Every loop:

1. `modbus.task()` runs, so the server can answer right away.
2. I save `currentMillis = millis()`, which is how many milliseconds have passed since the ESP32 started.
3. If `currentMillis - previousMillis >= interval`, at least 3 seconds have passed since the last reading. Then I read the DHT22, update the registers and the OLED, and set `previousMillis = currentMillis`.

So `currentMillis` is not about when Modbus polls. It's the clock I check every loop to decide if it's time to read the sensor again. Modbus gets to run every loop no matter what.

## Result

The server answers immediately, and the sensor is still only read every 3 seconds.
