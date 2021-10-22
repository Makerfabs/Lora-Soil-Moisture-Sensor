# Lora Soil Moisture Sensor

[toc]

# Makerfabs

[Makerfabs home page](https://www.makerfabs.com/)

[Makerfabs Wiki](https://makerfabs.com/wiki/index.php?title=Main_Page)


# Lora Soil Moisture Sensor

## Introduce

Product Link: [Lora_Soil_Moisture_Sensor](https://www.makerfabs.com/lora-soil-moisture-sensor.html)

Wiki Link:  [Lora_Soil_Moisture_Sensor Wiki](https://www.makerfabs.com/wiki/index.php?title=Lora_Soil_Moisture_Sensor)

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


## V2
Old version, containing 555 chip. A number of items are provided as references.

**More details, see readme. md in the [V2](https://github.com/Makerfabs/Lora-Soil-Moisture-Sensor/tree/master/V2) folder.**

