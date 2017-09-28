# plain-clock

Depends on the esp8266 Arduino environment

Depends on these libraries:
* [the ESP8266 version of the ST7735 library](https://github.com/ddrown/Adafruit-ST7735-Library)
* [millisecond precision clock library](https://github.com/ddrown/Arduino_Clock)
* [Timezone library ported to ESP8266](https://github.com/ddrown/Timezone)
* [ESP8266 NTP client with millisecond precision](https://github.com/ddrown/Arduino_NTPClient)
* [rate adjustment of the local clock from offset measurements](https://github.com/ddrown/Arduino_ClockPID)

Hardware:
* ESP8266 - processor
* ST7735 based LCD board
* Custom case

LCD/ESP8266 connections (NodeMCU names):
* D5 - LCD SPI CLK
* D7 - LCD SPI MOSI
* D8 - LCD CS
* D2 - LCD RES
* D1 - LCD RS/DC
* 3.3V - LCD VCC
* GND - LCD GND

LCD + processor uses around 100mA@5V on startup, ~25mA@5V while running

NTP client, should be within 10ms of true time (depending on network conditions): https://blog.dan.drown.org/binary-clock-update-2/

Video: https://youtu.be/ySibjZQW1yc
