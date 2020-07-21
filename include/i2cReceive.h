
void receiveData()
{
    Wire.beginTransmission(MPU);
    Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 7 * 2, true); // request a total of 7*2 = 14 registers
    //reading two registers in the same variable at onece
    AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}
void normalizeData()
{
    // Following coeficients are selected according to the MPU6050 datasheet.
    // Those are specified for each of selectable gyro/acc scale ranges
    // For a range of +-2g,  divide the raw values by 16384
    // For a 250deg/s range  divide the raw values by 131.0
    TmpNormal = Tmp / 340.00 + 36.53;
    AcXNormal = AcX / accelCoeficient;
    AcYNormal = AcY / accelCoeficient;
    AcZNormal = AcZ / accelCoeficient;
    GyXNormal = GyX / gyroCoeficient;
    GyYNormal = GyY / gyroCoeficient;
    GyZNormal = GyZ / gyroCoeficient;
    // At this point the Gyro and Accel variables contain  the calibrated data in ° / s and m / s²
}
void processData()
{
    /*
  // uncomment to apply error correction
  // Assumed Error values:
  // AccErrorX ~(0.58)
  // AccErrorY ~(-1.58)
  // GyroErrorX ~(-0.56)
  // GyroErrorY ~(2)
  // GyroErrorZ ~ (-0.8)
  // See  calculate_IMU_error() for details */

    /* 
  //  uncomment to apply angle error correction
  // accAngleX = (atan(AcYNormal / sqrt(pow(AcXNormal, 2) + pow(AcZNormal, 2))) * 180 / PI) - 0.58;
  // accAngleY = (atan(-1 * AcXNormal / sqrt(pow(AcYNormal, 2) + pow(AcZNormal, 2))) * 180 / PI) + 1.58;  */

    // proceed with no angle error correction
    accAngleX = (atan(AcYNormal / sqrt(pow(AcXNormal, 2) + pow(AcZNormal, 2))) * 180 / PI);
    accAngleY = (atan(-1 * AcXNormal / sqrt(pow(AcYNormal, 2) + pow(AcZNormal, 2))) * 180 / PI);
    /* 
  // gyro error correction
  // GyXNormal = GyXNormal + 0.56;
  // GyYNormal = GyYNormal - 2;
  // GyZNormal = GyZNormal + 0.79; */
    //
    currentTime = millis();                            // Current time actual time read
    elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
    //  values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
    gyroAngleX = gyroAngleX + GyXNormal * elapsedTime; // deg/s * s = deg
    gyroAngleY = gyroAngleY + GyYNormal * elapsedTime;
    yaw = yaw + GyZNormal * elapsedTime;

    // Apply a complementary filter - combine acceleromter and gyro angle values
    roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
    pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
    previousTime = currentTime; // Previous time is stored before the actual time read
}

void calculate_IMU_error()
{
    int c = 0;
    float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
    delay(500);

    // sendStatusString("starting err calibration");
    delay(300);

    // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
    // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
    // Read accelerometer values 200 times
    while (c < 200)
    {
        Wire.beginTransmission(MPU);
        Wire.write(0x3B);
        Wire.endTransmission(false);
        Wire.requestFrom(MPU, 6, true);
        AcXNormal = (Wire.read() << 8 | Wire.read()) / 16384.0;
        AcYNormal = (Wire.read() << 8 | Wire.read()) / 16384.0;
        AcZNormal = (Wire.read() << 8 | Wire.read()) / 16384.0;
        // Sum all readings
        AccErrorX = AccErrorX + ((atan((AcYNormal) / sqrt(pow((AcXNormal), 2) + pow((AcZNormal), 2))) * 180 / PI));
        AccErrorY = AccErrorY + ((atan(-1 * (AcXNormal) / sqrt(pow((AcYNormal), 2) + pow((AcZNormal), 2))) * 180 / PI));
        c++;
    }
    //Divide the sum by 200 to get the error value
    AccErrorX = AccErrorX / 200;
    AccErrorY = AccErrorY / 200;
    c = 0;
    // Read gyro values 200 times
    while (c < 200)
    {
        Wire.beginTransmission(MPU);
        Wire.write(0x43);
        Wire.endTransmission(false);
        Wire.requestFrom(MPU, 6, true);
        GyX = Wire.read() << 8 | Wire.read();
        GyY = Wire.read() << 8 | Wire.read();
        GyZ = Wire.read() << 8 | Wire.read();
        // Sum all readings
        GyroErrorX = GyroErrorX + (GyX / 131.0);
        GyroErrorY = GyroErrorY + (GyY / 131.0);
        GyroErrorZ = GyroErrorZ + (GyZ / 131.0);
        c++;
    }
    //Divide the sum by 200 to get the error value
    GyroErrorX = GyroErrorX / 200;
    GyroErrorY = GyroErrorY / 200;
    GyroErrorZ = GyroErrorZ / 200;
    //
    OSCMessage msgErrA("/1/a/errorCalibration");
    msgErrA.add(AccErrorX);
    msgErrA.add(AccErrorY);
    Udp.beginPacket(targetIp, targetPort);
    msgErrA.send(Udp);
    Udp.endPacket();
    msgErrA.empty();
    delay(300);
    //
    OSCMessage msgErrG("/1/g/errorCalibration");
    msgErrG.add(GyroErrorX);
    msgErrG.add(GyroErrorY);
    msgErrG.add(GyroErrorZ);
    Udp.beginPacket(targetIp, targetPort);
    msgErrG.send(Udp);
    Udp.endPacket();
    msgErrG.empty();
    delay(300);
}