#include <avr/wdt.h>
#include <avr/sleep.h>
#include <RadioLib.h>
#include "I2C_AHT10.h"
#include <Wire.h>

#define NODENAME "Soil2"

#define DIO0 2
#define DIO1 6
#define DIO2 7
#define DIO5 8

#define LORA_RST 4
#define LORA_CS 10

#define SPI_MOSI 11
#define SPI_MISO 12
#define SPI_SCK 13

int ledPin = 13;
int shu = 0;
int sensorPin = A2; // select the input pin for the potentiometer
int sensorPowerCtrlPin = 5;

int sensorValue = 0;     // variable to store the value coming from the sensor
int16_t packetnum = 0;   // packet counter, we increment per xmission
float temperature = 0.0; //
float humidity = 0.0;
String errcode = "";

SX1278 radio = new Module(LORA_CS, DIO0, LORA_RST, DIO1, SPI, SPISettings());
AHT10 humiditySensor;

ISR(WDT_vect)
{
    Serial.print("[Watch dog]");
    Serial.println(shu);
    delay(100);
    shu++;
    wdt_reset();
}

void setup()
{
    wdt_disable();
    pinMode(ledPin, OUTPUT);
    Serial.begin(115200);
    Serial.println("[Start]");
    delay(100);
    SPI.begin();

    //setup start
    Serial.println("[Setup]");
    delay(100);

    // initialize SX1278 with default settings
    Serial.println(String("[SX1278]Sensor name is :") + String(NODENAME));
    Serial.print(F("[SX1278] Initializing ... "));
    int state = radio.begin();
    if (state == ERR_NONE)
    {
        Serial.println(F("success!"));
    }
    else
    {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true)
            ;
    }
    radio.sleep();

    //AHT10
    pinMode(sensorPowerCtrlPin, OUTPUT);
    digitalWrite(sensorPowerCtrlPin, HIGH); //Sensor power on

    Wire.begin();
    if (humiditySensor.begin() == false)
    {
        Serial.println("AHT10 not detected. Please check wiring. Freezing.");
    }
    else
        Serial.println("AHT10 acknowledged.");

    read_sensor();
    //setup over

    low_power_set();
}

void loop()
{
    wdt_disable();

    if (shu > 7) //(7+1) x 8S
    {
        //code start
        Serial.println("Code start*************************************");

        read_sensor();
        //code end
        Serial.println("Code end*************************************");

        //count init
        shu = 0;
    }

    watchdog_init();
    delay(10);
    sleep_cpu();
}

//Set low power mode and into sleep
void low_power_set()
{
    Serial.println("[Set]Sleep Mode Set");
    delay(100);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    watchdog_init();
    delay(10);
    sleep_cpu();
}

//Enable watch dog
void watchdog_init()
{
    MCUSR &= ~(1 << WDRF);
    WDTCSR |= (1 << WDCE) | (1 << WDE);

    //WDTCSR = 1 << WDP1 | 1 << WDP2; //1S
    WDTCSR = 1 << WDP0 | 1 << WDP3; //8S

    WDTCSR |= _BV(WDIE); //not rst, inter interrutp
    wdt_reset();
}

void read_sensor()
{
    digitalWrite(sensorPowerCtrlPin, HIGH); //Sensor power on
    for (int i = 0; i < 3; i++)
    {
        sensorValue = analogRead(sensorPin);
        delay(200);
        if (humiditySensor.available() == true)
        {
            temperature = humiditySensor.getTemperature();
            humidity = humiditySensor.getHumidity();
        }
        if (isnan(humidity) || isnan(temperature))
        {
            Serial.println(F("Failed to read from AHT sensor!"));
        }
    }
    digitalWrite(sensorPowerCtrlPin, LOW); //Sensor power on

    String lora_msg = "#" + (String)packetnum +" NAME:" + (String)NODENAME + " H:" + (String)humidity + "% T:" + (String)temperature + " C" + " ADC:" + (String)sensorValue;
    Serial.println(lora_msg);
    packetnum++;
    radio.transmit(lora_msg);
    delay(1000);
    radio.sleep();
}