# Lora Soil Moisture Sensor

[toc]

# Makerfabs

[Makerfabs home page](https://www.makerfabs.com/)

[Makerfabs Wiki](https://makerfabs.com/wiki/index.php?title=Main_Page)


# Lora Soil Moisture Sensor

## Introduce

Product Link: [Lora_Soil_Moisture_Sensor](https://www.makerfabs.com/lora-soil-moisture-sensor.html)

Wiki Link:  [Lora_Soil_Moisture_Sensor_V3](https://www.makerfabs.com/wiki/index.php?title=Lora_Soil_Moisture_Sensor_V3)

The Lora soil moisture sensor is based on Atmel's Atmega328P, it collects local air temperature/ humidity with sensor AHT10, and detect the soil humidity with capacitor-humility measurement solution with MCU clock, and transmit the local environment data to the gateway, with Lora communication, suit for applications for smart-farm, irrigation, agriculture, etc. 

In applications, always you do not need to check the air/soil state continuously, have a test of them for few seconds after then minutes/hours sleeping is normally Ok for most projects. To save power, there the Air/ Soil measuring functional could be shut down in the working, so they can be only powered ON a short time and then a long time power off. With MCU in sleeping mode and low power consumption Lora module, this module works ok with 2 AAA battery more than one year. Besides, this sensor is coated with waterproof paint, which makes it longer working time in damp soil. 



## Feature

- Classic ATMEL AVR 8-bit Atmega328P, with Arduino Pro Mini(3.3V/8M bootloader pre-loaded) .
- Capacitor-humility measurement.
- AHT10 temperature and humidity sensor.
- Working with 2 AAA more than one years.
- 3D printed case.
- Waterproof coating.
- Unique ID, can be used directly without secondary programming.

![front](md_pic/front.jpg)

# Version
## V3 
In the latest version, the 555 chip has been removed so that the sleep current can be as low as 7.1uA. And added UID, can be used directly.

**More details, see readme. md in the [V3](https://github.com/Makerfabs/Lora-Soil-Moisture-Sensor/tree/master/V3) folder.**
# Makerfabs Soil Moisture Sensor V3 For the Helium Network
Adapted from <https://hackaday.io/project/180796-lorawan-soil-moisture-sensor>

## Equipment needed:

|Sensor|<https://www.makerfabs.com/lora-soil-moisture-sensor-v3.html>||
| :- | :- | :- |
|3.3V USB to TTL / Serial converter|<https://www.makerfabs.com/cp2104-usb-to-serial-converter.html>|Makerfabs recommended|
||<https://ftdichip.com/products/ttl-232r-3v3/>|Arduino recommended|
||<https://www.sparkfun.com/products/9873>|Arduino recommended|
||[https://www.amazon.com/Serial-Adapter-Signal-Prolific-Windows/dp/B07R8BQYW1/](https://www.amazon.com/Serial-Adapter-Signal-Prolific-Windows/dp/B07R8BQYW1/ref=rvi_sccl_3/130-8941171-3218759?pd_rd_w=pLTyv&content-id=amzn1.sym.f5690a4d-f2bb-45d9-9d1b-736fee412437&pf_rd_p=f5690a4d-f2bb-45d9-9d1b-736fee412437&pf_rd_r=2VXNT4N0YP2KE4HXH7XZ&pd_rd_wg=rzSKu&pd_rd_r=5e7c75ef-e88f-4056-a7fc-fb7d07ab074c&pd_rd_i=B07R8BQYW1&psc=1)|Not ideal, but what I had lying around|
||[https://www.amazon.com/dp/B07X4ZTRD2](https://www.amazon.com/dp/B07X4ZTRD2?psc=1&ref=ppx_yo2ov_dt_b_product_details)|did NOT work|
|Dupont wire and header pins||optional depending on converter|

## Connecting and uploading to the board:

1. Use EITHER the power output of the USB-TTL converter OR batteries to power the sensor, not both
2. If the USB-TTL converter does not have DTR, you must use the RST button on the sensor to upload a sketch: hold down RST, press upload, wait until the status shows "uploading", immediately release RST
3. If connection does not work, try reversing RX and TX
4. What worked for me: sensor powered by batteries; converter GND, RX, TX only connected, RX-TX reversed

## Software needed:
* MCCI LoRaWAN LMIC library <https://github.com/mcci-catena/arduino-lmic>
* Lightweight low power library for Arduino <https://github.com/rocketscream/Low-Power>
* For original source code and additional information, refer to: <https://hackaday.io/project/180796-lorawan-soil-moisture-sensor>

## Helium setup:
1. Pay careful attention to the byte order and order of the variables APPEUI, DEVEUI, APPKEY. These are different than what is displayed in the helium console by default
2. The decoder here is provided for reference, it has not been modified to work on the helium console

## Reference info:

* ATMEL AVR 8-bit Atmega328P, with Arduino Pro Mini(3.3V/8M bootloader pre-loaded)
* AHT10 temperature and humidity sensor
* <https://docs.arduino.cc/retired/getting-started-guides/ArduinoProMini>




## V2
Old version, containing 555 chip. A number of items are provided as references.

**More details, see readme. md in the [V2](https://github.com/Makerfabs/Lora-Soil-Moisture-Sensor/tree/master/V2) folder.**

