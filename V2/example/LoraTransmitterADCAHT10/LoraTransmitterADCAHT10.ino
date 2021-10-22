// LoRa 9x_TX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example LoRa9x_RX

//Function:Transmit Soil Moisture to Lora
//Author: Charlin
//Date:2020/06/12
//hardware: Lora Soil Moisture Sensor v1.1


#include <SPI.h>
#include "RH_RF95.h"


#include "I2C_AHT10.h"
#include <Wire.h>
AHT10 humiditySensor;


int sensorPin = A2;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor
int sensorPowerCtrlPin = 5;

void sensorPowerOn(void)
{
  digitalWrite(sensorPowerCtrlPin, HIGH);//Sensor power on 
}
void sensorPowerOff(void)
{
  digitalWrite(sensorPowerCtrlPin, LOW);//Sensor power on 
}

#define RFM95_CS 10
#define RFM95_RST 4
#define RFM95_INT 2

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 433.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() 
{

  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, LOW);
  delay(100);
  digitalWrite(RFM95_RST, HIGH);

  pinMode(sensorPowerCtrlPin, OUTPUT);
  //digitalWrite(sensorPowerCtrlPin, LOW);//Sensor power on 
  sensorPowerOn();
  //pinMode(sensorPin, INPUT);
  
  //while (!Serial);
  Serial.begin(115200);
  delay(100);

  Wire.begin(); //Join I2C bus
  //Check if the AHT10 will acknowledge
  if (humiditySensor.begin() == false)
  {
    Serial.println("AHT10 not detected. Please check wiring. Freezing.");
    //while (1);
  }
  else
    Serial.println("AHT10 acknowledged.");
    
  Serial.println("Marduino LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while(!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  //rf95.setModemConfig(Bw125Cr48Sf4096);

  //Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); 
  Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);

  //dht.begin();
}

int16_t packetnum = 0;  // packet counter, we increment per xmission
float temperature=0.0;//
float humidity=0.0;

void loop()
{

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  //float humidity = 6.18;//dht.readHumidity();
  // Read temperature as Celsius (the default)

  sensorPowerOn();//
  delay(100);
  sensorValue = analogRead(sensorPin);
  delay(200);

  if (humiditySensor.available() == true)
  {
    //Get the new temperature and humidity value
    temperature = humiditySensor.getTemperature();
    humidity = humiditySensor.getHumidity();

    //Print the results
    Serial.print("Temperature: ");
    Serial.print(temperature, 2);
    Serial.print(" C\t");
    Serial.print("Humidity: ");
    Serial.print(humidity, 2);
    Serial.println("% RH");

  }
    // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from AHT sensor!"));
    //return;
  }
  
  delay(100);
  //sensorPowerOff();

  Serial.print(F("Moisture ADC : "));
  Serial.println(sensorValue);

  
  //Serial.print(F("Humidity: "));
  //Serial.print(humidity);
  //Serial.print(F("%  Temperature: "));
  //Serial.print(temperature);
  //Serial.println("Humidity is " + (String)humidity);
  //Serial.println("Temperature is " + (String)temperature);

  String message = "#"+(String)packetnum+" Humidity:"+(String)humidity+"% Temperature:"+(String)temperature+"C"+" ADC:"+(String)sensorValue;
  Serial.println(message);
  packetnum++;
  Serial.println("Transmit: Sending to rf95_server");
  
  // Send a message to rf95_server

  uint8_t radioPacket[message.length()+1];
  message.toCharArray(radioPacket, message.length()+1);
  
  radioPacket[message.length()+1]= '\0';

  Serial.println("Sending..."); delay(10);
  rf95.send((uint8_t *)radioPacket, message.length()+1); 
  Serial.println("Waiting for packet to complete..."); delay(10);
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  Serial.println("Waiting for reply..."); delay(10);
  if(rf95.waitAvailableTimeout(8000))
  {
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
      Serial.print("Got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);    
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
  else
  {
    Serial.println("No reply, is there a listener around?");
  }
  delay(1000);
}
