### Description
Generic sound buzzer module
#### Usage
Allows for notifications or error codes to be audibly played back to the user.

### Build
#### Hardware
- 1x Arduino Uno
-  1x Sound Buzzer Module
#### Software
Using the following code we can set pin8 on the Arduino to output constant power.
```
void setup() {
  pinMode(8, OUTPUT);
}
void loop() {
  digitalWrite(8, HIGH);
}
```
From here we can determine which pins on the 3pin buzzer are to be used.
Through trial and error I was able to determine the following.
A -> GND
B -> ?
C -> SIG

![](/img/soundbuzzer_1.jpg)
![](/img/soundbuzzer_2.jpg)
### Notes & Troubleshooting

Pin B on the module is unknown at this time.