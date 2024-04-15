/**************************************************************************
This was written based on example code included with the libraries for:
- ESP8266 (Lolin D1 mini v4.0.0)
- 128x64 OLED display
- SGP40 gas sensor
- BME280 temperature, pressure, humidity sensor
 **************************************************************************/
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

// https://github.com/FastLED/FastLED
#include <FastLED.h>

// https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_GFX.h>

// https://github.com/adafruit/Adafruit_SSD1306
#include <Adafruit_SSD1306.h>

// https://github.com/adafruit/Adafruit_SGP40
#include "Adafruit_SGP40.h"
Adafruit_SGP40 sgp;

// https://bitbucket.org/christandlg/bmx280mi/src/master/
#include <BMx280I2C.h>
#define I2C_ADDRESS 0x76
//create a BMx280I2C object using the I2C interface with I2C Address 0x76
BMx280I2C bmx280(I2C_ADDRESS);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };

#define LED_PIN     2
#define NUM_LEDS    1
#define BRIGHTNESS  64
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

// Limits for LED indication
#define GRN_LIM     90
#define RED_LIM     200
#define BREATH_LIM  250
#define ALARM1_LIM  300
#define ALARM2_LIM  400

CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100
CRGBPalette256 palette;
TBlendType    currentBlending;

/**
 * @brief Initializes the necessary components and sets up the initial state of the program.
 * 
 * This function is called once when the program starts. It performs the following tasks:
 * - Initializes the LED pin and sets it to LOW.
 * - Adds LEDs to the FastLED library and sets the brightness.
 * - Initializes the serial communication.
 * - Checks if the SGP40 sensor is connected and prints its serial number.
 * - Checks if the BMx280 sensor is connected and prints its type.
 * - Resets the BMx280 sensor to default parameters.
 * - Sets oversampling settings for pressure, temperature, and humidity measurements.
 * - Sets the sensor to normal mode with 4 measurements per second.
 * - Initializes the SSD1306 display and shows an initial splash screen.
 * - Clears the display buffer.
 * - Displays the VOC and temperature on the screen.
 */
void setup() {
  digitalWrite (LED_PIN, LOW);
  pinMode (LED_PIN, OUTPUT);

  delay( 1000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(  BRIGHTNESS );

  Serial.begin(9600);
  while (!Serial) { delay(10); } // Wait for serial console to open!

  if (! sgp.begin()){
    Serial.println("SGP40 sensor not found :(");
    while (1);
  }

  Serial.print("Found SGP40 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);

	//begin() checks the Interface, reads the sensor ID (to differentiate between BMP280 and BME280)
	//and reads compensation parameters.
	if (!bmx280.begin())
	{
		Serial.println("begin() failed. check your BMx280 Interface and I2C Address.");
		while (1);
	}

	if (bmx280.isBME280())
		Serial.println("sensor is a BME280");
	else
		Serial.println("sensor is a BMP280");

	//reset sensor to default parameters.
	bmx280.resetToDefaults();

  //by default sensing is disabled and must be enabled by setting a non-zero
	//oversampling setting.
	//set an oversampling setting for pressure and temperature measurements. 
	bmx280.writeOversamplingPressure(BMx280MI::OSRS_P_x16);
	bmx280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);

  //if sensor is a BME280, set an oversampling setting for humidity measurements.
	if (bmx280.isBME280())
		bmx280.writeOversamplingHumidity(BMx280MI::OSRS_H_x16);

  //set the sensor to "normal" mode with 4 measurement per second:
  bmx280.writeStandbyTime(BMx280MI::T_SB_3);
  bmx280.writePowerMode(BMx280MI::BMx280_MODE_NORMAL);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(1000); // Pause for 1 second

  // Clear the buffer
  display.clearDisplay();

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  
  // display.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...

  displayVOC(45);    // Display VOC
  displayTem(45);
}

/**
 * @brief The main loop of the program.
 * 
 * This function is called repeatedly in the program. It performs the following tasks:
 * 1. Retrieves the temperature, humidity, and VOC (Volatile Organic Compounds) index from the sensors.
 * 2. Calculates the LED color based on the VOC index.
 * 3. Displays the VOC index, temperature, and humidity on a display.
 * 4. Updates the display.
 * 5. Waits for the measurement to finish before proceeding to the next iteration.
 * 
 * @note The measurement data is read from the sensor in the `hasValue()` function. 
 * Make sure to call the `get*()` functions only after `hasValue()` has returned true.
 */
void loop() {
  float temp = bmx280.getTemperature();
  float humi = bmx280.getHumidity();
  int32_t voc = sgp.measureVocIndex(temp, humi); 

  uint8_t colour = 0;
  colour = constrain (voc, GRN_LIM, RED_LIM);
  colour = map(colour, GRN_LIM, RED_LIM, 0, 255);
  led_colour(colour, voc);

  //wait for the measurement to finish
	do
	{
		delay(200);
	} while (!bmx280.hasValue());

  displayVOC(voc);
  displayTem(temp);
  displayHum(humi);
  display.display();
}

/**
 * @brief Displays the VOC (Volatile Organic Compounds) index on the display.
 * 
 * This function clears the display and prints the VOC index on the screen.
 * The VOC index is passed as a parameter to the function.
 * 
 * @param voc_index The VOC index to be displayed.
 */
void displayVOC(int32_t voc_index) {
  display.clearDisplay();

  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.print(F("VOC : ")); 
  display.println(voc_index);
}

/**
 * @brief Displays the temperature on the OLED display.
 * 
 * This function sets the text size, color, and cursor position on the display.
 * It then prints the temperature value followed by the unit "C" to indicate Celsius.
 * 
 * @param tem_index The temperature value to be displayed.
 */
void displayTem(int32_t tem_index) {

  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,20);      
  display.print("Temp: ");       
  display.print(tem_index);
  display.println("C");
}

