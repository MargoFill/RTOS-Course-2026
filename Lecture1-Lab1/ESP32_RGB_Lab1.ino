#define RGB_BUILTIN 38 
#define RGB_BRIGHTNESS 10 //50
 
void setup() { 
  // No separate initialisation is required for this example. 
} 
 

/* //TASK A
void loop() { 
  //neopixelWrite(RGB_BUILTIN, 0, RGB_BRIGHTNESS, 0); //GREEN
  //neopixelWrite(RGB_BUILTIN, 0, 0, RGB_BRIGHTNESS); //BLUE
  //neopixelWrite(RGB_BUILTIN, RGB_BRIGHTNESS, RGB_BRIGHTNESS, RGB_BRIGHTNESS); //WHITE
  neopixelWrite(RGB_BUILTIN, RGB_BRIGHTNESS, 0, 0);
  delay(1000); 

 
  // LED OFF 
  neopixelWrite(RGB_BUILTIN, 0, 0, 0); 
  delay(1000); 
} */


/* //TASK B
void loop() { 
  //neopixelWrite(RGB_BUILTIN, 0, RGB_BRIGHTNESS, 0); //GREEN
  //neopixelWrite(RGB_BUILTIN, 0, 0, RGB_BRIGHTNESS); //BLUE
  //neopixelWrite(RGB_BUILTIN, RGB_BRIGHTNESS, RGB_BRIGHTNESS, RGB_BRIGHTNESS); //WHITE
  neopixelWrite(RGB_BUILTIN, RGB_BRIGHTNESS, 0, 0);
  delay(250); 

 
  // LED OFF 
  neopixelWrite(RGB_BUILTIN, 0, 0, 0); 
  delay(250); 
} */

/* //TASK C
void loop() { 
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
  delay(500); 
} */

//TASK D
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


}