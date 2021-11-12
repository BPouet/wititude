/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
  Modified by BP
*********/

//Version by BP
//V1 = WEBSERVER with BME280
//V2 = SET WIFI TO ACCESS POINT
//V3 = WORKS AND REFRESH EVERYSECONDE
//V4 = GIVES MAX ALTITUDE
//V5 = ADAFRUIT MPU6050 support
//V5 = Original MPU6050 LIBRARY because adafruit mix gyro and accel
//V6 = IT WORKS BUT INNACURATE
//V7 = ADD BATTERY VOLTAGE READ

// The BME280 library was not at adress X76, I also had to remove the CHIPID in thr MASTER FILE.


//  SD2 = D2 on ESP8266
//  SCL = D1 on ESP8266

// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <ESPAsyncWebServer.h>


// Ajust base pression
#define SEALEVELPRESSURE_HPA (1013.25)  

//Définir MAX ALTITUDE AND SPEED
float startAltitude = 0;
float maxAltitude;
int buzzer = 6; //Pin for buzzer
float RatioFactor=7.33;  //Resistors Ration Factor 26k + 10k resistor with 12volt = 5.714  IN MY CASE 100K + 10K = 7.33  
int BAT= A0;              //Analog channel A0 as used to measure battery voltage

Adafruit_BME280 bme;

// Set your access point network credentials
const char* ssid = "ROCKET-Access-Point";
const char* password = "rocket";


// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

void setup() 
{
  Serial.begin(115200);
  bool status;

  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
    delay(10);
    
    }else{
      Serial.println("BMP280 init ok!");
    }

  
  // Setting the ESP as an access point
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
  
  startAltitude = bme.readAltitude(1013.25);
  float maxAltitude = (bme.readAltitude(1013.25) - startAltitude);

  /* Default settings from datasheet. */
  bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                    Adafruit_BME280::SAMPLING_X2,   // temperature
                    Adafruit_BME280::SAMPLING_X16,   // pressure
                    Adafruit_BME280::SAMPLING_NONE, // humidity
                    Adafruit_BME280::FILTER_X16,
                    Adafruit_BME280::STANDBY_MS_0_5 );
  
}
void loop(){

  int value = LOW;
  float Tvoltage=0.0;
  float Vvalue=0.0,Rvalue=0.0;
  
  //detect apogee
  float A = (bme.readAltitude(1013.25) - startAltitude);
      if ( A > maxAltitude) {
        maxAltitude = A;           //save highest altitude
  }

    /////////////////////////////////////Battery Voltage//////////////////////////////////  
  for(unsigned int i=0;i<10;i++){
  Vvalue=Vvalue+analogRead(BAT);         //Read analog Voltage
  delay(5);                              //ADC stable
  }
  Vvalue=(float)Vvalue/10.0;            //Find average of 10 values
  Rvalue=(float)(Vvalue/1024.0)*5;      //Convert Voltage in 5v factor
  Tvoltage=Rvalue*RatioFactor;          //Find original voltage by multiplying with factor
    /////////////////////////////////////Battery Voltage//////////////////////////////////
  
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.print("<meta http-equiv=\"refresh\" content=\"1\">");
            //le 1 est une seconde de refresh
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the table 
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial;}");
            client.println("table { border-collapse: collapse; width:35%; margin-left:auto; margin-right:auto; }");
            client.println("th { padding: 12px; background-color: #0043af; color: white; }");
            client.println("tr { border: 1px solid #ddd; padding: 12px; }");
            client.println("tr:hover { background-color: #bcbcbc; }");
            client.println("td { border: none; padding: 12px; }");
            client.println(".sensor { color:white; font-weight: bold; background-color: #bcbcbc; padding: 1px; }");
            
            // Web Page Heading
            client.println("</style></head><body><h1>ROCKET LIVE DATA</h1>");
            client.println("<table><tr><th>MESURE</th><th>VALEUR</th></tr>");

            client.println("<tr><td>Altitude</td><td><span class=\"sensor\">");
            client.println(bme.readAltitude(1013.25) - startAltitude);
            client.println(" m</span></td></tr>"); 

            client.println("<tr><td>Max Altitude</td><td><span class=\"sensor\">");
            client.println(maxAltitude);
            client.println(" m</span></td></tr>");
            
           client.println("<tr><td>Battery</td><td><span class=\"sensor\">");
           client.println(Tvoltage);
           client.println(" V</span></td></tr>"); 

           if(Tvoltage<=6){
           client.println("Battery dead OR disconnected</span></td></tr>");  
            }
           else if(Tvoltage>6 && Tvoltage<=8){
            client.println("Battery Low</span></td></tr>");  
            }
           else if(Tvoltage>8 && Tvoltage<=9){
            client.println("Battery Ok</span></td></tr>");  
            }
           else{
            client.println(" Battery Full</span></td></tr>");  
            }
            
        //   client.println("<tr><td>Max Speed</td><td><span class=\"sensor\">");
        //   client.println(maxspeed);
           //client.println(" km/h</span></td></tr>");  

           client.println("<tr><td>Temp. Celsius</td><td><span class=\"sensor\">");
           client.println(bme.readTemperature());
           client.println(" *C</span></td></tr>");       
            
            client.println("</body></html>");
          
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