/**
 * Displays the humidity index on the OLED display.
 * 
 * @param hum_index The humidity index to be displayed.
 */
void displayHum(int32_t hum_index) {

  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,40);           
  display.print("Hum : ");  
  display.print(hum_index);
  display.println("%");
}

/**
 * Sets the LED color based on the VOC (Volatile Organic Compounds) value.
 * 
 * @param colour The desired color of the LED.
 * @param voc The VOC value to determine the LED color.
 */
void led_colour(uint8_t colour, int32_t voc){
  if  (voc <= GRN_LIM) breathe(CRGB::Blue);
  if ((voc > GRN_LIM) && (voc <= RED_LIM)) colour_voc(voc, colour);
  if ((voc >RED_LIM) && (voc <= BREATH_LIM)) breathe(CRGB::Red);
  if ((voc >BREATH_LIM) && (voc <= ALARM1_LIM)) alert_1(CRGB::Red);
  if  (voc >ALARM2_LIM) alert_2(CRGB::Red);
}

/**
 * Sets the color of the first LED based on the VOC (Volatile Organic Compounds) value.
 * 
 * @param voc The VOC value to determine the color.
 * @param col The color index to use from the color palette.
 */
void colour_voc(int32_t voc, uint8_t col){

  palette = CRGBPalette256(CRGB::Green, CRGB::Red);
  leds[0] = ColorFromPalette(palette, col, BRIGHTNESS, currentBlending);
  FastLED.show();
}

/**
 * Breathe function that creates a breathing effect with a specified color.
 * 
 * @param colour The color to use for the breathing effect.
 */
void breathe(CRGB colour){
  CRGBPalette16 palette = CRGBPalette16(CRGB::Black, colour, CRGB::Black);
  leds[0] = CRGB::Black; FastLED.show();
  for (int i = 0; i <= 255; i++) {
    leds[0] = ColorFromPalette(palette, i, BRIGHTNESS, currentBlending);
    FastLED.show();
  }
}

/**
 * Sets the color of the first LED in the `leds` array to the specified `colour`,
 * displays the updated LED color using FastLED, and then waits for 2 seconds.
 * After that, sets the color of the first LED to white, displays the updated LED color,
 * and waits for 5 milliseconds.
 *
 * @param colour The color to set the first LED to.
 */
void alert_1(CRGB colour){
  leds[0] = colour;       FastLED.show(); delay (2000);
  leds[0] = CRGB::White;  FastLED.show();  delay (5);
}

/**
 * Displays an alert using the specified color on the first LED in the `leds` array.
 * The alert consists of a sequence of color changes and delays.
 *
 * @param colour The color to display for the alert.
 */
void alert_2(CRGB colour){
  leds[0] = colour;       FastLED.show(); delay (2000);
  leds[0] = CRGB::White;  FastLED.show(); delay (10);
  leds[0] = CRGB::Black;  FastLED.show(); delay (100);
  leds[0] = CRGB::White;  FastLED.show(); delay (10);
  leds[0] = CRGB::Black;  FastLED.show(); delay (10);
}

