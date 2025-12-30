/*******************************************************************************
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 * Copyright (c) 2018 Terry Moore, MCCI
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example sends a valid LoRaWAN packet with payload "Hello,
 * world!", using frequency and encryption settings matching those of
 * the The Things Network.
 *
 * This uses OTAA (Over-the-air activation), where where a DevEUI and
 * application key is configured, which are used in an over-the-air
 * activation procedure where a DevAddr and session keys are
 * assigned/generated for use with all further communication.
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in
 * g1, 0.1% in g2), but not the TTN fair usage policy (which is probably
 * violated by this sketch when left running for longer)!

 * To use this sketch, first register your application and device with
 * the things network, to set or generate an AppEUI, DevEUI and AppKey.
 * Multiple devices can use the same AppEUI, but each device has its own
 * DevEUI and AppKey.
 * 
 *
 * Do not forget to define the radio type correctly in
 * arduino-lmic/project_config/lmic_project_config.h or from your BOARDS.txt.
 *
 *******************************************************************************/

// MCCI LoRaWAN LMIC library, Version: 4.0.0
#include <lmic.h>

// aht10 library, Date: 03-01-2020 
// https://github.com/Makerfabs/Project_IoT-Irrigation-System/tree/master/LoraTransmitterADCAHT10
#include "I2C_AHT10.h"

// Lightweight low power library for Arduino, Version: 1.81, Date: 21-01-2020 
#include <LowPower.h>

// standard libraries
#include <Wire.h>
#include <hal/hal.h>
#include <SPI.h>

AHT10 humiditySensor; 

//
// For normal use, we require that you edit the sketch to replace FILLMEIN
// with values assigned by the TTN console. However, for regression tests,
// we want to be able to compile these scripts. The regression tests define
// COMPILE_REGRESSION_TEST, and in that case we define FILLMEIN to a non-
// working but innocuous value.
//
#ifdef COMPILE_REGRESSION_TEST
# define FILLMEIN 0
#else
# warning "You must replace the values marked FILLMEIN with real values from the TTN control panel!"
# define FILLMEIN (#dont edit this, edit the lines that use FILLMEIN)
#endif

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8]={  };
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8]={  };
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
static const u1_t PROGMEM APPKEY[16] = {  };
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}

// payload to send to TTN gateway
static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 15; //1200;

// sensors pin mapping
int sensorPin = A2;         // select the input pin for the potentiometer
int sensorPowerCtrlPin = 5; // select control pin for switching VCC (sensors)

// RFM95 pin mapping
const lmic_pinmap lmic_pins = {
    .nss = 10,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 4,
    .dio = {2, 6, 7},
};

// switch VCC on (sensors on)
void sensorPowerOn(void)
{
  digitalWrite(sensorPowerCtrlPin, HIGH);//Sensor power on 
}

// switch VCC off (sensor off)
void sensorPowerOff(void)
{
  digitalWrite(sensorPowerCtrlPin, LOW);//Sensor power on 
}

void printHex2(unsigned v) {
    v &= 0xff;
    if (v < 16)
        Serial.print('0');
    Serial.print(v, HEX);
}

void onEvent (ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Serial.println(F("EV_SCAN_TIMEOUT"));
            break;
        case EV_BEACON_FOUND:
            Serial.println(F("EV_BEACON_FOUND"));
            break;
        case EV_BEACON_MISSED:
            Serial.println(F("EV_BEACON_MISSED"));
            break;
        case EV_BEACON_TRACKED:
            Serial.println(F("EV_BEACON_TRACKED"));
            break;
        case EV_JOINING:
            Serial.println(F("EV_JOINING"));
            break;
        case EV_JOINED:
            Serial.println(F("EV_JOINED"));
            {
              u4_t netid = 0;
              devaddr_t devaddr = 0;
              u1_t nwkKey[16];
              u1_t artKey[16];
              LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
              Serial.print("netid: ");
              Serial.println(netid, DEC);
              Serial.print("devaddr: ");
              Serial.println(devaddr, HEX);
              Serial.print("AppSKey: ");
              for (size_t i=0; i<sizeof(artKey); ++i) {
                if (i != 0)
                  Serial.print("-");
                printHex2(artKey[i]);
              }
              Serial.println("");
              Serial.print("NwkSKey: ");
              for (size_t i=0; i<sizeof(nwkKey); ++i) {
                      if (i != 0)
                              Serial.print("-");
                      printHex2(nwkKey[i]);
              }
              Serial.println();
            }
            // Disable link check validation (automatically enabled
            // during join, but because slow data rates change max TX
	          // size, we don't use it in this example.
            LMIC_setLinkCheckMode(0);
            break;
        /*
        || This event is defined but not used in the code. No
        || point in wasting codespace on it.
        ||
        || case EV_RFU1:
        ||     Serial.println(F("EV_RFU1"));
        ||     break;
        */
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;
        case EV_TXCOMPLETE:
            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("Received ack"));
            if (LMIC.dataLen) {
              Serial.print(F("Received "));
              Serial.print(LMIC.dataLen);
              Serial.println(F(" bytes of payload"));
            }
            // Schedule next transmission
            //os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);

        
        // Use library from https://github.com/rocketscream/Low-Power
        for (int i=0; i<int(TX_INTERVAL/8); i++) {
           // low power sleep mode
           LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
           }
        do_send(&sendjob);
            break;
        case EV_LOST_TSYNC:
            Serial.println(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            Serial.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            Serial.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            Serial.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            Serial.println(F("EV_LINK_ALIVE"));
            break;
        /*
        || This event is defined but not used in the code. No
        || point in wasting codespace on it.
        ||
        || case EV_SCAN_FOUND:
        ||    Serial.println(F("EV_SCAN_FOUND"));
        ||    break;
        */
        case EV_TXSTART:
            Serial.println(F("EV_TXSTART"));
            break;
        case EV_TXCANCELED:
            Serial.println(F("EV_TXCANCELED"));
            break;
        case EV_RXSTART:
            /* do not print anything -- it wrecks timing */
            break;
        case EV_JOIN_TXCOMPLETE:
            Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
            break;

        default:
            Serial.print(F("Unknown event: "));
            Serial.println((unsigned) ev);
            break;
    }
}

