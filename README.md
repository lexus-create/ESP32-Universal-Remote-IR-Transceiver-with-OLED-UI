# ESP32-Universal-Remote-IR-Transceiver-with-OLED-UI
An IR remote tool built on an ESP32. It can learn IR signals from any remote, save them to an SD card, and fire them back later. Built as a learning project to get more comfortable with ESP32, SPI/SD storage, and non-blocking code.
The device boots into a simple 3-item menu on the OLED, navigated with two buttons:

Receive Mode — listens for 5 seconds, captures any IR signal it sees, and appends it to a file on the SD card.
Transmit Mode — loads everything saved so far, lets you scroll through the list on the OLED, and fires whichever one you land on.
Repeat Mode — resends the last signal you transmitted, 10 times in a row. Useful if the first shot didn't land.



Hardware:
ESP32 DevKit
SSD1306 OLED (128x64, I2C)
SD card module (SPI)
IR receiver module
IR transmitter module
2x push buttons

Component     	Pin
OLED SDA	      GPIO23
OLED SCL	      GPIO22
SD CS	          GPIO16
SD SCK	        GPIO4
SD MISO	        GPIO15
SD MOSI        	GPIO2
IR Transmit (DAT)	GPIO21
IR Receive (OUT)	GPIO5
Button 1	      GPIO19
Button 2	      GPIO18

Discontinued for now as my hardware is faulty.

