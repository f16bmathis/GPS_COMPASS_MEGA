/* 
 * MEGA
 * RED TFT (ILI9341)
 * Pin19  to 4th pin on sensor
 * GPS Sensor GT-U7
 * Displays Compass and GPS headers, SEEMS TO WORK!
 * Changed KPH to MPH
 * Changed Altitude Meters to Feet
 * Added GPS Valid Gren/Red box ***ONLY valid if 
 * GPS SAT signal is lost, not if wire #19 is 
 * pulled out and Arduino is not receiving signals.***
 * Added Dist and Heading to Capitol Airport Waukesha WI (can be changed)
 * NEED TO FIX GPS TIME ******************************************************
*/

#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
static const int RXPin = 18, TXPin = 19;
static const uint32_t GPSBaud = 9600;
#include <Arduino.h>
#define USE_ADAFRUIT_SHIELD_PINOUT 1
#include <Adafruit_GFX.h> 
#include <Adafruit_Sensor.h> 
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;

TinyGPSPlus gps;          // May have GPS TIME calculations

#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// Assign names to common 16-bit color values:
#define   BLACK  0x0000
#define   BLUE   0x001F
#define   RED    0xF800
#define   GREEN  0x07E0
#define   CYAN   0x07FF
#define   YELLOW 0xFFE0
#define   ORANGE 0xFD20
#define   WHITE  0xFFFF

String    Time, Date;
float     NumberSats, Latitude, Longitude, Bearing;
float     AltitudeMETRES, AltitudeMILES, AltitudeKM, AltitudeFEET;
float     SpeedKPH, SpeedMPH, SpeedKNOTS, SpeedMPS;
//float     distanceToLondon;           /////////////////////////

const int centreX  = 230; // Location of the compass display on screen
const int centreY  = 120;
const int diameter = 70; // Size of the compass
int       dx = centreX, dy = centreY;
int       last_dx = centreX, last_dy = centreY - diameter * 0.85;

// For stats that happen every 5 seconds
unsigned long last = 0UL;

void setup()

{
  Serial.begin(115200);
  Serial1.begin(9600); // connect gps sensor
  //ss.begin(GPSBaud);
  Serial.print("What the HECK!");

  tft.begin();            // Start the TFT display
  tft.setRotation(5);     // Rotate screen by 90°
  tft.setTextSize(2);     // Set medium text size
  tft.setTextColor(YELLOW);
  tft.fillScreen(BLACK);
}