void do_send(osjob_t* j){

float   temperature=0.0;        //temperature
float   humidity=0.0;           //humidity
int     soilmoisturepercent=0;  //spoil moisture humidity
uint8_t payload[8];             //payload for TX
int     AirValue = 828;         //capacitive sensor in the value (maximum value)
int     WaterValue = 496;       //capacitive sensor in water value (minimum value)
int     sensorValue = 0;        //capacitive sensor
int     x = 0;


    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else {


    // read capacitive sensor value
    sensorPowerOn();//
    delay(100);
    sensorValue = analogRead(sensorPin);
    delay(200);
  

  
  // measure voltage by band gap voltage
  unsigned int getVDD = 0;

  
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  while (((getVDD == 0)&&(x<=10)) ||  isnan(getVDD)){
  x++;
  ADMUX = (1<<REFS0) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1);
  delay(50);                        // Wait for Vref to settle
  ADCSRA |= (1<<ADSC);              // Start conversion
  while (bit_is_set(ADCSRA,ADSC));  // wait until done
  getVDD = ADC;                     // Vcc in millivolts
  // mcu dependend calibration
  }
  getVDD = 1122475UL / (unsigned long)getVDD; //1126400 = 1.1*1024*1000
  
     sensorPowerOff();
     delay(100);
     sensorPowerOn();
     delay(300);

    // Get the new temperature and humidity value
       while ((humiditySensor.available() == false) && (x<10))

       {
              x++;
              delay(300);
       }

     temperature = humiditySensor.getTemperature();
     humidity = humiditySensor.getHumidity();

   if (humidity == 0) Serial.println(F("Failed to read from AHT sensor (zero values)!"));

    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from AHT sensor (value NaN)!"));
	      temperature=0.0;       
	      humidity=0.0;         
    }

    soilmoisturepercent = map(sensorValue, AirValue, WaterValue, 0, 100);
    if(soilmoisturepercent >= 100)
    {
     soilmoisturepercent=100;
    }
    else if(soilmoisturepercent <=0)
    {
      soilmoisturepercent=0;
    }
    
    // measurement completed, power down sensors
    sensorPowerOff();

    //Print the results
    Serial.print(F("Temperature: "));
    Serial.print(temperature, 2);
    Serial.print(F(" C\t"));
    Serial.print(F("Humidity: "));
    Serial.print(humidity, 2);
    Serial.println(F("% RH\t"));
    
    Serial.print(F("Voltage: "));
    Serial.print(getVDD);
    Serial.println(F("mV \t"));
  
    Serial.print(F("Moisture ADC : "));
    Serial.print(soilmoisturepercent);
    Serial.println(F("% \t"));

    // prepare payload for TX
    byte csmLow = lowByte(soilmoisturepercent);
    byte csmHigh = highByte(soilmoisturepercent);
    // place the bytes into the payload
    payload[0] = csmLow;
    payload[1] = csmHigh;

    // float -> int
    // note: this uses the sflt16 datum (https://github.com/mcci-catena/arduino-lmic#sflt16)
    // used range for mapping type float to int:  -1...+1, -> value/100
    uint16_t payloadTemp = 0;
    if (temperature != 0) payloadTemp = LMIC_f2sflt16(temperature/100);
        // int -> bytes
    byte tempLow = lowByte(payloadTemp);
    byte tempHigh = highByte(payloadTemp);
    // place the bytes into the payload
    payload[2] = tempLow;
    payload[3] = tempHigh;
       
   // used range for mapping type float to int:  -1...+1, -> value/100
    uint16_t payloadHumid = 0;
    if(humidity !=0) payloadHumid = LMIC_f2sflt16(humidity/100);
    // int -> bytes
    byte humidLow = lowByte(payloadHumid);
    byte humidHigh = highByte(payloadHumid);
    payload[4] = humidLow;
    payload[5] = humidHigh;   
 
    // int -> bytes
    byte battLow = lowByte(getVDD);
    byte battHigh = highByte(getVDD);
    payload[6] = battLow;
    payload[7] = battHigh;

    // Prepare upstream data transmission at the next possible time.
    LMIC_setTxData2(1, payload, sizeof(payload), 0);
    Serial.println(F("Packet queued"));
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

void setup() {
    Serial.begin(9600);
    Serial.println(F("Starting"));

    // set control pin for VCC as Output
    pinMode(sensorPowerCtrlPin, OUTPUT);
    sensorPowerOn();
    
    delay(200);
    
    Wire.begin(); //Join I2C bus
    //Check if the AHT10 will acknowledge
    if (humiditySensor.begin() == false)
    {
      Serial.println(F("AHT10 not detected. Please check wiring. Freezing."));
    //while (1);
    }
  else
    Serial.println(F("AHT10 acknowledged."));
    
    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

    LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);
    // Start job (sending automatically starts OTAA too)
    do_send(&sendjob);
}

void loop() {
    os_runloop_once();
}
