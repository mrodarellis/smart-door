# smart-door
The «Smart Door» is a door that has the right sensors to understand its environment and responds appropriately to it and at the same time to record and store important events for its users. With the information it stores it can be considered as a device of the IoT (Internet of things)
For the «Smart Door An Implementation using Micro Controllers» project, the intelligent door construction can include a smart lock and a simple tracking and recording system.
The work will present the ability to construct a complex environment using very simple and inexpensive microcontrollers. The basic microcontroller  selections to be used are ESP32 and ESP8266.
Basic Smart Door Systems
1. Smart lock Allows opening and closing the door lock.
For the implementation proposed, the user can have four different ways to use these are : fingerprint, NFC, Keypad and application.
2. Activity monitoring and recording system Allows recording of video in a database of events occurring in front of the door.
Smart locking subsystems
1st lock unlock depending on how the door locks and/or unlocks.
The activation modes are: 1.Fingerprint 2. nfc card, 3. keypad and 4. Application
2nd Android Application locks and/or unlocks the door
3rd Fingerprint reader receives a fingerprint and accordingly locks and/or unlocks the door. The AS608/FPM10A Optical Fingerprint Reader Sensor was used.
4th NFC reader reads a card and locks and/or unlocks the door accordingly. Two types of nfc cards was explored one at 125 kHz and one at 13.56 MHz.
5th Keypad reader accepts a password and locks and/or unlocks the door accordingly
6th Imaging system that will be triggered by proximity, motion and by the 2,3,4 and 5 systems. The VGA CIF OV7670 CMOS Camera was used.

7th subsystem Database logging system. records facts, data  and statistics also sends a relevant email notification.

Keywords
Smart door, ESP32, ESP-Now, Pjon, ESP8266, Influxdb, microcontrollers
