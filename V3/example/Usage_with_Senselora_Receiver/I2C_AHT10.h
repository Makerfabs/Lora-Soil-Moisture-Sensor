/****************************************************************
 * 
 ******************************************************************/

#ifndef __I2C_AHT10_H__
#define __I2C_AHT10_H__

#include <Arduino.h>
#include <Wire.h>

#define AHT10_DEFAULT_ADDRESS 0x38

enum registers
{
    sfe_aht10_reg_reset = 0xBA,
    sfe_aht10_reg_initialize = 0xBE,
    sfe_aht10_reg_measure = 0xAC,
};

class AHT10
{
private:
    TwoWire *_i2cPort; //The generic connection to user's chosen I2C hardware
    uint8_t _deviceAddress;
    bool measurementStarted = false;

    struct
    {
        uint32_t humidity;
        uint32_t temperature;
    } sensorData;

    struct
    {
        uint8_t temperature : 1;
        uint8_t humidity : 1;
    } sensorQueried;

public:
    //Device status
    bool begin(TwoWire &wirePort = Wire); //Sets the address of the device and opens the I2C bus
    bool isConnected();                   //Checks if the AHT10 is connected to the I2C bus
    bool available();                     //Returns true if new data is available

    //Measurement helper functions
    uint8_t getStatus();       //Returns the status byte
    bool isCalibrated();       //Returns true if the cal bit is set, false otherwise
    bool isBusy();             //Returns true if the busy bit is set, false otherwise
    bool initialize();         //Initialize for taking measurement
    bool triggerMeasurement(); //Trigger the AHT10 to take a measurement
    void readData();           //Read and parse the 6 bytes of data into raw humidity and temp
    bool softReset();          //Restart the sensor system without turning power off and on

    //Make measurements
    float getTemperature(); //Goes through the measurement sequence and returns temperature in degrees celcius
    float getHumidity();    //Goes through the measurement sequence and returns humidity in % RH
};
#endif
