#include <WiFiClientSecure.h>
#include <WiFi.h>
#include "secrets.h" //not for you
#include <DHT.h> //library for dht
#include <Adafruit_SSD1306.h> // This library is especially for the oled i have.

//need to define values here so the esp32 can connect to internet
//wifi setup to for my hotspot, and i will use defines from my secret.h
//reason for this is beacuse i am going to push this to my repo on github.
#ifndef WIFI_SSID
  #error "WIFI_SSID is not defined! Please check the secrets file.";
#endif

//The reason for both is that i want to know what is missing if something is.
#ifndef WIFI_PASS
  #error "WIFI_PASS is not defined! Please check the secrets file.";
#endif


/*Now defines the pin to dht22!
So my dht is connected to gpio pin 4 on esp32. OUT is connected to it.
OUT is the actual data transfer that esp32 receives, where i will then retrieve 
the values from the esp32.
*/

#define DHTPIN 27 //this is the pin on esp32 for receiving data
#define DHTTYPE DHT22 // defining the sensor as DHT22.

/*
So defining the oled after defining the dht22
*/
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C // 0011 1101 in a 8 bit but found out that arduino is a 7 bit system????

Adafruit_SSD1306 oledDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //&Wire is scl and sda pins on the oled.


//Now create an instance of DHT with the reference as dht that holds two parameters, the pin and the type value of the sensor
DHT dht (DHTPIN, DHTTYPE);


void setup() {
  //Debugging: It would not connect, and i am wondering if it doesnt find the hotspot. so i want to see that it did find with an for loop
  WiFi.mode(WIFI_STA);
  int networks = WiFi.scanNetworks();
  Serial.printf("Found %d networks:\n",networks);
  for (int i = 0; i < networks; i++) {
    Serial.printf("  %s (%d dBm)\n", WiFi.SSID(i).c_str(), WiFi.RSSI(i));
  }

  Serial.begin(115200);
  if (!oledDisplay.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
  Serial.printf("OLED not found\n"); //
  }
  oledDisplay.clearDisplay();
  oledDisplay.setTextColor(SSD1306_WHITE);
  oledDisplay.display();
  dht.begin(); //we begin the sensor.
  Serial.printf("\nEsp32 will now try to connect to wifi client:%s \n",WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("Connecting to Wifi..\n");
    while(WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.printf("Not connected yet\n");
    }
    Serial.printf("Wifi succesfully connected to: %s \n", WIFI_SSID);
    Serial.printf("IP Address is: %s \n",WiFi.localIP().toString().c_str()); // need to add format specifires because of getting ip to string, and the format specifier for string is %s.
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.printf("Not connected, status: %d\n", WiFi.status());
    WiFi.reconnect();
    delay(15000);
    return;
  }
  //Now if interned is reconnected, we will try to fetch temperature values from dht.
  //And ofc, we need the values stored as float
  delay(3000); // delay this by three seconds so the dht can get the temperature
  float temperature = dht.readTemperature(); // i thought first this was get, but it is actually read. get is usually done with Java haha!
  float fahrenheit = dht.readTemperature(true); // need to pass as true so it can fetch the fahrenheit and not celcius twice.
  float humidity = dht.readHumidity();

  if(isnan(temperature) || isnan(fahrenheit) || isnan(humidity)) {
    Serial.printf("Failed to read from DHT22 Sensor, and could not retrieve data from temperature,fahrenheit and humidity\n");
    return;
  } 

  Serial.printf("Temperature in Celcius: %.1f\n", temperature); //so it's a format specifier for like instead of 22.222 celcius -> 22.1 Celcius. Defining one desimal after the whole number.
  Serial.printf("Temperature in Fahrenheit: %.1f\n", fahrenheit);
  Serial.printf("Humidity: %.1f\n", humidity);

  oledDisplay.clearDisplay();
  oledDisplay.setTextSize(1,2); // this is not too big or too small on the oled.
  oledDisplay.setCursor(0, 0);
  oledDisplay.printf("%s\n", WiFi.localIP().toString().c_str());
  oledDisplay.printf("\n");
  oledDisplay.printf("%.1f Celcius\n", temperature);
  oledDisplay.printf("%.1f %% Humidity\n", humidity);
  oledDisplay.display();
}
