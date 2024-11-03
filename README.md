# hoverserial
An Arduino library for communicating with flashed Hoverboard controllers: specifically the [EFeru](https://github.com/EFeru/hoverboard-firmware-hack-FOC) FOC fork.

This mostly exists to enable code re-use, especially when driving multiple hoverboards from one microcontroller, as it is very similar to the code in the hoverserial.ino example in the original repository but refactored and with some boilerplate stuck round it.

Controlling a hoverboard can be as simple as...

```c++
#include <hoverserial.h>
#define HOVERBOARD_RX_PIN 18
#define HOVERBOARD_TX_PIN 17
#define HOVERBOARD_BAUD_RATE 115200
#define SPEED 100
#define STEER 0

hoverserial hoverboard;

void setup() {
	Serial1.begin(HOVERBOARD_BAUD_RATE, SERIAL_8N1, HOVERBOARD_RX_PIN, HOVERBOARD_TX_PIN);
	hoverboard.begin(Serial1);
}

void loop() {
    hoverboard.housekeeping();
    if(hoverboard.isConnected())
    {
        hoverboard.setSpeed(SPEED);
        hoverboard.setSteer(STEER);
    }
}
```

...the library only sends serial commands every 250ms unless you're updating the speed/steer value.

As the firmware can be flashed for either speed & steer or tank style two speeds there are two ways to send the value, but this is mostly cosmetic as the data structures sent to the controller are the same.

You can also monitor the reported motor speeds, battery voltage and controller temperature.

    Serial.print(F("Left speed: "));
    Serial.print(hoverboard.getSpeedLeft());
    Serial.print(F(" Right speed: "));
    Serial.print(hoverboard.getSpeedRight());
    Serial.print(F(" Battery voltage: "));
    Serial.print(float(hoverboard.getBatteryVoltage())/100.0);  //Battery voltage is x100
    Serial.print(F(" Controller temp: "));
    Serial.println(float(hoverboard.getBoardTemperature())/10.0);  //Temperature in C is x10

That's about it but see [CHANGELOG.md](CHANGELOG.md) for the latest info.
