/****************************************************************************************************************************\
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * You received a copy of the GNU General Public License along with this program in file 'License.txt'.
 *
 * IDE download    : https://www.arduino.cc/en/Main/Software
 * ESP8266 Package : https://github.com/esp8266/Arduino
 *
* Additional information about licensing can be found at : http://www.gnu.org/licenses
\*************************************************************************************************************************/

// This file is to be loaded onto an Arduino Pro Mini so it will act as a simple IO extender to the ESP module.
// Communication between ESP and Arduino is using the I2C bus, so only two wires needed.
// It best to run the Pro Mini on 3V3, although the 16MHz versions do not officially support this voltage level on this frequency.
// That way, you can skip levelconverters on I2C.
// Arduino Mini Pro uses A4 and A5 for I2C bus. ESP I2C can be configured but they are on GPIO-4 and GPIO-5 by default.

#include <Wire.h>
volatile int valueRead=0;

void setup()
{
  Wire.begin(0x7f);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop() {}

void receiveEvent(int count)
{
  if (count == 4)
  {
    byte cmd = Wire.read();
    byte port = Wire.read();
    int value = Wire.read();
    value += Wire.read()*256;
    switch(cmd)
      {
        case 1:
          pinMode(port,OUTPUT);
          digitalWrite(port,value);
          break;
        case 2:
          pinMode(port,INPUT_PULLUP);
          valueRead = digitalRead(port);
          break;
        case 3:
          analogWrite(port,value);
          break;
        case 4:
          valueRead = analogRead(port)/4;
          break;
      }
  }
}

void requestEvent()
{
  Wire.write(valueRead & 0xff);
  //Wire.write(valueRead >> 8);
}

