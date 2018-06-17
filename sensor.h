#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Kalman.h> // Source: https://github.com/TKJElectronics/KalmanFilter

class Sensor {
  public:
    Sensor();
    ~Sensor();
    void update();
    void init();
    double getDegreeX();
    double getDegreeY();
    void calibrate();

  private:
    const uint8_t IMUAddress = 0x68; // AD0 is logic low on the PCB
    const uint16_t I2C_TIMEOUT = 1000; // Used to check for errors in I2C communication
    Kalman kalmanX; // Create the Kalman instances
    Kalman kalmanY;
    uint32_t timer;
    uint8_t i2cData[14];
    /* IMU Data */
    double accX, accY, accZ;
    double gyroX, gyroY, gyroZ;
    int16_t tempRaw;
    double gyroXangle, gyroYangle; // Angle calculate using the gyro only
    double compAngleX, compAngleY; // Calculated angle using a complementary filter
    double kalAngleX, kalAngleY; // Calculated angle using a Kalman filter
    double offsetX, offsetY;

    uint8_t i2cWrite(uint8_t registerAddress, uint8_t data, bool sendStop);
    uint8_t i2cWrite(uint8_t registerAddress, uint8_t *data, uint8_t length, bool sendStop);
    uint8_t i2cRead(uint8_t registerAddress, uint8_t *data, uint8_t nbytes);

};

#endif
