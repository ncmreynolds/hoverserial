/*
 * Example sketch for UART control of a hoverboard re-flashed in speed & steer mode
 * 
 * This assumes multiple hardware Serial interfaces are available, for example on an ESP32 or Arduino Mega
 * 
 */
#include <hoverserial.h>
#define HOVERBOARD_RX_PIN 18
#define HOVERBOARD_TX_PIN 17
#define HOVERBOARD_BAUD_RATE 115200

hoverserial hoverboard;

int16_t speed = 0;
int8_t direction = 50;
uint32_t lastChange = 0;
uint32_t lastStatusUpdate = 0;

void setup() {
  //Set up the serial ports
  Serial.begin(115200);
  Serial1.begin(HOVERBOARD_BAUD_RATE, SERIAL_8N1, HOVERBOARD_RX_PIN, HOVERBOARD_TX_PIN);
  //Set up hoverserial debug output
  //hoverboard.debug(Serial);
  //Set up hoverserial connection to the hoverboard
  hoverboard.begin(Serial1);
}

void loop() {
  //Maintain the hoverboard connection. This must run frequently else the hoverboard will stop and beep plaintively. This is a kind of "deadman's switch" built into the hoverboard firmware.
  hoverboard.housekeeping();
  //if(hoverboard.isConnected())
  {
    if(millis() - lastChange > 3000)  //Don't use delay(), use this kind of non-blocking loop when controlling the hoverboard
    {
      lastChange = millis();
      speed += direction;
      if(speed >= 1000)
      {
        speed = 1000;
        direction = -direction;
      }
      else if(speed <= -1000)
      {
        speed = -1000;
        direction = -direction;
      }
      hoverboard.setSpeed(speed);
    }
    if(millis() - lastStatusUpdate > 5000)
    {
      lastStatusUpdate = millis();
      Serial.print(F("Left speed: "));
      Serial.print(hoverboard.getSpeedLeft());
      Serial.print(F(" Right speed: "));
      Serial.print(hoverboard.getSpeedRight());
      Serial.print(F(" Battery voltage: "));
      Serial.print(float(hoverboard.getBatteryVoltage())/100.0);  //Battery voltage is x100
      Serial.print(F(" Controller temp: "));
      Serial.println(float(hoverboard.getBoardTemperature())/10.0);  //Temperature in C is x10
    }
  }
}
