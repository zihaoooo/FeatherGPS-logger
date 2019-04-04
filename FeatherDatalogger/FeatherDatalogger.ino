//Libraries
#include <SPI.h> // SD card library
#include <SD.h>  // SD card library
#include <Adafruit_GPS.h> //GPS library
#include <Adafruit_Sensor.h>
#include <RTClib.h>
#include <math.h>;
#include <Wire.h>
#include <Adafruit_BME280.h>


//BEM280

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C


//GPS
#define GPSSerial Serial1 // name of the hardware serial port
Adafruit_GPS GPS(&GPSSerial); //connect to the GPS on the hardware port
#define GPSECHO false // Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console; Set to 'true' if you want to debug and listen to the raw GPS sentences
uint32_t timer = millis();
                                                   
const int chipSelect = 4; //SD chipselect pin

File dataFile;

void setup() 
{
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  
  bme.begin();  
  
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz
     
  // Request updates on antenna status, comment out to keep quiet
  //GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);

  //SD card 
  if (!SD.begin(chipSelect)) {
    Serial.println("card");
    while (1);
  }
 
  dataFile = SD.open("DATA.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Unixtime, Lat, Lon, Temperature, Humidity, Pressure"); //This is where you change the header based on the type of sensor you installed. 
    dataFile.flush();
    Serial.println("FileOpen");
  }
  else{
    while (1);
  } 
}

void loop()
{  

  char c = GPS.read();
//  if (GPSECHO)
//  if (c) Serial.print(c);

  
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
//    Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }

  if (timer > millis()) timer = millis();
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer

    if (GPS.fix) {  

          //BME280
          float temp = bme.readTemperature();
          float humidity = bme.readHumidity();
          float pressure = bme.readPressure() / 100.0F;

          //DATETIME          
          String datetime = ""; 
          DateTime dt (GPS.year, GPS.month, GPS.day, GPS.hour, GPS.minute, GPS.seconds);
          datetime += dt.unixtime();
          

          //PRINT        
          dataFile.print(datetime);
          dataFile.print(","); 
          dataFile.print(GPS.latitudeDegrees, 6);
          dataFile.print(",");
          dataFile.print(GPS.longitudeDegrees, 6);
          dataFile.print(",");
          dataFile.print(temp, 2);
          dataFile.print(",");
          dataFile.print(humidity,2);
          dataFile.print(",");
          dataFile.println(pressure,2);
          dataFile.flush(); 
          
          Serial.print(datetime);
          Serial.print(","); 
          Serial.print(GPS.latitudeDegrees, 6);
          Serial.print(",");
          Serial.print(GPS.longitudeDegrees, 6);
          Serial.print(",");
          Serial.print(temp, 2);
          Serial.print(",");
          Serial.print(humidity,2);  
          Serial.print(",");
          Serial.println(pressure,2);  
                                   
          pulse(1);
                           
    } 
    else {
    Serial.println("waiting GPS signal");
    pulse(1);pulse(1);pulse(1);
    } 
  }


}

void pulse(int pulseSpeed){
  for (int i=0;i<255;i++){
  analogWrite(13, i);
  delay(pulseSpeed);     
  }
  for (int i=255;i>=0;i--){
  analogWrite(13, i);
  delay(pulseSpeed);     
  }                          
}




 
