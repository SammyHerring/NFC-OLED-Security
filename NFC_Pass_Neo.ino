//Largely based on Lewis Callaway's Instructable code:
// http://www.instructables.com/id/NFC-Computer-Unlocker
 
#include <Adafruit_PN532.h>
#include <Wire.h>
#include <SPI.h>
#include <Keyboard.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 12
#define DPI      1
Adafruit_SSD1306 display(OLED_RESET);

#ifdef __AVR__
  #include <avr/power.h>
#endif
 
#define IRQ 5
// this trace must be cut and rewired to work on Leonardo
#define RESET 9

#define PIN            6
#define NUMPIXELS      32

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 500; // delay for half a second

Adafruit_PN532 nfc(IRQ, RESET);
 
void setup() {
  
  #if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("Starting Up...");
  display.display();

  pixels.begin(); // This initializes the NeoPixel library.
  
  Serial.begin(9600);
  nfc.begin();
 
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    #define DPI      0
    //while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
 
  // configure board to read RFID tags
  nfc.SAMConfig();
  Keyboard.begin(); //initiate the keyboard
  pinMode(13, OUTPUT);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();

}
 
unsigned digit = 0;
 
void loop() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // Buffer to store the returned UID
  uint8_t uidLength; // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i, pixels.Color(0,0,0));
    pixels.show(); // This sends the updated pixel color to the hardware.
  }
 
  // wait for RFID card to show up!
  Serial.println("Waiting for an ISO14443A Card ...");

  for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(1000,100,0)); // Moderately bright green color.
      pixels.show(); // This sends the updated pixel color to the hardware.
    }
 
  // Wait for an ISO14443A type cards (Mifare, etc.). When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
 
  uint32_t cardidentifier = 0;
 
  if (DPI == 1) {
      display.clearDisplay();
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.print("Ready");
      display.display();
  
  if (success) {
    // Found a card!
    Serial.print("Card detected #");
    // turn the four byte UID of a mifare classic into a single variable #
    cardidentifier = uid[3];
    cardidentifier <<= 8; cardidentifier |= uid[2];
    cardidentifier <<= 8; cardidentifier |= uid[1];
    cardidentifier <<= 8; cardidentifier |= uid[0];
    Serial.println(cardidentifier);
 
    if (cardidentifier == 578365700) { //update with your RFID identifier!
      Keyboard.write('m');  //update with your password!
      Keyboard.write('y');
      Keyboard.write('p');
      Keyboard.write('a');
      Keyboard.write('s');
      Keyboard.write('s');
      Keyboard.write('w');
      Keyboard.write('o');
      Keyboard.write('r');
      Keyboard.write('d');
      Keyboard.write('\n'); // carriage return (ENTER key), remove if not desired
    for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(0,100,0)); // Moderately bright green color.
      pixels.show(); // This sends the updated pixel color to the hardware.
    }
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.println("Valid");
      display.println(cardidentifier);
      display.display();
       delay(1000); //makes sure the password isn't repeated
    }
    else {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.println("Invalid");
      display.println(cardidentifier);
      display.display();
      for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(100,0,0)); // Moderately bright green color.
      pixels.show(); // This sends the updated pixel color to the hardware.
      }
    delay(1000); //makes sure the password isn't repeated
    }
    }
  }
    else {
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print("No PN53x");
    display.display();
    }
  }

