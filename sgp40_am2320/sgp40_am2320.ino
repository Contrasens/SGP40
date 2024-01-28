#include "U8glib.h"
#include <Wire.h>
#include "Adafruit_SGP40.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_AM2320.h"

// setup u8g object, please remove comment from one of the following constructor calls
// IMPORTANT NOTE: The following list is incomplete. The complete list of supported 
// devices with all constructor calls is here: https://github.com/olikraus/u8glib/wiki/device
//U8GLIB_SSD1306_128X64 u8g(13, 11, 10, 9);	// SW SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
//U8GLIB_SSD1306_128X64 u8g(4, 5, 6, 7);	// SW SPI Com: SCK = 4, MOSI = 5, CS = 6, A0 = 7 (new white HalTec OLED)
//U8GLIB_SSD1306_128X64 u8g(10, 9);		// HW SPI Com: CS = 10, A0 = 9 (Hardware Pins are  SCK = 13 and MOSI = 11)
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);	// I2C / TWI 
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);	// Fast I2C / TWI 
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);	// Display which does not send AC
//U8GLIB_SSD1306_ADAFRUIT_128X64 u8g(13, 11, 10, 9);	// SW SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
//U8GLIB_SSD1306_ADAFRUIT_128X64 u8g(10, 9);		// HW SPI Com: CS = 10, A0 = 9 (Hardware Pins are  SCK = 13 and MOSI = 11)


Adafruit_SGP40 sgp;
Adafruit_AM2320 am2320 = Adafruit_AM2320();
uint16_t t, h;
uint16_t voc_index, voc_index_adjusted;

void setup(void) {

  Serial.begin(115200);
  while (!Serial) { delay(10); } // Wait for serial console to open!

  Serial.println("SGP40 test with AM2320 compensation");

  if (! sgp.begin()){
    Serial.println("SGP40 VOC sensor not found :(");
    while (1);
  }

  if (! am2320.begin()) {  
    Serial.println("Couldn't find AM2320 Temperature & Humidity Sensor");
    while (1);
  }

  Serial.print("Found AM2320 + SGP40 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);

  // flip screen, if required
  // u8g.setRot180();
  
  // set SPI backup if required
  //u8g.setHardwareBackup(u8g_backup_avr_spi);

  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
  
  pinMode(8, OUTPUT);
}

void loop(void) {
  // picture loop
  t = (uint16_t) am2320.readTemperature();
  h = (uint16_t) am2320.readHumidity();
  
  //voc_index = sgp.measureVocIndex();
  voc_index_adjusted = sgp.measureVocIndex(t, h);

  Serial.print("Temp *C = "); Serial.print(t); Serial.print("\t\t"); Serial.print("Hum. % = "); Serial.print(h); Serial.print("\t\t"); 
  Serial.print("VOC = "); Serial.print(voc_index); Serial.print("\t\t"); Serial.print("VOC (adj) = "); Serial.println(voc_index_adjusted); 

  u8g.firstPage();  


  do {
    // VOC index
    u8g.setFont(u8g_font_fub17);
    u8g.drawStr( 0, 20, "VOC");
    //char voc[3];
    //sprintf (voc, "%d", voc_index);
    //u8g.drawStr( 40, 15, voc);    
    //u8g.drawStr( 80, 15, "/");
    char voca[3];
    sprintf (voca, "%d", voc_index_adjusted);
    u8g.drawStr(60, 20, voca);    

    u8g.setFont(u8g_font_helvB10);
    
    u8g.drawStr( 0, 45, "Temperature");
    u8g.drawStr( 117, 45, "C");
    char buft[3];
    sprintf (buft, "%d", t);
    u8g.drawStr(95, 45, buft);    

    u8g.drawStr( 0, 60, "Humidity");
    u8g.drawStr( 117, 60, "%");
    char bufh[3];
    sprintf (bufh, "%d", h);
    u8g.drawStr(95, 60, bufh);    

  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  //delay(50);
}

