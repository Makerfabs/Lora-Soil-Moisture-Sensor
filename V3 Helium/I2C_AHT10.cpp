/****************************************************************

 ***************************************************************/

#include "I2C_AHT10.h"

/*--------------------------- Device Status ------------------------------*/
bool AHT10::begin(TwoWire &wirePort)
{
    _i2cPort = &wirePort; //Grab the port the user wants to communicate on

    _deviceAddress = AHT10_DEFAULT_ADDRESS; //We had hoped the AHT10 would support two addresses but it doesn't seem to

    if (isConnected() == false)
        return false;

    //Wait 40 ms after power-on before reading temp or humidity. Datasheet pg 8
    delay(40);

    //Check if the calibrated bit is set. If not, init the sensor.
    if (isCalibrated() == false)
    {
        //Send 0xBE0800
        initialize();

        //Immediately trigger a measurement. Send 0xAC3300
        triggerMeasurement();

        delay(75); //Wait for measurement to complete

        uint8_t counter = 0;
        while (isBusy())
        {
            delay(1);
            if (counter++ > 100)
                return (false); //Give up after 100ms
        }

        //This calibration sequence is not completely proven. It's not clear how and when the cal bit clears
        //This seems to work but it's not easily testable
        if (isCalibrated() == false)
        {
            return (false);
        }
    }

    //Check that the cal bit has been set
    if (isCalibrated() == false)
        return false;

    //Mark all datums as fresh (not read before)
    sensorQueried.temperature = true;
    sensorQueried.humidity = true;

    return true;
}

//Ping the AHT10's I2C address
//If we get a response, we are correctly communicating with the AHT10
bool AHT10::isConnected()
{
    _i2cPort->beginTransmission(_deviceAddress);
    if (_i2cPort->endTransmission() == 0)
        return true;

    //If IC failed to respond, give it 20ms more for Power On Startup
    //Datasheet pg 7
    delay(20);

    _i2cPort->beginTransmission(_deviceAddress);
    if (_i2cPort->endTransmission() == 0)
        return true;

    return false;
}

/*------------------------ Measurement Helpers ---------------------------*/

uint8_t AHT10::getStatus()
{
    _i2cPort->requestFrom(_deviceAddress, (uint8_t)1);
    if (_i2cPort->available())
        return (_i2cPort->read());
    return (0);
}

//Returns the state of the cal bit in the status byte
bool AHT10::isCalibrated()
{
    return (getStatus() & (1 << 3));
}

//Returns the state of the busy bit in the status byte
bool AHT10::isBusy()
{
    return (getStatus() & (1 << 7));
}

bool AHT10::initialize()
{
    _i2cPort->beginTransmission(_deviceAddress);
    _i2cPort->write(sfe_aht10_reg_initialize);
    _i2cPort->write(0x80);
    _i2cPort->write(0x00);
    if (_i2cPort->endTransmission() == 0)
        return true;
    return false;
}

bool AHT10::triggerMeasurement()
{
    _i2cPort->beginTransmission(_deviceAddress);
    _i2cPort->write(sfe_aht10_reg_measure);
    _i2cPort->write(0x33);
    _i2cPort->write(0x00);
    if (_i2cPort->endTransmission() == 0)
        return true;
    return false;
}

//Loads the
void AHT10::readData()
{
    //Clear previous data
    sensorData.temperature = 0;
    sensorData.humidity = 0;

    if (_i2cPort->requestFrom(_deviceAddress, (uint8_t)6) > 0)
    {
        uint8_t state = _i2cPort->read();

        uint32_t incoming = 0;
        incoming |= (uint32_t)_i2cPort->read() << (8 * 2);
        incoming |= (uint32_t)_i2cPort->read() << (8 * 1);
        uint8_t midByte = _i2cPort->read();

        incoming |= midByte;
        sensorData.humidity = incoming >> 4;

        sensorData.temperature = (uint32_t)midByte << (8 * 2);
        sensorData.temperature |= (uint32_t)_i2cPort->read() << (8 * 1);
        sensorData.temperature |= (uint32_t)_i2cPort->read() << (8 * 0);

        //Need to get rid of data in bits > 20
        sensorData.temperature = sensorData.temperature & ~(0xFFF00000);

        //Mark data as fresh
        sensorQueried.temperature = false;
        sensorQueried.humidity = false;
    }
}

//Triggers a measurement if one has not been previously started, then returns false
//If measurement has been started, checks to see if complete.
//If not complete, returns false
//If complete, readData(), mark measurement as not started, return true
bool AHT10::available()
{
    if (measurementStarted == false)
    {
        triggerMeasurement();
        measurementStarted = true;
        return (false);
    }

    if (isBusy() == true)
    {
        return (false);
    }

    readData();
    measurementStarted = false;
    return (true);
}

bool AHT10::softReset()
{
    _i2cPort->beginTransmission(_deviceAddress);
    _i2cPort->write(sfe_aht10_reg_reset);
    if (_i2cPort->endTransmission() == 0)
        return true;
    return false;
}

/*------------------------- Make Measurements ----------------------------*/

float AHT10::getTemperature()
{
    if (sensorQueried.temperature == true)
    {
        //We've got old data so trigger new measurement
        triggerMeasurement();

        delay(75); //Wait for measurement to complete

        uint8_t counter = 0;
        while (isBusy())
        {
            delay(1);
            if (counter++ > 100)
                return (false); //Give up after 100ms
        }

        readData();
    }

    //From datasheet pg 8
    float tempCelsius = ((float)sensorData.temperature / 1048576) * 200 - 50;

    //Mark data as old
    sensorQueried.temperature = true;

    return tempCelsius;
}

float AHT10::getHumidity()
{
    if (sensorQueried.humidity == true)
    {
        //We've got old data so trigger new measurement
        triggerMeasurement();

        delay(75); //Wait for measurement to complete

        uint8_t counter = 0;
        while (isBusy())
        {
            delay(1);
            if (counter++ > 100)
                return (false); //Give up after 100ms
        }

        readData();
    }

    //From datasheet pg 8
    float relHumidity = ((float)sensorData.humidity / 1048576) * 100;

    //Mark data as old
    sensorQueried.humidity = true;

    return relHumidity;
}
