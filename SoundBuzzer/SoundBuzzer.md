## Description
Make the buzzer play when desired
## Build
### Hardware
#### Components
- 1x Arduino Uno
-  1x Sound Buzzer Module
> Generic module.
![](SoundBuzzer/img/soundbuzzer_2.jpg)
### Software
Using the following code we can set pin8 on the Arduino to output constant power.
```C++
void setup() {
  pinMode(8, OUTPUT);
}
void loop() {
  digitalWrite(8, HIGH);
}
```

### Testing
Using trial and error, I was able to determined what the pins on the buzzer were for.
A -> GND
B -> VCC (5V)?
C -> SIGNAL

| Arduino Pin |  -  | Pin |
| :---------: | :-: | :-: |
|    Pin 9    | ->  | SIG |
|     GND     | ->  | GND |
![](SoundBuzzer/img/soundbuzzer_1.jpg)
Through this layout I was able to have the buzzer play a tone.

## Notes & Troubleshooting
- How the B Pin is used on the buzzer was unused during this project

## To Do
- N/A