# LAB 1

## Task A -> Change the color

Changed LED color to Green, Blue and White respectively by changing the RGB code.
`neopixelWrite(RGB_BUILTIN, 0, RGB_BRIGHTNESS, 0); //GREEN
neopixelWrite(RGB_BUILTIN, 0, 0, RGB_BRIGHTNESS); //BLUE
neopixelWrite(RGB_BUILTIN, RGB_BRIGHTNESS, RGB_BRIGHTNESS, RGB_BRIGHTNESS); //WHITE`

## Task B
Delay changed from 1000ms to 250ms.
By changing this delay, time between white LED being changing state from ON to OFF becomes shorter.
` //neopixelWrite(RGB_BUILTIN, RGB_BRIGHTNESS, RGB_BRIGHTNESS, RGB_BRIGHTNESS); //WHITE
  neopixelWrite(RGB_BUILTIN, RGB_BRIGHTNESS, 0, 0);
  delay(250); `
  
By changing this delay, time between white LED being changing state from OFF to ON becomes shorter. Cycle resets faster.
  `// LED OFF 
  neopixelWrite(RGB_BUILTIN, 0, 0, 0); 
  delay(250); `

## Task C

Repeated sequence  Red > Green > Blue > Off with a delay of 500ms was created.
` void loop() { 
  // Red ON
  neopixelWrite(RGB_BUILTIN, RGB_BRIGHTNESS, 0, 0);
  delay(500); 
  //Green ON
  neopixelWrite(RGB_BUILTIN, 0, RGB_BRIGHTNESS, 0);
  delay(500); 
   //Blue ON
  neopixelWrite(RGB_BUILTIN, 0, 0, RGB_BRIGHTNESS);
  delay(500); 
  // LED OFF 
  neopixelWrite(RGB_BUILTIN, 0, 0, 0); 
  delay(500); }`

## Task D
My custom repeated sequence Yellow > Violet > Cyan > Off with a delay of 800ms and RGB_BRIGHTNESS=10 (which makes the LED's blinking less bright) was created.

` #define RGB_BUILTIN 38 
#define RGB_BRIGHTNESS 10
void loop(){
  //Yellow LED ON
  neopixelWrite(RGB_BUILTIN, RGB_BRIGHTNESS, RGB_BRIGHTNESS, 0);
  delay(800);
  //Violet LED ON
  neopixelWrite(RGB_BUILTIN, RGB_BRIGHTNESS, 0, RGB_BRIGHTNESS);
  delay(800); 
  //Cyan LED ON
  neopixelWrite(RGB_BUILTIN, 0, RGB_BRIGHTNESS, RGB_BRIGHTNESS);
  delay(800); 
  // LED OFF 
  neopixelWrite(RGB_BUILTIN, 0, 0, 0); 
  delay(800); 
}`
