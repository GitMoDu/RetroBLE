# ![Connecting past devices to the present](https://github.com/GitMoDu/RetroBLE/blob/master/media/banner.jpg)

Arduino framework for combined BLE/USB HID devices using nRF52850.

Includes battery management and reporting.

## Hardware Support
The framework is built around the [Bluefruit library](https://github.com/adafruit/Adafruit_nRF52_Arduino/blob/master/libraries/Bluefruit52Lib/src/bluefruit.h) for nRF52840. Supported devices include:
  * [Seeed Studios Xiao nRF52840 (and Sense)](https://wiki.seeedstudio.com/XIAO_BLE/)
    - Includes [BMS](https://wiki.seeedstudio.com/XIAO_BLE/#battery-charging-current)
  * [Generic nRF82840](https://github.com/adafruit/Adafruit_nRF52_Arduino) 
    - No native BMS.

## Dependencies
### Framework
- [Adafruit TinyUSB](https://github.com/adafruit/Adafruit_TinyUSB_Arduino)
- [Seeed Studio nRF52 Core](https://github.com/Seeed-Studio/Adafruit_nRF52_Arduino)
- [Adafruit nRF52 Core](https://github.com/adafruit/Adafruit_nRF52_Arduino)
- [Task Scheduler](https://github.com/arkhipenko/TaskScheduler)

### Controller
[VirtualPad](https://github.com/GitMoDu/VirtualPad)
- Embedded virtual controller, standard mapping for generic controllers.


# Published Retro BLE devices
[Atari Joystick (1977)](https://github.com/GitMoDu/RetroBLE/tree/master/examples/AtariJoystick)

![](https://raw.githubusercontent.com/GitMoDu/RetroBLE/master/media/atari_joystick.jpg)


[Mega Drive Control Pad (1988)](https://github.com/GitMoDu/RetroBLE/tree/master/examples/MegaDrive3Button)

![](https://raw.githubusercontent.com/GitMoDu/RetroBLE/master/media/mega_drive_control_pad.jpg)

[Model M Keyboard (1985)](https://github.com/GitMoDu/ModernModelM)

![](https://github.com/GitMoDu/ModernModelM/blob/master/media/modelm.jpg)


