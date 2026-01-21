/*  date 2026-01-21
  ****************************************
Copyright (c) GitHub, Inc.
Permission is hereby granted, free of charge, to any person obtaining 
a copy of this software and associated documentation files (the “Software”), 
to deal in the Software without restriction, including without limitation the rights to use, 
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR 
THE USE OR OTHER DEALINGS IN THE SOFTWARE
  *****************************************
For calculating the exact posotions of where my telecope was looking at I needed: Azimut, Altitude, location, exact time. 
I have already fitted my dobson telscope with Azumut and Altitude sensors. 
 locatiob (longitide and latitude) and position of the earth rotation. 
 This position is determined by sedereal time. "Sidereal time is a "time scale that is based on Earth's rate of rotation 
 measured relative to the fixed stars
 ". https://en.wikipedia.org/wiki/Sidereal_time#:~:text=Sidereal%20time%20is%20a%20%22time,axis%20relative%20to%20the%20stars. 
 A sidereal day on Earth is approximately 86164.0905 seconds (23 h 56 min 4.0905 s or 23.9344696 h). 

0 preparation: Set fixed parameters: sederal day / julian day ratio sedereal time at net reference date-time 
  Define struct sedereal ( decimal, hour, minute, second) to store found valus
1 Get GMT time from the internet use the epoch time = seconds passed since 1970-01-01 00:00:00 hrs
2 convert epoch time to decimal days
3 remove whole days, remain only fraction
4 convert to decimal hours = decimal sedereal time
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

// global struct sedereal time
struct sedTime {
  double dec; 
  int hour; 
  int min;
  int sec;
  };

sedTime sedereal;

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
  calculateSederealTime();
    Serial.println( "Sedereal decimal time = " + String(sedereal.dec) );
    Serial.println( "Sedereal time = " + String(sedereal.hour) 
                                     + ":" + String(sedereal.min) 
                                     + ":" + String(sedereal.sec) );    
  delay(1000);
}

void calculateSederealTime() {
  // requires global struct Sedereal. {double decimal, int hour, int min, int sec}
  // temporary string for formatted sereal output
  char tempStr[30];

  // *************  sedereal reference data data lolcal used only
  // sedereal (decimal) time at start epoch time
  // https://www.phpsciencelabs.com/sidereal-time-calculator/index.php
  double sederealZeroTime = 6.6819758112 ; 
  // sederealFactor  = sedereal timeUnit / julian timeUnit
  // https://en.wikipedia.org/wiki/Sidereal_time
  double sederalFacor     = 0.9972695648031844; 
  //  value calibrated to 1 sec dev with reference https://www.localsiderealtime.com/  18-1-2026
  // *******************************************

  // update time from NTP if needed
  timeClient.update(); 

  // get time in hr min sec 
  int h = timeClient.getHours();
  int m = timeClient.getMinutes();
  int s = timeClient.getSeconds();
  // print Formated HH:MM:SS string
      snprintf(tempStr, sizeof(tempStr), "Julian time: %02d:%02d:%02d", h, m, s);
      Serial.println( tempStr );

  // seconds elapsed since start epoch time 00:00:00 UTC on 1 January 1970 
  unsigned long epochSeconds = timeClient.getEpochTime();
      Serial.println("Epoch time (sec):" + String(epochSeconds) );
  // Caculate Sedereal time 
  double sederealSeconds = double(epochSeconds) /sederalFacor; // sedereal day is shorter, nr of days longer
  double dDays = sederealSeconds/3600/24; 
  // add sedereal time (hours) at reference time 1970 in decimal days
  // adding as decimal day avoids check hours > 24
  dDays = dDays + sederealZeroTime/24; 
      snprintf(tempStr, sizeof(tempStr), "Days since 1-1-1970: %lf",dDays);
      Serial.println( tempStr);

  // calculate (decimal) hours, min and seconds from double dDays
  double dHours = (dDays  - floor(dDays)  )*24; // decimal required to calculate minutes
  double dMin   = (dHours - floor(dHours) )*60; // decimal required to calculate seconds
  double dSec   = (dMin   - floor(dMin)   )*60;

  // convert to integer hours min and sec
  sedereal.dec  = dHours; 
  sedereal.hour = int(dHours);
  sedereal.min  = int(dMin);
  sedereal.sec  = int(dSec); 
}
