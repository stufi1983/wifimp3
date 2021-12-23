#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x3f, 20, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
#include <DFMiniMp3.h>
class Mp3Notify
{
  public:
    static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
    {

    }
    static void OnError(uint16_t errorCode)
    {

    }
    static void OnPlayFinished(DfMp3_PlaySources source, uint16_t track)
    {

    }
    static void OnPlaySourceOnline(DfMp3_PlaySources source)
    {
    }
    static void OnPlaySourceInserted(DfMp3_PlaySources source)
    {
    }
    static void OnPlaySourceRemoved(DfMp3_PlaySources source)
    {
    }
};

DFMiniMp3<HardwareSerial, Mp3Notify> mp3(Serial);

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] =  {D4, D5, D6, D7};//connect to the row pinouts of the keypad
byte colPins[COLS] = {D0, D3, D8}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

char chr[255];
byte stringPos = 0;
long currentMillis;
long lastMillis = millis();

void setup() {
  mp3.begin();

  //  Serial.begin(9600);
  lcd.init();                      // initialize the lcd
  lcd.backlight();
  lcd.setCursor(0, 0); //kolom,baris
  lcd.print("- System Ready -");
  lcd.setCursor(0, 1); //kolom,baris
  lcd.print("Play: ");
  lastMillis = millis();
}


void loop() {

  char key = keypad.getKey();
  if (key != NO_KEY) {
    lcd.backlight();
    //    Serial.print(key);
    if (key == '#') {
      chr[stringPos] = '\0';
      stringPos = 0;
      //      Serial.println();
      lcd.setCursor(5, 1); //kolom,baris
      lcd.print(chr);
      lcd.print(" OK");
      requestoplay();
    }
    else if (key == '*') {
      resetString();
      resetDisplay();
    } else {
      chr[stringPos] = key;
      stringPos++;
      lcd.setCursor(5, 1); //kolom,baris
      lcd.print(chr);
    }
    lastMillis = millis();
  }

  if (( millis() - lastMillis) > 3000) {
    lcd.noBacklight();
  }

}

void resetDisplay() {
  //  Serial.println();
  lcd.setCursor(0, 1); //kolom,baris
  lcd.print("Play:           ");
}

void requestoplay() {
  mp3.playMp3FolderTrack(atoi(chr));  // sd:/mp3/0001.mp3
  mp3.loop();
  delay(1);

  delay(5000);
  resetString();
  resetDisplay();
}

void resetString() {
  stringPos = 0;
  for (byte a = 0; a < 255; a++) {
    chr[a] = '\0';
  }
}

