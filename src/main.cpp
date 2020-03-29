/*
  WriteMultipleFields
  
  Description: Writes values to fields 1,2,3,4 and status in a single ThingSpeak update every 20 seconds.
  
  Hardware: Arduino WiFi Shield
  
  !!! IMPORTANT - Modify the secrets.h file for this project with your network connection and ThingSpeak channel details. !!!
  
  Note:
  - Requires the WiFi library
  - Ensure the WiFi Shield has the latest firmware. Instruction to update can be found here: https://www.arduino.cc/en/Hacking/WiFiShieldFirmwareUpgrading
  - This example is written for a network using WPA encryption. For WEP or WPA, change the WiFi.begin() call accordingly.
  
  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize, and 
  analyze live data streams in the cloud. Visit https://www.thingspeak.com to sign up for a free account and create a channel.  
  
  Documentation for the ThingSpeak Communication Library for Arduino is in the README.md folder where the library was installed.
  See https://www.mathworks.com/help/thingspeak/index.html for the full ThingSpeak documentation.
  
  For licensing information, see the accompanying license file.
  
  Copyright 2018, The MathWorks, Inc.
*/

#include <Arduino.h>
#include "ThingSpeak.h"
#include <WiFi101.h>
#include "secrets.h"

#define myLED 6

char ssid[] = SECRET_SSID;    //  your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// Initialize our values
int sensorPin = A0;
int sensorValue = 0;
int led;
float temperature = 0.0;
float voltage = 0.0;


String myStatus = "";

void setup() {
  pinMode(myLED,OUTPUT);
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  analogReadResolution(10); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  // set the fields with the values
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, led);
  
  // figure out the status message
  if(voltage < 760){
    myStatus = String("Temperature OK"); 
  }
  else if(voltage >= 760 & voltage < 780){
    myStatus = String("TEMPERATURE RISING");
  }
  else if(voltage > 780){
    myStatus = String("HIGH TEMPERATURE");
  }
  
  // set the status
  ThingSpeak.setStatus(myStatus);
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  // get temperature reading
  sensorValue = analogRead(sensorPin);
  voltage = sensorValue * (3300 / 1024);
  Serial.print("voltage = ");
  Serial.print(voltage);
  temperature = (voltage - 500) / 10;
  Serial.print("temperature = ");
  Serial.println(temperature);
  delay(100);
  if (temperature >= 28){
    digitalWrite(myLED,HIGH);
    led = 1;
  }
  else {
    digitalWrite(myLED,LOW);
    led = 0;
  }    
  
    delay(20000); // Wait 20 seconds to update the channel again
}