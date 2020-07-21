/*---------------------------------------------------------------------------------------------
MOTION CAPTURE NODE PCB TEST FOR ESP8266-01 & MPU6050
SENDS FOLLOWING VALUES OVER OSC:

- RAW DATA RECEIVED VIA I²C
- DATA NORMALIZED ACCORDING TO DATASHEET AND  SELECTED SCALE RANGE

AUTHOR: ROST LIKHOLAT
THIS EXAMPLE CODE IS IN THE PUBLIC DOMAIN
--------------------------------------------------------------------------------------------- */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <Wire.h>

const IPAddress targetIp(192, 168, 178, 75); //  OSC receiver IP
const unsigned int targetPort = 6666;        // remote OSC receiver port
const unsigned int localPort = 8888;         // local port to listen for OSC packets (not used for sending)
const int nodeID = 0;
WiFiUDP Udp; // UDP instance

// I2C address of the MPU-6050 - 0x68 or 0x69 if AD0 is pulled HIGH
const int MPU = 0x68;
float accelCoeficient = 16384.0;
float gyroCoeficient = 131.0;

int16_t Tmp, AcX, AcY, AcZ, GyX, GyY, GyZ = 0;
float TmpNormal, AcXNormal, AcYNormal, AcZNormal, GyXNormal, GyYNormal, GyZNormal = 0.0;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ = 0.0;
float roll, pitch, yaw = 0.0;
unsigned long elapsedTime, currentTime, previousTime;

#include "secrets.h"
#include "i2cReceive.h"
#include "oscSend.h"

void setup()
{
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  sendStatusString("wifi started");
  Udp.begin(localPort);
  sendStatusString("udp started");

  Wire.begin(0, 2);
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  // Default Accelerometer Sensitivity Range: +/- 2g
  // Default Gyro Sensitivity Range: +/- 250deg/s
  delay(50);
  /* 
  // Set Accelerometer Sensitivity Range to +/- 8g
  Wire.beginTransmission(MPU);
  Wire.write(0x1C); //Talk to the ACCEL_CONFIG register
  Wire.write(0x10); //Set the register bits as 00010000
  Wire.endTransmission(true);
  delay(20);
  // Set Gyro Sensitivity Range  to +/- 1000deg/s
  Wire.beginTransmission(MPU);
  Wire.write(0x1B); // Talk to the GYRO_CONFIG register
  Wire.write(0x10); // Set the register bits as 00010000
  Wire.endTransmission(true);
  delay(20);
   */
  //get the IMU error values for your module
  // calculate_IMU_error();
}

void loop()
{

  receiveData();   // get raw data over i²c
  normalizeData(); // apply datasheet params
  processData();   // Apply error correction

  // uncomment to  display raw data recieved via i²c
  sendRawGyro();
  // sendRawAccel();

  // uncomment to display data normalised to deg/s & m/s²
  // sendNormalGyro();
  // sendNormalAccel();

  // uncomment to display calculated roll pitch and yaw
  // sendRPY();

  //  implement a delay to reduce the network load
  // delay(20);
}