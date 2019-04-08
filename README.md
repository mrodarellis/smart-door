Abstract

The «Smart Door» is a door that has the right sensors to understand its environment and responds appropriately to it and at the same time to record and store important events for its users. With the information it stores it can be considered as a device of the IoT (Internet of things)
For the «Smart door an implementation using micro controllers» project, the proposed implementation of the smart door includes a smart lock and a simple tracking and recording system.
This paper will present the ability to construct a complex environment using very simple and inexpensive microcontrollers. 
The basic microcontroller selections to be used are ESP32 and ESP8266. The Arduino IDE development environment was used to program these microcontrollers.


Basic Smart Door Systems
1. Smart lock Allows opening and closing the door lock.
For the implementation proposed, the user can have four different ways to use. These are: application, fingerprint, NFC and keypad.
2. Activity monitoring and recording system. Allows recording in a database of events occurring in front of the door.
The paper implements the following subsystems:

1st Lock Unlock subsystem- locks and/or unlocks the door.
The activation modes are: 1.Application 2.fingerprint 3.NFC card and 4. keypad. 

2nd Android Application - locks and/or unlocks the door

3rd Fingerprint subsystem - receives a fingerprint and accordingly locks and/or unlocks the door. The AS608/FPM10A Optical Fingerprint Reader Sensor was used.

4th NFC subsystem - reads a card and locks and/or unlocks the door accordingly. Two types of NFC cards was explored one at 125 kHz and one at 13.56 MHz.

5th Keypad subsystem - accepts a password and locks and/or unlocks the door accordingly.

6th Imaging subsystem can be triggered by proximity. The VGA CIF OV7670 CMOS Camera was used.

7th Database logging subsystem records facts and data for editing, history and statistics  via Wi-Fi to Influxdb data base. Also sends a relevant email notification.

Finally, the conclusions of this paper are presented based on the developed systems.


Keywords
Smart door, ESP32, ESP-Now, Pjon, ESP8266, Influxdb, microcontrollers
