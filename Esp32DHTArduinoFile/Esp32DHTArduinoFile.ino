#include <WiFiClientSecure.h>
#include <WiFi.h>
#include "secrets.h" //not for you
#include <DHT.h> //library for dht
#include <Adafruit_SSD1306.h> // This library is especially for the oled i have.
#include <ModbusIP_ESP8266.h> // so the lbrary from the documentation does not support my esp32.

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


/*So on setup, i will also start the server there
by using ModbusTCPServer.begin(); but i want also to create a object of it so i can check if the server is working.
I also need to create a designated port for it so it can reach the protocol

EDIT: No need for starting the server, the library has the wifiserver default on port 502
*/ 
ModbusIP modbus;

void setup() {

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

    //begin the server
    modbus.server();
    Serial.printf("Modbus TCP Server started\n");

    //So the new way is to call the object and create the registers
    modbus.addHreg(0,1234); //test value
    modbus.addHreg(2,0); //temperature
    modbus.addHreg(3,0); //humidity
    modbus.addHreg(1,0); //uptime
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.printf("Not connected, status: %d\n", WiFi.status());
    WiFi.reconnect();
    delay(15000);
    return;
  }

  /*So i need to make a reference variable for the wifiserver so the esp32 understands that a client should connect
  and that is by using the reference wifiServer(502) that has the port
  So it should accept it and poll for modbus tcp requests while client is connected

  EDIT: This was the old library, the new one just ask for modbus.task() and accept the client and et cetera.
  */
  modbus.task();
  

  //Now if internet is reconnected, we will try to fetch temperature values from dht.
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
  oledDisplay.printf("Modbus:\n %s\n", WiFi.localIP().toString().c_str());
  oledDisplay.printf("%.1f Celcius\n", temperature);
  oledDisplay.printf("%.1f %% Humidity\n", humidity);
  oledDisplay.display();
}
