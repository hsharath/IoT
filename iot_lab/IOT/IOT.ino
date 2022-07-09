#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Wire.h>              // Wire library (required for I2C devices)
#include <Adafruit_BMP280.h>   // Adafruit BMP280 sensor library
 
#include "index.h" //Our HTML webpage contents with javascripts
#include "DHTesp.h" //DHT11 Library for ESP
 
#define LED 2 //On board LED
#define DHTpin 14 //D5 of NodeMCU is GPIO14
 
// define device I2C address: 0x76 or 0x77 (0x77 is library default address)
#define BMP280_I2C_ADDRESS  0x77
// initialize Adafruit BMP280 library
Adafruit_BMP280  pressure;
 
#define ALTITUDE 1655.0 // Altitude in meters
 
DHTesp dht;
 
//SSID and Password of your WiFi router
const char* ssid = "RITSTUDENTS";
const char* password = "ritstudents";
 
ESP8266WebServer server(80); //Server on port 80
 
void handleRoot() {
String s = MAIN_page; //Read HTML contents
server.send(200, "text/html", s); //Send web page
}
 
float humidity, temperature;
 
void handleADC() {
char status;
double T,P,p0,a;
double Tdeg, Tfar, phg, pmb;
 

T = pressure.readTemperature();
P = pressure.readPressure();
 // Print out the measurement:
Serial.print("temperature: ");
Serial.print(T,2);
Tdeg = T;
Serial.print(" deg C, ");
Tfar = (9.0/5.0)*T+32.0;
Serial.print((9.0/5.0)*T+32.0,2);
Serial.println(" deg F");


/// Print out the measurement:
Serial.print("absolute pressure: ");
Serial.print(P,2);
pmb = P;
Serial.print(" mb, ");
phg = P*0.0295333727;
Serial.print(P*0.0295333727,2);
Serial.println(" inHg");
 
//p0 = pressure.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
//Serial.print("relative (sea-level) pressure: ");
//Serial.print(p0,2);
//Serial.print(" mb, ");
//Serial.print(p0*0.0295333727,2);
//Serial.println(" inHg");
// 
a = pressure.readAltitude();
Serial.print("computed altitude: ");
Serial.print(a,0);
Serial.print(" meters, ");
Serial.print(a*3.28084,0);
Serial.println(" feet");

 
int rain = 268-analogRead(A0);
 
//Create JSON data
String data = "{\"Rain\":\""+String(rain)+"\",\"Pressuremb\":\""+String(pmb)+"\",\"Pressurehg\":\""+String(phg)+"\", \"Temperature\":\""+ String(temperature) +"\", \"Humidity\":\""+ String(humidity) +"\"}";
 
digitalWrite(LED,!digitalRead(LED)); //Toggle LED on data request ajax
server.send(200, "text/plane", data); //Send ADC value, temperature and humidity JSON to client ajax request
 
delay(dht.getMinimumSamplingPeriod());
 
humidity = dht.getHumidity();
temperature = dht.getTemperature();
 
Serial.print("H:");
Serial.println(humidity);
Serial.print("T:");
Serial.println(temperature); //dht.toFahrenheit(temperature));
Serial.print("R:");
Serial.println(rain);
}
 
void setup()
{
Serial.begin(115200);
Serial.println("Setup initiated");
 
// dht11 Sensor
 
dht.setup(DHTpin, DHTesp::DHT11); //for DHT11 Connect DHT sensor to GPIO 17
pinMode(LED,OUTPUT);
 
////BMP280 Sensor
if (pressure.begin(BMP280_I2C_ADDRESS))
Serial.println("BMP280 init success");
else
{
Serial.println("BMP280 init fail\n\n");
//while(1); // Pause forever.
}
 
WiFi.begin(ssid, password); //Connect to your WiFi router
Serial.println("");
 
// Wait for connection
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
 
//If connection successful show IP address in serial monitor
Serial.println("");
Serial.print("Connected to ");
Serial.println(ssid);
Serial.print("IP address: ");
Serial.println(WiFi.localIP()); //IP address assigned to your ESP
 
server.on("/", handleRoot); //Which routine to handle at root location. This is display page
server.on("/readADC", handleADC); //This page is called by java Script AJAX
 
server.begin(); //Start server
Serial.println("HTTP server started");
}
 
void loop()
{
server.handleClient(); //Handle client requests
}
