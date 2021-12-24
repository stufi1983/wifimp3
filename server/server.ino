#include "ESP8266WiFi.h"
#include "WiFiUdp.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

const char* WIFI_SSID = "B1014"; // change this to your own
const char* WIFI_PASSWORD = "lalunalolipoo"; // change this to your own
const unsigned int NET_PORT = 50000;

IPAddress local_IP(192, 168, 43, 151);
IPAddress gateway(192, 168, 43, 1);
IPAddress SendIP(192, 168, 43, 255);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional



WiFiUDP Udp;

char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
char NetMsg_Something[] = "YourMessage";

unsigned long nextBroadcastTimeMS = 0;

LiquidCrystal_I2C lcd(0x3f, 20, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
#include <DFMiniMp3.h>
class Mp3Notify
{
  public:
    static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
    {
      //lcd.print(" OK");
    }
    static void OnError(uint16_t errorCode)
    {
      //lcd.print(" Error");
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
  ConnectToNetwork();
  lcd.setCursor(0, 0); //kolom,baris
  lcd.print("- System Ready -");
  lcd.setCursor(0, 1); //kolom,baris
  resetDisplay();
  lcd.print("Play: ");
  lastMillis = millis();
}


void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    ConnectToNetwork();
    lcd.print("- System Ready -");
    lcd.setCursor(0, 1); //kolom,baris
    resetDisplay();
    lcd.print("Play: ");
    lastMillis = millis();
  }
  
  char key = keypad.getKey();
  if (key != NO_KEY) {
    lcd.backlight();
    //    Serial.print(key);
    if (key == '#') {
      chr[stringPos] = '\0';
      //      Serial.println();
      lcd.setCursor(5, 1); //kolom,baris
      lcd.print(chr);
      lcd.print(" OK");
      Udp.beginPacket(SendIP, NET_PORT);
      Udp.write(chr, stringPos); //Send one byte to ESP8266
      Udp.endPacket();
      requestoplay();
      stringPos = 0;
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

  //NetworkListen();
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

void ConnectToNetwork() {
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0); //kolom,baris
  lcd.print("Connecting....");
  lcd.setCursor(0, 1); //kolom,baris
  lcd.print(WIFI_SSID);

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    //Serial.println("STA Failed to configure");
  }

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  byte dotPos = 11;
  lcd.setCursor(dotPos, 0); //kolom,baris
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.print("."); dotPos++;
    if (dotPos > 16) {
      dotPos = 11;
      lcd.setCursor(dotPos, 0); //kolom,baris
      lcd.print("          ");
      lcd.setCursor(dotPos, 0); //kolom,baris
    }

  }
  lcd.clear();
  lcd.setCursor(0, 0); //kolom,baris
  lcd.print("I" + WiFi.localIP().toString());
  lcd.setCursor(0, 1); //kolom,baris
  lcd.print("S" + WiFi.subnetMask().toString());

  //  NetworkAddressCalculations();
  Udp.begin(NET_PORT);
  delay(2000);
}

void NetworkListen() {

  int packetSize = Udp.parsePacket();
  if (packetSize) {
    //    Serial.print("Received packet of size ");
    //    Serial.print(packetSize);
    //    Serial.print(" bytes of maximum size ");
    //    Serial.println(UDP_TX_PACKET_MAX_SIZE);
    //    Serial.print("From ");
    //    Serial.print(Udp.remoteIP().toString());
    //    Serial.print(", port ");
    //    Serial.println(Udp.remotePort());


    memset(packetBuffer, 0, sizeof(packetBuffer));
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    //lcd.setCursor(0, 0);
    //lcd.print(Udp.remoteIP().toString());

    resetDisplay();
    lcd.backlight();
    lcd.setCursor(0, 1);
    lcd.print(packetBuffer);

    //    Serial.println("Contents:");
    //    Serial.println(packetBuffer);
    //    if (strcmp(packetBuffer, NetMsg_Something) == 0){
    //      Serial.println("* This is what we were waiting for *");
    //    }
    delay(1000);
    resetDisplay();
  }
}
