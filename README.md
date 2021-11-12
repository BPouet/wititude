# Wititude
Altimeter with WIFI AP  

====
This altimeter is of a NODE MCU 8266 with BMP280.  
The objective is to activate the altimeter, launch the rocket and when you find it, you connect to wifi to get MAX ALTITUDE.  
I was tired of opening the payload to check the altimeter each time and had problem with other wireless module.
The project is made to be inexpensive.
I did try to put a MPU6050but data was innacurate. Also test to get battery voltage with 2 resistors but it was using too much power.

==================
WHAT YOU NEED  
- NODE MCU 8266
- BMP280
- LED
- BUZZER
- 9V BATTERY or POWER BOOST to 5V if you use LIPO 3.7v

LIBRARIES
- ESP8266WiFi.h  
- Wire  
- Adafruit_BMP280  
- Adafruit_sensor  
- ESPAsyncWebServer
(Some library are included with the board NODE MCU 8266. Make sure to install it in ARDUINO IDE)

WIFI is "Rocket Access Point"
PAssword is rocket
IP is 192.168.4.1

Connect with phone or pc and go to the ip in the web explorer.  
You shall get all the info via wifi  

Cheers!




REFERENCES
Rui Santos - Complete project details at https://randomnerdtutorials.com  
