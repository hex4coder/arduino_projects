/**
 * Complete project details at https://RandomNerdTutorials.com/esp8266-load-cell-hx711/
 * 
 * HX711 library for Arduino - example file
 * https://github.com/bogde/HX711
 *
 * MIT License
 * (c) 2018 Bogdan Necula
 *
**/

#include <Arduino.h>
#include "HX711.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = D5;
const int LOADCELL_SCK_PIN = D6;

HX711 scale;


/* Put your SSID & Password */
const char* ssid = "SmartScaller.Net";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

/* Put IP Address details */
// IPAddress local_ip(192,168,1,1);
// IPAddress gateway(192,168,1,1);
// IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

float berat = 0;
float calibration_factor = -109525;

// String retBerat() {
//   return String("<h1>") + String(berat) + String("</h1>");
// }

void handle_web() {
  if(berat < 0) {
    berat = berat * -1;
  } // absolute function
  server.send(200, "text/plain", String(berat, 3)); 
}

void handle_tare() {
  scale.tare();
  server.send(200, "text/plain", "ok");
}

void setup() {
  Serial.begin(115200);
  Serial.println("HX711 Demo");
  Serial.println("Initializing the scale");

  WiFi.softAP(ssid, password);
 // WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_web);
  server.on("/reset", handle_tare);
  server.begin();
  Serial.println("HTTP server started");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  Serial.println("HX711 Calibration");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press a,s,d,f to increase calibration factor by 10,100,1000,10000 respectively");
  Serial.println("Press z,x,c,v to decrease calibration factor by 10,100,1000,10000 respectively");
  Serial.println("Press t for tare");
  scale.set_scale();
  scale.tare(); //Reset the scale to 0
 
  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
}

void loop() {
  server.handleClient();

  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  berat = scale.get_units(5);
 
  Serial.print("Reading: ");
  Serial.print(berat, 3);
  Serial.print(" kg"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();
 
  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor += 10;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 10;
    else if(temp == 's')
      calibration_factor += 100;  
    else if(temp == 'x')
      calibration_factor -= 100;  
    else if(temp == 'd')
      calibration_factor += 1000;  
    else if(temp == 'c')
      calibration_factor -= 1000;
    else if(temp == 'f')
      calibration_factor += 10000;  
    else if(temp == 'v')
      calibration_factor -= 10000;  
    else if(temp == 't')
      scale.tare();  //Reset the scale to zero
  }

  //berat = scale.read();
  
}