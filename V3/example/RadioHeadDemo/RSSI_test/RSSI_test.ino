//Example of sending control code to AC Dimmer
//Author :Vincent
//Create :2021/7/31
//Use Maduino Lora Radio
//Complier esp32

#include <SPI.h>
#include <RH_RF95.h>

#define NODE_NAME "SOIL915"

#define REC_MODE 0

#define ESP32_LORA
//#define AVR_LORA

#ifdef ESP32_LORA

//Makepython Lora
// #define DIO0 35
// #define DIO1 39

// #define RFM95_RST 2
// #define RFM95_CS 25

// #define SPI_MOSI 13
// #define SPI_MISO 12
// #define SPI_SCK 14

//Lora Expansion
#define DIO0 32
#define DIO1 35

#define RFM95_RST 22
#define RFM95_CS 21

#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCK 18

#endif

#ifdef AVR_LORA

#define RFM95_CS 10
#define RFM95_RST 9
#define DIO0 2

#endif

// Change to 434.0 or other frequency, must match RX's freq!
// #define RF95_FREQ 433.0
// #define RF95_FREQ 868.0
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, DIO0);

void setup()
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  //while (!Serial);
  Serial.begin(115200);
  delay(100);

  Serial.println("Arduino LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

#ifdef ESP32_LORA
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
#endif

  while (!rf95.init())
  {
    Serial.println("LoRa radio init failed");
    while (1)
      ;
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ))
  {
    Serial.println("setFrequency failed");
    while (1)
      ;
  }
  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);

  // //Long range, low bandwidth, reduced communication tolerance.
  rf95.setModemConfig(rf95.Bw31_25Cr48Sf512);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(17, false);
}

int16_t packetnum = 0; // packet counter, we increment per xmission
long int runtime = 0;

void loop()
{

  if (REC_MODE)
  {
    if (rf95.available())
    {
      // Should be a message for us now
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);

      if (rf95.recv(buf, &len))
      {
        packetnum++;
        RH_RF95::printBuffer("Received: ", buf, len);
        Serial.print("Got: ");
        Serial.println((char *)buf);
        Serial.print("RSSI: ");
        Serial.println(rf95.lastRssi(), DEC);
      }
      else
      {
        Serial.println("Receive failed");
      }
    }
  }
  else
  {
    if ((millis() - runtime) > 3000)
    {
      char radiopacket[40] = "";
      sprintf(radiopacket, "%s:%d", NODE_NAME, packetnum++);

      Serial.println("Sending...");
      delay(10);
      rf95.send((uint8_t *)radiopacket, 40);

      Serial.println("Waiting for packet to complete...");
      delay(10);
      rf95.waitPacketSent();

      delay(500);
      runtime = millis();
    }
  }
}
