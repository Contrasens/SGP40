# VOC (Volatile Organic Compounds) index with ESP8266, SGP40 and BME280 Sensors

This project is built for an ESP8266 (Lolin D1 mini v4.0.0) with a 128x64 OLED display, SGP40 gas sensor, and BME280 temperature, pressure, humidity sensor.

## Description

This Arduino project is designed to read data from SGP40 gas sensor and BME280 temperature, pressure, humidity sensor, and display the readings on a 128x64 OLED display.

## Libraries Used

- [FastLED](https://github.com/FastLED/FastLED)
- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
- [Adafruit SGP40](https://github.com/adafruit/Adafruit_SGP40)
- [BMx280I2C](https://bitbucket.org/christandlg/bmx280mi/src/master/)

## Installation

1. Install [Arduino IDE](https://www.arduino.cc/en/software).
2. Open Arduino IDE and go to Sketch > Include Library > Manage Libraries.
3. Search for and install the libraries mentioned above.
4. Download this project and open the .ino file in Arduino IDE.

## BOM
I used the following hardware parts (I have no associations with any of the vendors!):
1. [D1 MINI V4.0.0](https://de.aliexpress.com/item/1005001621784437.html)
2. [SGP40](https://de.aliexpress.com/item/4000037083952.html)
3. [BME280-5V](https://de.aliexpress.com/item/1005003688067858.html) - make sure you buy the 5V version
4. [OLED Display Modul 128X64 I2C](https://de.aliexpress.com/item/32896971385.html) - I took the white I2C version

## Usage

1. Connect your Arduino board to your computer.
2. Select the correct board and port in the Arduino IDE.
3. Click the Upload button to compile the code and upload it to your Arduino board.

## Contributing

Contributions are welcome. Please open an issue or submit a pull request.

## License

This project is open source and available under the [MIT License](LICENSE).