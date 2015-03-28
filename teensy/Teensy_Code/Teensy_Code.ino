#define SD_CS 20

#include <SdFat.h>
#include "Wire.h"
#include "I2Cdev.h"
#include "BMP085.h"

BMP085 pressure;
SdFat sd;
SdFile file;
double accel[3], gyro[3], mag[3];
double lat = 0.0, lng = 0.0;
double temp, pres, alt;
boolean fix;
String reply = "";

void setup(){
	Wire.begin();
	Serial.begin(57600);
	Serial1.begin(9600);        //GPS
	Serial2.begin(57600);       //IMU
	Serial3.begin(57600);

	pressure.initialize();
	delay(4000);
	Serial.println(pressure.testConnection() ? "BMP085 connection successful" : "BMP085 connection failed");
	if (!sd.begin(SD_CS, SPI_HALF_SPEED)){
		Serial.println("SD Card Failed!");
		//delay(5000);
	}

        pinMode(13, OUTPUT);
        Serial2.print("#4");
}

void loop(){
	readIMU();
	readPressure();
	printData();
	//writeSDData();
	/*if(Serial3.available() >= 2){
		if(Serial3.read() == '#'){
			char c = Serial3.read();
			if(c == 'g')
				readGPS();
			else if(c == 'd')
				transmitSDData();
		}
	}*/
	delay(100);
}

void readPressure(){
	pressure.setControl(BMP085_MODE_TEMPERATURE);
	//delay(1);
	temp = pressure.getTemperatureC();
	pressure.setControl(BMP085_MODE_PRESSURE_3);
	//delay(20);
	pres = pressure.getPressure();
	alt = pressure.getAltitude(pres); 
}

void writeSDData(){
	if (!file.open("data.txt", O_RDWR | O_CREAT | O_AT_END)) {
		Serial.println("File failed to open!!!");
		delay(3000);
	} else {
		file.print(accel[0]); file.print(",");
		file.print(accel[1]); file.print(",");
		file.print(accel[2]); file.print(",");
		file.print(gyro[0]); file.print(",");
		file.print(gyro[1]); file.print(",");
		file.print(gyro[2]); file.print(",");
		file.print(mag[0]); file.print(",");
		file.print(mag[1]); file.print(",");
		file.print(mag[2]); file.print(",");
		file.print(temp); file.print(",");
		file.print(pres); file.print(",");
		file.print(alt); file.println("");
		file.close();
	}
}

void transmitSDData() {
	//implement QT handshaking
	if (!file.open("data.txt", O_RDWR)) {
		Serial.println("File failed to open!!!");
		delay(3000);
	}
	int data;
	while ((data = file.read()) >= 0)
		Serial3.write(data);
	file.close();
}

void readIMU() {
        digitalWrite(13, HIGH);
        while (Serial2.available()) {
                Serial2.read();
        }
        delay(10);
        while (Serial2.available()) {
              unsigned char c = Serial2.read();
              if (c == '$') break;
        }
        digitalWrite(13, LOW);
	accel[0] = Serial2.parseFloat();
	accel[1] = Serial2.parseFloat();
	accel[2] = Serial2.parseFloat();
	gyro[0] = Serial2.parseFloat();
	gyro[1] = Serial2.parseFloat();
	gyro[2] = Serial2.parseFloat();
	mag[0] = Serial2.parseFloat();
	mag[1] = Serial2.parseFloat();
	mag[2] = Serial2.parseFloat();
}

void printData() {
	Serial3.print(accel[0]); Serial3.print(",");
	Serial3.print(accel[1]); Serial3.print(",");
	Serial3.print(accel[2]); Serial3.print(",");
	Serial3.print(gyro[0]); Serial3.print(",");
	Serial3.print(gyro[1]); Serial3.print(",");
	Serial3.print(gyro[2]); Serial3.print(",");
	Serial3.print(mag[0]); Serial3.print(",");
	Serial3.print(mag[1]); Serial3.print(",");
	Serial3.print(mag[2]); Serial3.print(",");
	Serial3.print(temp); Serial3.print(",");
	Serial3.print(pres); Serial3.print(",");
	Serial3.print(alt); Serial3.println("");
}

bool validFix(String s) {
	if (s[19] != ',')   // Check the GPRMC string for Fix
		return true;
	else
		return false;
}

void readGPS() {
	fix = false;
	while(!fix){
		Serial.println("loop");
		readGPSData();
		Serial.println(reply);
		if(reply.substring(0,5) == "GPRMC"){
			Serial.println(reply);
			if(validFix(reply)){
				String latitude = reply.substring(18,27);
				String longitude = reply.substring(31,41);
			
				char latbuf[latitude.length()], lngbuf[longitude.length()];
				latitude.toCharArray(latbuf,latitude.length());
				longitude.toCharArray(lngbuf,longitude.length());
			
				convertLatLngToDecimal(latbuf, lngbuf);
				if (reply[29] == 'S')
				 lat = (-1)*lat;
		
				if (reply[42] == 'W')
					 lng = (-1)*lng;
				fix = true;
			}
		}
	}
	Serial.println("FIX!!!!!");
	Serial.print("Lat: ");
	Serial.println(lat);
	Serial.print("Long: ");
	Serial.println(lng);
}

void convertLatLngToDecimal(char latbuf[], char lngbuf[])
{
	double d = atof(latbuf);
	lat = (int) (d/100);
	lat += (d-(lat*100))/60;
	
	d = atof(lngbuf);
	lng = (int) (d/100);
	lng += (d-(lng*100))/60;
}

void readGPSData(){
	char c[100] = "";
	if (Serial1.available())
	{
		while (Serial1.read() != '$') { }
		Serial1.readBytesUntil('\r',c,100);
	}
	reply = c;
}
