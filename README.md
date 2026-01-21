<header>

# Sedereal time
</header>
Calculate sedereal time from NET time

## reason why
Astronomers need to find the position of objects in the sky. Though the position of objects is known , they are not easy to find as the earth is rotating.

For calculating the exact posotions of where my telecope was looking at I needed: Azimut, Altitude, location, exact time. 
I have already fitted my dobson telscope with Azumut and Altitude sensors. Also locatiob (longitide and latitude) and position of the earth rotation. This position is determined by sedereal time. 
"Sidereal time is a "time scale that is based on Earth's rate of rotation measured relative to the fixed stars".
https://en.wikipedia.org/wiki/Sidereal_time#:~:text=Sidereal%20time%20is%20a%20%22time,axis%20relative%20to%20the%20stars.
A sidereal day on Earth is approximately 86164.0905 seconds (23 h 56 min 4.0905 s or 23.9344696 h).
 This project is to determine sedereal time 
 
 **##HowW**  
 Net time has as reference 1970-01-01 00:00:00 hrs and keepps track of seconds since that starting point.
 At  https://aa.usno.navy.mil/data/siderealtime we can get the exact sedereal time at the reference time
 With also the ration sedereal day / (julian) day 
 we can calculate sedereal time now

**##Procedure**
 - 0 preparation:
     Set fixed parameters:
         sederal day / julian day ratio
         sedereal time at net reference date-time
     Define struct sedereal ( decimal, hour, minute, second) to store found valus 
 - 1 Get GMT time from the internet
     use the epoch time = seconds passed since 1970-01-01 00:00:00 hrs
 - 2 convert epoch time to decimal days
 - 3 remove whole days, remain only fraction
 - 4 convert to decimal hours  = decimal sedereal time
 - 5 extract hours, minutes and seconds

**Limitations**
application is now for ESP8266 but can easy be adapted for other microProcessors
For accurate calculations we need 32 bit(long)  integer and double float. 
Be aware that ATmega328P based systems handle long int and double as int and float and lack accuracy 
