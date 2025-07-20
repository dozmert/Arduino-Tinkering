## Description
Hacking a [Kmart Anko Smart Wi-Fi LED Sunset Light](https://www.kmart.com.au/product/smart-wi-fi-led-sunset-light-43128976/) in order to control the LEDs without the need of the Mirabella Genio Smart Home App 🤮.

## Build
### Hardware
### Components
- 1 x TZT Pro Micro ATmega32U4 5V 16MHz
-  1x LED Sunset Light

![SunsetLightHack](SunsetLightHack/img/sunsetlight_1.jpg)
## Teardown & Internals
Brief googling lead me to a teardown of this lamp by a user on the arduino.cc forums. This post can be found [here](https://forum.arduino.cc/t/hacking-a-sunset-lamp/1303684/18). The lamp is torn down and a WiFi/controller board can be found in the case where the on/off button is found.

![SunsetLightHack](SunsetLightHack/img/sunsetlight_2.jpg)
![](SunsetLightHack/img/sunsetlight_3.jpg)
![](SunsetLightHack/img/sunsetlight_4.jpg)

We can see from the images provided that the board is being powered by the 5V coming in via the USB connection on one side. On the other side we can see 4 sets of wires which is powering the LEDs. There appear to be labeled from top-down G, B, R, VCC (I confirmed this on my own light's board).

If we disconnect the controller cable away from the light, we are given 4 male pins that we can deduce are going to be that G, B, R, VCC.

![](SunsetLightHack/img/sunsetlight_5.jpg)
## TESTING
To start determining what each pin is, we can create a script that's going to create three pins that we can use along with the boards VCC.
```C++
// Simple test: Turn on pins 5, 6, and 9 (for R, G, B)
// For common-anode RGB: LOW = ON (0V), HIGH = OFF (5V)
void setup() {
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(5, LOW); // RED ON
  digitalWrite(6, LOW); // GREEN ON
  digitalWrite(9, LOW); // BLUE ON
}
void loop() {
  // Nothing to do here
}
```

![](SunsetLightHack/img/sunsetlight_6.jpg)

With a simple board layout I was able to determine what each pin was. With the pin that's marked on the cable housing to be VCC, the remaining three are going to be the the LED colors.

![](SunsetLightHack/img/sunsetlight_7.jpg)

An issue I found while testing was that the LEDs were not nearly as bright as they were compared to when they are running off the factory controller. I determined this to be a result of not having the correct resistors.

Using a multimeter I was able to determine that the default controller is sending 3v into the the VCC and the R,G,B pins are coming back with varying levels depending on the lighting option. Similarly I was able to determine that the VCC is sending in approximately 46mA which is returning as 43mA on the R,G,B pins.

I don't know a lot about electronics but from I could infer, the LEDs are common anode in which the VCC is shared and the RGB lines go low to GND to light up. I'll also need a controller that can sink current on GPIO pins to pull them low.

## Notes & Troubleshooting
- N/A