void loop() {
  
  Latitude       = gps.location.lat();
  Longitude      = gps.location.lng();
  Date           = String(gps.date.day() < 10 ? "0" : "") + String(gps.date.day()) + "/" + String(gps.date.month() < 10 ? "0" : "") + String(gps.date.month()) + "/" + String(gps.date.year());
  // Date           = String(gps.date.month()<10?"0":"") + String(gps.date.month()) + "/" + String(gps.date.day()<10?"0":"") + String(gps.date.day()) + "/" + String(gps.date.year());
  Time           = String(gps.time.hour() < 10 ? "0" : "")   + String(gps.time.hour())   + ":" +
                   String(gps.time.minute() < 10 ? "0" : "") + String(gps.time.minute()) + ":" +  String(gps.time.hour() < 10 ? "0" : "") + String(gps.time.second() < 10 ? "0" : "") + String(gps.time.second());
  Bearing        = gps.course.deg();
  SpeedKPH       = gps.speed.kmph();
  SpeedMPH       = gps.speed.mph();
  SpeedKNOTS     = gps.speed.knots();
  SpeedMPS       = gps.speed.mps();
  NumberSats     = gps.satellites.value();
  AltitudeMETRES = gps.altitude.meters();
  AltitudeKM     = gps.altitude.kilometers();
  AltitudeMILES  = gps.altitude.miles();
  AltitudeFEET   = gps.altitude.feet();
  

// ********* GPS GOOD? This works ONLY if SAT signal is lost NOT if Pin19 is pulled out *******
if (gps.satellites.value ()> 3 )
   tft.fillRect(0, 0, 30, 8 * 4, GREEN); 
else   
   tft.fillRect(0, 0, 30, 8 * 4, RED);     //left,top,right,down * 4?, Green

// *** Adding Dist & Course to Capitol *******************************************
if (gps.location.isValid())
{
  static const double CAPITOL_LAT = 43.089619, CAPITOL_LON = -88.179440;
      double distanceToLondon =
        TinyGPSPlus::distanceBetween(
          gps.location.lat(),
          gps.location.lng(),
          CAPITOL_LAT, 
          CAPITOL_LON);
      double courseToLondon =
        TinyGPSPlus::courseTo(
          gps.location.lat(),
          gps.location.lng(),
          CAPITOL_LAT, 
          CAPITOL_LON);

      Serial.print(F("CAPITOL Distance="));
      Serial.print(distanceToLondon/1000*.6213712, 2);        // *0.6213712  *** change from KM o miles!!!!!!!!!
      Serial.print(F(" MILES Course-to="));
      Serial.print(courseToLondon, 6);
      Serial.print(F(" degrees ["));
      Serial.print(TinyGPSPlus::cardinal(courseToLondon));
      Serial.println(F("]"));
      tft.setTextColor(CYAN);
      tft.setCursor(150, 0);                                // Sets it back to not print over "Capitol"
      tft.println(TinyGPSPlus::cardinal(courseToLondon));   // Prints "SE" or cardinal heading on TFT
      tft.setCursor(190, 0);
      tft.println(distanceToLondon/1000*.6213712, 1);       // Prints Distance to capitol
      tft.setCursor(235, 0);
      tft.print("Miles");
  }
// end Addding Distance & Heading to Capitol

      
  while (Serial1.available() > 0)
      gps.encode(Serial1.read());
  if  (gps.location.isUpdated())

  {
  Serial.print(F("LOCATION   Fix Age="));
    Serial.print("***************");
    Serial.print(gps.location.isUpdated());
    Serial.print(gps.location.age());
    Serial.print(F("ms Raw Lat="));
    Serial.print(gps.location.rawLat().negative ? "-" : "+");
    Serial.print(gps.location.rawLat().deg);
    Serial.print("[+");
    Serial.print(gps.location.rawLat().billionths);
    Serial.print(F(" billionths],  Raw Long="));
    Serial.print(gps.location.rawLng().negative ? "-" : "+");
    Serial.print(gps.location.rawLng().deg);
    Serial.print("[+");
    Serial.print(gps.location.rawLng().billionths);
    Serial.print(F(" billionths],  Lat="));
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(" Long="));
    Serial.println(gps.location.lng(), 6);
  }

  Serial.println("Time\t\tDate\t\tLAT\tLON\tSATS\tAlt\tBearing\tSpeed(KPH)");
  Serial.println("----------------------------------------------------------------------------------");
  Serial.print(Time                   + "\t");
  Serial.print(Date                   + "\t");
  Serial.print(String(Latitude, 3)    + "\t");
  Serial.print(String(Longitude, 3)   + "\t");
  Serial.print(String(NumberSats)     + "\t");
  Serial.print(String(AltitudeMETRES) + "\t"); // Select as required
  Serial.print(String(Bearing)        + "\t");
  Serial.print(String(SpeedKPH)       + "\t"); // Select as required
  Serial.print(String(SpeedMPH)       + "\t"); // Select as required
  //Serial.print(String(distanceToLondon) +  "\t"); ///////////////////////////////
  Serial.println("\n");
  DisplayGPSdata(NumberSats, Latitude, Longitude, AltitudeMETRES, SpeedKPH, Bearing); // Select units as required
  smartDelay(1000);
  if (millis() > 5000 && gps.charsProcessed() < 10)  Serial.println(F("No GPS data received: check wiring"));
}
//#####################################################################
void DisplayGPSdata(float dNumberSats, float dLatitude, float dLongitude, float dAltitude, float dSpeed, float dBearing) {
  
  PrintText(20, 0, "  CAPITOL", CYAN, 2);
  //PrintText(40, 0, "distanceToLondon:" + String(),2, 2);        //////////////
  tft.fillRect(45, 40, 90, 19 * 4, BLACK);
  PrintText(0, 45, "LAT:" + String(dLatitude), YELLOW, 2);
  PrintText(0, 63, "LON:" + String(dLongitude), YELLOW, 2);
  PrintText(0, 81, "ALT:" + String(dAltitude * 3.28084, 1) + " FT", YELLOW, 2);
  PrintText(0, 99, "SAT:" + String(dNumberSats, 0), YELLOW, 2);
  tft.fillRect(80, 220, 120, 18, BLACK);
  PrintText(10, 220, "Speed:" + String(dSpeed / 1.609) + " Mph", YELLOW, 2);
  tft.fillRect(240, 220, 120, 18, BLACK);
  tft.setCursor(200, 220);
  tft.print("Azi: " + Bearing_to_Ordinal(dBearing));
  Display_Compass(dBearing);
  Display_Date_Time();
}
//#####################################################################
void Display_Compass(float dBearing) {
  int dxo, dyo, dxi, dyi;
  tft.setCursor(0, 0);
  tft.drawCircle(centreX, centreY, diameter, WHITE); // Draw compass circle
  for (float i = 0; i < 360; i = i + 22.5) {
    dxo = diameter * cos((i - 90) * 3.14 / 180);
    dyo = diameter * sin((i - 90) * 3.14 / 180);
    dxi = dxo * 0.9;
    dyi = dyo * 0.9;
    tft.drawLine(dxo + centreX, dyo + centreY, dxi + centreX, dyi + centreY, WHITE);
  }
  PrintText((centreX - 5), (centreY - diameter - 18), "N", GREEN, 2);
  PrintText((centreX - 5), (centreY + diameter + 5) , "S", GREEN, 2);
  PrintText((centreX + diameter + 5),  (centreY - 5), "E", GREEN, 2);
  PrintText((centreX - diameter - 15), (centreY - 5), "W", GREEN, 2);
  dx = (0.85 * diameter * cos((dBearing - 90) * 3.14 / 180)) + centreX; // calculate X position
  dy = (0.85 * diameter * sin((dBearing - 90) * 3.14 / 180)) + centreY; // calculate Y position
  draw_arrow(last_dx, last_dy, centreX, centreY, 5, 5, BLACK);   // Erase last arrow
  draw_arrow(dx, dy, centreX, centreY, 5, 5, YELLOW);           // Draw arrow in new position
  last_dx = dx;
  last_dy = dy;
}
//#####################################################################
void draw_arrow(int x2, int y2, int x1, int y1, int alength, int awidth, int colour) {
  float distance;
  int dx, dy, x2o, y2o, x3, y3, x4, y4, k;
  distance = sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
  dx = x2 + (x1 - x2) * alength / distance;
  dy = y2 + (y1 - y2) * alength / distance;
  k = awidth / alength;
  x2o = x2 - dx;
  y2o = dy - y2;
  x3 = y2o * k + dx;
  y3 = x2o * k + dy;
  x4 = dx - y2o * k;
  y4 = dy - x2o * k;
  tft.drawLine(x1, y1, x2, y2, colour);
  tft.drawLine(x1, y1, dx, dy, colour);
  tft.drawLine(x3, y3, x4, y4, colour);
  tft.drawLine(x3, y3, x2, y2, colour);
  tft.drawLine(x2, y2, x4, y4, colour);
}
//#####################################################################
void Display_Date_Time() {
  PrintText(0, 150, "Date/Time:", CYAN, 2);
  tft.fillRect(0, 165, 130, 19 * 2, BLACK);
  PrintText(0, 168, Time, GREEN, 2);
  PrintText(0, 188, Date, GREEN, 2);
}
//#####################################################################
String Bearing_to_Ordinal(float bearing) {
  if (bearing >= 348.75 || bearing < 11.25)  return "N";
  if (bearing >=  11.25 && bearing < 33.75)  return "NNE";
  if (bearing >=  33.75 && bearing < 56.25)  return "NE";
  if (bearing >=  56.25 && bearing < 78.75)  return "ENE";
  if (bearing >=  78.75 && bearing < 101.25) return "E";
  if (bearing >= 101.25 && bearing < 123.75) return "ESE";
  if (bearing >= 123.75 && bearing < 146.25) return "SE";
  if (bearing >= 146.25 && bearing < 168.75) return "SSE";
  if (bearing >= 168.75 && bearing < 191.25) return "S";
  if (bearing >= 191.25 && bearing < 213.75) return "SSW";
  if (bearing >= 213.75 && bearing < 236.25) return "SW";
  if (bearing >= 236.25 && bearing < 258.75) return "WSW";
  if (bearing >= 258.75 && bearing < 281.25) return "W";
  if (bearing >= 281.25 && bearing < 303.75) return "WNW";
  if (bearing >= 303.75 && bearing < 326.25) return "NW";
  if (bearing >= 326.25 && bearing < 348.75) return "NNW";
  return "?";
}

//#####################################################################
void PrintText(int x, int y, String text, int colour, byte text_size) {
  tft.setCursor(x, y);
  tft.setTextColor(colour);
  tft.setTextSize(text_size);
  tft.print(text);
  tft.setTextColor(YELLOW); // Default colour
  tft.setTextSize(2);       // Default Text Size
}
//#####################################################################
static void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (Serial1.available()) gps.encode(Serial1.read());
  } while (millis() - start < ms);
}
