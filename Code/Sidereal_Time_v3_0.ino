/*  
  ****************************************
Copyright (c) GitHub, Inc. Permission is hereby granted, free of charge, to any person obtaining 
a copy of this software and associated documentation files (the “Software”),  to deal in the Software 
without restriction, including without limitation the rights to use, copy, modify, merge, publish, 
distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software 
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS 
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE
  *****************************************
Objective: For users of a dobson telecope fitted with electronic Azimut and Altitude sensors
App calculates selesteal time used for calculatio Right Acention and Declination from Azimut, Altitude, longitude and latitude.

0 preparation: 
  Set fixed parameters: 
    - sideral day / julian day ratio 
    - reference sidereal time at 01-01-2026
    - time betweeen epoch time 01-01-1970 and 01-01-2026
  Define struct sidereal ( decimal, hour, minute, second) to store found valus
1 Get Epoch time = seconds passed since 1970-01-01 00:00:00 hrs
2 convert epoch time to decimal days
3 remove whole days, remain only fraction
4 convert to decimal hours = decimal sidereal time
5 extract hours, minutes and seconds

Limitations application is now for ESP8266 but can easy be adapted for other microProcessors 
For accurate calculations we need 32 bit(long) integer and double float. Be aware that ATmega328P based systems handle long int and double as int and float and lack accuracy
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#
// Set up UDP for NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // UTC timezone, update every 60 seconds
// WiFi credentials
const char* ssid = "telenet-450A4";
const char* password = "BryonyatParis";

// global struct sidereal time
struct sedTime {
  double dec; 
  int hour; 
  int min;
  int sec;
  };

sedTime sidereal;

// ********************************************
void setup() {
   Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.printf("Connecting to %s\n", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("\nConnected! IP address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();

}
// ********************************************
void loop() {
  calculatesiderealTime();
  
    Serial.printf("Sidereal time = %d:%02d:%02d", sidereal.hour, sidereal.min, sidereal.sec);
    Serial.println( "   decimal time = " + String(sidereal.dec) );
 
  delay(1000);
}

void calculatesiderealTime() {
  // temporary string for formatted sereal output
  char tempStr[30];

  // *************  sidereal reference data data lolcal used only
  // https://www.epochconverter.com/ : 
  // Epoch timestamp              @01-01-2026): 
  unsigned long  refEpochTime = 1767139200; //  sec
  // https://www.walter-fendt.de/html5/aen/siderealtime_en.htm
  // Greenwich Mean Sidereal Time @2026-01-01 6 h 42 min 38.6 s = 6.71072 h
  // double refsiderealTime = 6.71072222222222;  // hour
  double refsiderealTime = 6.64527777777778;

  // https://en.wikipedia.org/wiki/Sidereal_time : sidereal day = 23h 56min 4.0905s
  // siderealFactor: sidereal_day / julian_day = 0.997269565972222
  // v2.0-->  double sideralFacor     = 0.9972695648031844;  
  double sideralFacor     = 0.997269565972222;  
  //double sideralFacor     = 0.9972695648031844;  
  //  value calibrated to 1 sec dev with reference https://www.localsiderealtime.com/  18-1-2026
  // *******************************************

  // update time from NTP if needed
  timeClient.update(); 

  // seconds elapsed since start epoch time 00:00:00 UTC on 1 January 1970 
  unsigned long epochTime = timeClient.getEpochTime();
  //    Serial.println("Epoch time (sec):" + String(epochTime) );
  // calculate hours since sidereal reference dat-time

  // Caculate sidereal time 
  
  double dDays = ( double(epochTime - refEpochTime) /sideralFacor ) /3600/24; 
  // add sidereal time (hours) at reference time 1970 in decimal days
  dDays = dDays + refsiderealTime/24;  
  //    snprintf(tempStr, sizeof(tempStr), "Days since 1-1-1970: %lf",dDays);
  //    Serial.println( tempStr);

  // calculate (decimal) hours, min and seconds from double dDays
  double dHours = (dDays  - floor(dDays)  )*24; // decimal required to calculate minutes
  double dMin   = (dHours - floor(dHours) )*60; // decimal required to calculate seconds
  double dSec   = (dMin   - floor(dMin)   )*60;

  // convert to integer hours min and sec
  sidereal.dec  = dHours; 
  sidereal.hour = int(dHours);
  sidereal.min  = int(dMin);
  sidereal.sec  = int(dSec); 
}
