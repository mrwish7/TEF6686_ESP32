#include <WiFiClient.h>
#include <EEPROM.h>
#include <Wire.h>
#include <TFT_eSPI.h>               // https://github.com/Bodmer/TFT_eSPI
#include <TimeLib.h>                // https://github.com/PaulStoffregen/Time
#include <Hash.h>                   // https://github.com/bbx10/Hash_tng
#include "src/WiFiConnect.h"
#include "src/WiFiConnectParam.h"
#include "src/font.h"
#include "src/TEF6686.h"
#include "src/constants.h"
#include "src/language.h"

#define GFXFF 1
#define FONT24 &Aura2ProRegular24pt7b
#define FONT14 &Aura2ProRegular14pt8b
#define FONT7  &Aura2ProRegular7pt8b

#define TFT_GREYOUT     0x38E7
#define ROTARY_PIN_A    34
#define ROTARY_PIN_B    36
#define ROTARY_BUTTON   39
#define PIN_POT         35
#define BATTERY_PIN     13
#define PWRBUTTON       4
#define BWBUTTON        25
#define MODEBUTTON      26
#define CONTRASTPIN     2
#define STANDBYLED      19
#define SMETERPIN       27
//#define ARS       // uncomment for BGR type display (ARS version)

#ifdef ARS
#define VERSION         "v2.00ARS"
#include "TFT_Colors.h"
TFT_eSPI tft = TFT_eSPI(320, 240);
#else
#define VERSION         "v2.00"
TFT_eSPI tft = TFT_eSPI(240, 320);
#endif

IPAddress remoteip;
bool nobattery;
byte af_counterold;
byte battery;
byte batteryold;
byte subnetclient;
int rssi;
int rssiold = 200;
bool edgebeep;
byte ECCold;
bool RDSSPYUSB;
bool RDSSPYTCP;
byte audiomode;
bool BWreset;
bool change2;
bool cleanup;
bool showsquelch;
byte amnb;
byte fmnb;
bool direction;
bool dropout;
bool EONold;
bool fullsearchrds;
bool showrdserrors;
bool LowLevelInit;
bool memorystore;
bool menu;
bool menuopen;
bool power = true;
bool RDSstatusold;
bool screenmute;
bool seek;
bool setupmode;
bool softmuteam;
bool softmutefm;
bool SQ;
bool Stereostatusold;
bool StereoToggle = true;
bool store;
bool TPold;
bool TAold;
bool tuned;
bool USBmode = 1;
bool XDRMute;
bool XDRGTKdata;
bool wifi;
bool wificonnected;
bool XDRGTKTCP;
bool XDRGTKUSB;
byte region;
byte regionold;
byte language;
byte theme;
byte tunemode;
byte memorypos;
String salt;
String saltkey = "                ";
byte memoryposold;
byte menupage = 1;
byte menupagestotal = 3;
byte MSold;
byte band;
byte BWset;
byte ContrastSet;
byte displayflip;
byte EQset;
byte freqoldcount;
byte iMSEQ;
byte iMSset;
byte memoryband[30];
byte optenc;
byte rotarymode;
byte SNR;
byte SNRold;
byte stepsize;
byte TEF;
char buff[16];
String programServicePrevious;
char programTypePrevious[17];
char radioIdPrevious[6];
String radioTextPrevious;
int AGC;
int BWOld;
int charWidth = tft.textWidth("AA");
unsigned int ConverterSet;
int DeEmphasis;
int ForceMono;
int freqold;
byte HighCutLevel;
byte HighCutOffset;
unsigned int HighEdgeSet;
int LevelOffset;
unsigned int LowEdgeSet;
int LowLevelSet;
unsigned int LWHighEdgeSet;
unsigned int LWLowEdgeSet;
unsigned int MWHighEdgeSet;
unsigned int MWLowEdgeSet;
unsigned int SWHighEdgeSet;
unsigned int SWLowEdgeSet;
byte showSWMIBand = 1;
uint16_t SWMIBandPos;
uint16_t SWMIBandPosold;  // Fix Me: Should store this parameter into flash, for use of restart.
String SWMIBandstring = String();
String SWMIBandstringold = String();
int lowsignaltimer;
int bwupdatetimer;
int snrupdatetimer;
int menuoption = 30;
int MStatusold;
int OStatusold;
int peakholdold;
int peakholdtimer;
int rotary;
int scanner_filter;
int Sqstatusold;
int Squelch;
int Squelchold;
int SStatusold;
byte StereoLevel;
byte showmodulation;
int Stereostatus;
int VolSet;
int volume;
int XDRBWset;
int XDRBWsetold;
int xPos = 6;
int yPos = 2;
int16_t OStatus;
int16_t SAvg;
int16_t SAvg2;
int16_t SStatus;
String PIold;
String PSold;
String PTYold;
String rds_clock;
String rds_clockold;
String cryptedpassword;
String RDSSPYRDS;
String RDSSPYRDSold;
String RTold;
String XDRGTK_key;
String XDRGTKRDS;
String XDRGTKRDSold;
uint16_t BW;
uint16_t MStatus;
uint16_t USN;
uint16_t WAM;
uint8_t buff_pos;
uint8_t RDSstatus;
unsigned int change;
unsigned int freq_scan;
unsigned int frequency;
unsigned int frequency_AM;
unsigned int frequency_LW;
unsigned int frequency_MW;
unsigned int frequency_SW;
unsigned int frequencyold;
unsigned int frequency_MIBand_11M; // Surive before tuner restart
unsigned int frequency_MIBand_13M;
unsigned int frequency_MIBand_15M;
unsigned int frequency_MIBand_16M;
unsigned int frequency_MIBand_19M;
unsigned int frequency_MIBand_22M;
unsigned int frequency_MIBand_25M;
unsigned int frequency_MIBand_31M;
unsigned int frequency_MIBand_41M;
unsigned int frequency_MIBand_49M;
unsigned int frequency_MIBand_60M;
unsigned int frequency_MIBand_75M;
unsigned int frequency_MIBand_90M;
unsigned int frequency_MIBand_120M;
unsigned int frequency_MIBand_160M;
unsigned int memory[30];
unsigned int scanner_end;
unsigned int scanner_start;
unsigned int scanner_step;
unsigned long peakholdmillis;
unsigned long rtticker;

TEF6686 radio;
TFT_eSprite sprite = TFT_eSprite(&tft);
WiFiConnect wc;
WiFiServer Server(7373);
WiFiClient RemoteClient;
WiFiUDP Udp;

void setup() {
  setupmode = true;
  EEPROM.begin(249);
  if (EEPROM.readByte(43) != 27) DefaultSettings();

  frequency = EEPROM.readUInt(0);
  VolSet = EEPROM.readInt(4);
  ConverterSet = EEPROM.readUInt(8);
  LowEdgeSet = EEPROM.readUInt(12);
  HighEdgeSet = EEPROM.readUInt(16);
  ContrastSet = EEPROM.readByte(20);
  StereoLevel = EEPROM.readByte(21);
  HighCutLevel = EEPROM.readByte(22);
  HighCutOffset = EEPROM.readByte(23);
  LevelOffset = EEPROM.readInt(24);
  edgebeep = EEPROM.readByte(28);
  softmuteam = EEPROM.readByte(29);
  softmutefm = EEPROM.readByte(30);
  frequency_AM = EEPROM.readUInt(31);
  language = EEPROM.readByte(35);
  showrdserrors = EEPROM.readByte(36);
  TEF = EEPROM.readByte(37);
  displayflip = EEPROM.readByte(38);
  rotarymode = EEPROM.readByte(39);
  stepsize = EEPROM.readByte(40);
  tunemode = EEPROM.readByte(41);
  optenc = EEPROM.readByte(42);
  iMSset = EEPROM.readByte(44);
  EQset = EEPROM.readByte(45);
  band = EEPROM.readByte(46);
  LowLevelSet = EEPROM.readInt(47);
  memorypos = EEPROM.readByte(51);
  region = EEPROM.readByte(52);
  radio.rds.underscore = EEPROM.readByte(53);
  USBmode = EEPROM.readByte(54);
  wifi = EEPROM.readByte(55);
  subnetclient = EEPROM.readByte(56);
  showSWMIBand = EEPROM.readByte(57);
  radio.rds.filter = EEPROM.readByte(58);
  radio.rds.dynamicpi = EEPROM.readByte(59);
  frequency_LW = EEPROM.readUInt(221);
  frequency_MW = EEPROM.readUInt(225);
  frequency_SW = EEPROM.readUInt(229);
  XDRGTK_key = EEPROM.readString(233);
  showsquelch = EEPROM.readByte(244);
  showmodulation = EEPROM.readByte(245);
  amnb = EEPROM.readByte(246);
  fmnb = EEPROM.readByte(247);
  audiomode = EEPROM.readByte(248);

  LWLowEdgeSet = FREQ_LW_LOW_EDGE_MIN;   // later will read from flash
  LWHighEdgeSet = FREQ_LW_HIGH_EDGE_MAX; // later will read from flash
  MWLowEdgeSet = region == 0 ? FREQ_MW_LOW_EDGE_MIN_9K : FREQ_MW_LOW_EDGE_MIN_10K;      // later will read from flash
  MWHighEdgeSet = region == 0 ? FREQ_MW_HIGH_EDGE_MAX_9K : FREQ_MW_HIGH_EDGE_MAX_10K;   // later will read from flash
  SWLowEdgeSet = FREQ_SW_LOW_EDGE_MIN;
  SWHighEdgeSet = FREQ_SW_HIGH_EDGE_MAX;

  for (int i = 0; i < 30; i++) memoryband[i] = EEPROM.readByte(i + 60);
  for (int i = 0; i < 30; i++) memory[i] = EEPROM.readUInt((i * 4) + 100);
  btStop();

  if (USBmode) Serial.begin(19200); else Serial.begin(115200);

  if (iMSset == 1 && EQset == 1) iMSEQ = 2;
  if (iMSset == 0 && EQset == 1) iMSEQ = 3;
  if (iMSset == 1 && EQset == 0) iMSEQ = 4;
  if (iMSset == 0 && EQset == 0) iMSEQ = 1;

  switch (band) {
    case BAND_LW:
      frequency_LW = frequency_AM;
      if (stepsize > 3) stepsize = 3;
      break;
    case BAND_MW:
      frequency_MW = frequency_AM;
      if (stepsize > 3) stepsize = 3;
      break;
    case BAND_SW: frequency_SW = frequency_AM; break;
    default: break;
  }

  tft.init();

  if (displayflip == 0) {
#ifdef ARS
    tft.setRotation(0);
#else
    tft.setRotation(3);
#endif
  } else {
#ifdef ARS
    tft.setRotation(2);
#else
    tft.setRotation(1);
#endif
  }

  TEF = EEPROM.readByte(37);

  if (TEF != 101 && TEF != 102 && TEF != 205) SetTunerPatch();

  radio.init(TEF);
  uint16_t device;
  uint16_t hw;
  uint16_t sw;
  radio.getIdentification(device, hw, sw);
  if (TEF != (highByte(hw) * 100 + highByte(sw))) SetTunerPatch();

  analogWrite(CONTRASTPIN, ContrastSet * 2 + 27);
  analogWrite(SMETERPIN, 0);

  radio.getIdentification(device, hw, sw);
  uint8_t version = highByte(hw) * 100 + highByte(sw);

  pinMode(MODEBUTTON, INPUT);
  pinMode(BWBUTTON, INPUT);
  pinMode(ROTARY_BUTTON, INPUT);
  pinMode(ROTARY_PIN_A, INPUT);
  pinMode(ROTARY_PIN_B, INPUT);
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_A), read_encoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_B), read_encoder, CHANGE);

  if (digitalRead(BWBUTTON) == LOW && digitalRead(ROTARY_BUTTON) == HIGH) {
    if (rotarymode == 0) rotarymode = 1; else rotarymode = 0;
    EEPROM.writeByte(39, rotarymode);
    EEPROM.commit();
    tft.setFreeFont(FONT14);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.drawCentreString(myLanguage[language][1], 155, 77, GFXFF);
    tft.drawCentreString(myLanguage[language][2], 150, 107, GFXFF);
    while (digitalRead(BWBUTTON) == LOW) delay(50);
  }

  if (digitalRead(MODEBUTTON) == LOW) {
    if (displayflip == 0) {
      displayflip = 1;
      tft.setRotation(1);
    } else {
      displayflip = 0;
      tft.setRotation(3);
    }
    EEPROM.writeByte(38, displayflip);
    EEPROM.commit();
    tft.setFreeFont(FONT14);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.drawCentreString(myLanguage[language][3], 155, 77, GFXFF);
    tft.drawCentreString(myLanguage[language][2], 150, 107, GFXFF);
    while (digitalRead(MODEBUTTON) == LOW) delay(50);
  }

  if (digitalRead(PWRBUTTON) == LOW) {
    analogWrite(SMETERPIN, 511);
    tft.setFreeFont(FONT14);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.drawCentreString(myLanguage[language][4], 155, 77, GFXFF);
    tft.drawCentreString(myLanguage[language][5], 150, 107, GFXFF);
    while (digitalRead(PWRBUTTON) == LOW) delay(50);
    analogWrite(SMETERPIN, 0);
  }

  if (digitalRead(ROTARY_BUTTON) == LOW && digitalRead(BWBUTTON) == HIGH) {
    tft.setFreeFont(FONT14);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    if (optenc == 0) {
      optenc = 1;
      tft.drawCentreString(myLanguage[language][6], 155, 77, GFXFF);
    } else {
      optenc = 0;
      tft.drawCentreString(myLanguage[language][7], 155, 77, GFXFF);
    }
    EEPROM.writeByte(42, optenc);
    EEPROM.commit();
    tft.drawCentreString(myLanguage[language][2], 150, 107, GFXFF);
    while (digitalRead(ROTARY_BUTTON) == LOW) delay(50);
  }

  if (digitalRead(ROTARY_BUTTON) == LOW && digitalRead(BWBUTTON) == LOW) {
    tft.setFreeFont(FONT14);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    DefaultSettings();
    tft.drawCentreString(myLanguage[language][66], 155, 77, GFXFF);
    tft.drawCentreString(myLanguage[language][2], 150, 107, GFXFF);
    while (digitalRead(ROTARY_BUTTON) == LOW && digitalRead(BWBUTTON) == LOW) delay(50);
    ESP.restart();
  }

  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);
  tft.pushImage (0, 99, 211, 140, pe5pvblogo);
  tft.pushImage (239, 200, 80, 30, nxplogo);
  tft.setFreeFont(FONT14);
  tft.setTextColor(TFT_SKYBLUE);
  tft.drawCentreString(myLanguage[language][8], 160, 17, GFXFF);
  tft.setFreeFont(FONT7);
  tft.setTextColor(TFT_YELLOW);
  tft.drawCentreString("Software " + String(VERSION), 160, 48, GFXFF);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("Lithio", 260, 191, GFXFF);

  if (lowByte(device) == 14) {
    fullsearchrds = false;
    tft.drawString("Tuner: TEF6686 Lithio (" + String(version) + ")", 80, 64, GFXFF);
  } else if (lowByte(device) == 1) {
    fullsearchrds = true;
    tft.drawString("Tuner: TEF6687 Lithio FMSI (" + String(version) + ")", 80, 64, GFXFF);
  } else if (lowByte(device) == 9) {
    fullsearchrds = false;
    tft.drawString("Tuner: TEF6688 Lithio DR (" + String(version) + ")", 80, 64, GFXFF);
  } else if (lowByte(device) == 3) {
    fullsearchrds = true;
    tft.drawString("Tuner: TEF6689 Lithio FMSI DR (" + String(version) + ")", 80, 64, GFXFF);
  } else {
    tft.setTextColor(TFT_RED);
    tft.drawString(myLanguage[language][9], 80, 64, GFXFF);
    while (true);
    for (;;);
  }
  tft.drawString("Patch: v" + String(TEF), 80, 79, GFXFF);
  delay(1500);

  radio.setVolume(VolSet);
  radio.setOffset(LevelOffset);
  radio.setStereoLevel(StereoLevel);
  radio.setHighCutLevel(HighCutLevel);
  radio.setHighCutOffset(HighCutOffset);
  radio.clearRDS(fullsearchrds);
  radio.setMute();
  radio.setSoftmuteFM(softmutefm);
  radio.setSoftmuteAM(softmuteam);
  radio.setAMNoiseBlanker(amnb);
  radio.setFMNoiseBlanker(fmnb);
  radio.setAudio(audiomode);
  if (region == 1) radio.setDeemphasis(2);
  radio.rds.region = region;
  LowLevelInit = true;

  if (ConverterSet >= 200) {
    Wire.beginTransmission(0x12);
    Wire.write(ConverterSet >> 8);
    Wire.write(ConverterSet & (0xFF));
    Wire.endTransmission();
  }

  if (wifi == true) {
    tryWiFi();
    delay(2000);
  } else {
    Server.end();
    Udp.stop();
  }

  SelectBand();
  ShowSignalLevel();
  ShowBW();
  setupmode = false;
  sprite.createSprite(317, 16);
  radio.tone(50, -5, 2000);
}

void loop() {
  Communication();

  if (digitalRead(PWRBUTTON) == LOW && XDRGTKUSB == false && XDRGTKTCP == false) PWRButtonPress();

  if (power == true) {
    if (seek == true) Seek(direction);

    if ((SStatus / 10 > LowLevelSet) && LowLevelInit == false && menu == false && band == BAND_FM) {
      if (screenmute == false) {
        tft.setFreeFont(FONT7);
        tft.setTextColor(TFT_WHITE);
        if (showmodulation) {
          tft.drawString("20", 20, 146, GFXFF);
          tft.drawString("40", 50, 146, GFXFF);
          tft.drawString("60", 80, 146, GFXFF);
          tft.drawString("80", 110, 146, GFXFF);
          tft.drawString("100", 134, 146, GFXFF);
          tft.drawString("120", 164, 146, GFXFF);
          tft.drawString("%", 196, 146, GFXFF);
          tft.drawString("M", 6, 132, GFXFF);
        }
        if (region == 0) tft.drawString("PI:", 216, 191, GFXFF);
        if (region == 1) tft.drawString("ID:", 216, 191, GFXFF);
        tft.drawString("PS:", 6, 191, GFXFF);
        tft.drawString("PTY:", 6, 164, GFXFF);
        if (!showmodulation) tft.drawLine(20, 150, 200, 150, TFT_GREYOUT); else tft.drawLine(20, 150, 200, 150, TFT_DARKGREY);
      }
      LowLevelInit = true;
    }

    if ((SStatus / 10 <= LowLevelSet) && band == BAND_FM) {
      if (LowLevelInit == true && menu == false) {
        if (screenmute == false) {
          tft.setFreeFont(FONT7);
          tft.fillRect(20, 139, 12, 8, TFT_GREYOUT);
          tft.fillRect(34, 139, 12, 8, TFT_GREYOUT);
          tft.fillRect(48, 139, 12, 8, TFT_GREYOUT);
          tft.fillRect(62, 139, 12, 8, TFT_GREYOUT);
          tft.fillRect(76, 139, 12, 8, TFT_GREYOUT);
          tft.fillRect(90, 139, 12, 8, TFT_GREYOUT);
          tft.fillRect(104, 139, 12, 8, TFT_GREYOUT);
          tft.fillRect(118, 139, 12, 8, TFT_GREYOUT);
          tft.fillRect(132, 139, 12, 8, TFT_GREYOUT);
          tft.fillRect(146, 139, 12, 8, TFT_GREYOUT);
          tft.fillRect(160, 139, 12, 8, TFT_GREYOUT);
          tft.fillRect(174, 139, 12, 8, TFT_GREYOUT);
          tft.fillRect(188, 139, 12, 8, TFT_GREYOUT);
          tft.setTextColor(TFT_GREYOUT);
          if (showmodulation) {
            tft.drawString("20", 20, 146, GFXFF);
            tft.drawString("40", 50, 146, GFXFF);
            tft.drawString("60", 80, 146, GFXFF);
            tft.drawString("80", 110, 146, GFXFF);
            tft.drawString("100", 134, 146, GFXFF);
            tft.drawString("120", 164, 146, GFXFF);
            tft.drawString("%", 196, 146, GFXFF);
            tft.drawString("M", 6, 132, GFXFF);
          }
          if (region == 0) tft.drawString("PI:", 216, 191, GFXFF);
          if (region == 1) tft.drawString("ID:", 216, 191, GFXFF);
          tft.drawString("PS:", 6, 191, GFXFF);
          tft.drawString("PTY:", 6, 164, GFXFF);
          tft.drawLine(20, 150, 200, 150, TFT_GREYOUT);
          tft.drawBitmap(110, 5, RDSLogo, 67, 22, TFT_GREYOUT);
        }
        LowLevelInit = false;
      }

      if (millis() >= lowsignaltimer + 300) {
        lowsignaltimer = millis();
        if (band == BAND_FM) radio.getStatus(SStatus, USN, WAM, OStatus, BW, MStatus); else radio.getStatusAM(SStatus, USN, WAM, OStatus, BW, MStatus);
        if (screenmute == true) readRds();
        if (menu == false) {
          doSquelch();
          GetData();
        }
      }

    } else {
      if (band == BAND_FM) radio.getStatus(SStatus, USN, WAM, OStatus, BW, MStatus); else radio.getStatusAM(SStatus, USN, WAM, OStatus, BW, MStatus);
      if (menu == false) {
        doSquelch();
        readRds();
        GetData();
        if (screenmute == false) ShowModLevel();
      }
    }

    if (menu == true && menuopen == true && menupage == 1 && menuoption == 110) {
      if (band == BAND_FM) radio.getStatus(SStatus, USN, WAM, OStatus, BW, MStatus); else radio.getStatusAM(SStatus, USN, WAM, OStatus, BW, MStatus);
      if (millis() >= lowsignaltimer + 500 || change2 == true) {
        lowsignaltimer = millis();
        change2 = false;
        if (SStatus > SStatusold || SStatus < SStatusold) {
          tft.setFreeFont(FONT24);
          tft.setTextColor(TFT_BLACK);
          if (SStatus / 10 != SStatusold / 10) tft.drawRightString(String(SStatusold / 10), 150, 146, GFXFF);
          tft.setFreeFont(FONT14);
          tft.drawString("." + String(abs(SStatusold % 10)), 156, 137, GFXFF);
          tft.setFreeFont(FONT24);
          tft.setTextColor(TFT_SKYBLUE);
          tft.drawRightString(String(SStatus / 10), 150, 146, GFXFF);
          tft.setFreeFont(FONT14);
          tft.drawString("." + String(abs(SStatus % 10)), 156, 137, GFXFF);
          SStatusold = SStatus;
        }
      }
    }

    if (rotary == -1) KeyUp();
    if (rotary == 1) KeyDown();

    if (digitalRead(ROTARY_BUTTON) == LOW) ButtonPress();
    if (digitalRead(MODEBUTTON) == LOW && screenmute == false) ModeButtonPress();
    if (digitalRead(BWBUTTON) == LOW && screenmute == false) BWButtonPress();

    if (store == true) change++;

    if (change > 200 && store == true) {
      detachInterrupt(digitalPinToInterrupt(ROTARY_PIN_A));
      detachInterrupt(digitalPinToInterrupt(ROTARY_PIN_B));
      EEPROM.writeUInt(0, frequency);
      EEPROM.writeUInt(31, frequency_AM);
      EEPROM.writeByte(46, band);
      EEPROM.writeUInt(221, frequency_LW);
      EEPROM.writeUInt(225, frequency_MW);
      EEPROM.writeUInt(229, frequency_SW);
      EEPROM.commit();
      store = false;
      attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_A), read_encoder, CHANGE);
      attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_B), read_encoder, CHANGE);
    }
  }
}

void GetData() {
  if (screenmute == false) {
    if (band == BAND_FM) {
      showPI();
      showPTY();
      showCT();
      showPS();
      showTP();
      showTA();
      showMS();
      showEON();
      showAF();
      showECC();
      showRadioText();
      ShowStereoStatus();
    }
    ShowRSSI();
    ShowBattery();
    ShowOffset();
    ShowSignalLevel();
    ShowBW();
  }
}

void PWRButtonPress() {
  if (menu == false) {
    unsigned long counterold = millis();
    unsigned long counter = millis();
    while (digitalRead(PWRBUTTON) == LOW && counter - counterold <= 1000) counter = millis();

    if (counter - counterold < 1000) {
      if (power == false) {
        ESP.restart();
      } else {
        if (tunemode != TUNE_MEM) {
          if (band == BAND_FM) {
            band = BAND_LW;
            if (stepsize > 3) stepsize = 3;
          }
          else if (band == BAND_LW) band = BAND_MW;
          else if (band == BAND_MW) band = BAND_SW;
          else if (band == BAND_SW) band = BAND_FM;
          StoreFrequency();
          SelectBand();
        }
      }
    } else {
      if (power == false) {
        ESP.restart();
      } else {
        power = false;
        analogWrite(SMETERPIN, 0);
        analogWrite(CONTRASTPIN, 0);
        pinMode (STANDBYLED, OUTPUT);
        digitalWrite(STANDBYLED, LOW);
        StoreFrequency();
        radio.power(1);
      }
    }
    while (digitalRead(PWRBUTTON) == LOW) delay(50);
    delay(100);
  }
}

void StoreFrequency() {
  EEPROM.writeUInt(0, frequency);
  EEPROM.writeUInt(31, frequency_AM);
  EEPROM.writeByte(46, band);
  EEPROM.writeUInt(221, frequency_LW);
  EEPROM.writeUInt(225, frequency_MW);
  EEPROM.writeUInt(229, frequency_SW);
  EEPROM.commit();
}

void LimitAMFrequency() {
  switch (band) {
    case BAND_LW:
      frequency_AM = frequency_LW;
      if (frequency_AM > LWHighEdgeSet || frequency_AM < LWLowEdgeSet) {
        frequency_AM = LWLowEdgeSet;
      }
      break;
    case BAND_MW:
      frequency_AM = frequency_MW;
      if (frequency_AM > MWHighEdgeSet || frequency_AM < MWLowEdgeSet) {
        frequency_AM = MWLowEdgeSet; // take care of 9K/10K step
      }
      break;
    case BAND_SW:
      frequency_AM = frequency_SW;
      if (frequency_AM > SWHighEdgeSet || frequency_AM < SWLowEdgeSet) {
        frequency_AM = SWLowEdgeSet;
      }
      break;
  }
}

void DivdeSWMIBand() {
  if (frequency_AM >= FREQ_SW_160M_START && frequency_AM <= FREQ_SW_160M_END) {
    SWMIBandPos = SW_MI_BAND_160M;
    frequency_MIBand_160M = frequency_AM;
    SWMIBandstring = (String) "" + SWMIBandPos + " m";
  } else if (frequency_AM >= FREQ_SW_120M_START && frequency_AM <= FREQ_SW_120M_END) {
    SWMIBandPos = SW_MI_BAND_120M;
    frequency_MIBand_120M = frequency_AM;
    SWMIBandstring = (String) "" + SWMIBandPos + " m";
  } else if (frequency_AM >= FREQ_SW_90M_START && frequency_AM <= FREQ_SW_90M_END) {
    SWMIBandPos = SW_MI_BAND_90M;
    frequency_MIBand_90M = frequency_AM;
    SWMIBandstring = (String) "" + SWMIBandPos + " m";
  } else if (frequency_AM >= FREQ_SW_75M_START && frequency_AM <= FREQ_SW_75M_END) {
    SWMIBandPos = SW_MI_BAND_75M;
    frequency_MIBand_75M = frequency_AM;
    SWMIBandstring = (String) "" + SWMIBandPos + " m";
  } else if (frequency_AM >= FREQ_SW_60M_START && frequency_AM <= FREQ_SW_60M_END) {
    SWMIBandPos = SW_MI_BAND_60M;
    frequency_MIBand_60M = frequency_AM;
    SWMIBandstring = (String) "" + SWMIBandPos + " m";
  } else if (frequency_AM >= FREQ_SW_49M_START && frequency_AM <= FREQ_SW_49M_END) {
    SWMIBandPos = SW_MI_BAND_49M;
    frequency_MIBand_49M = frequency_AM;
    SWMIBandstring = (String) "" + SWMIBandPos + " m";
  } else if (frequency_AM >= FREQ_SW_41M_START && frequency_AM <= FREQ_SW_41M_END) {
    SWMIBandPos = SW_MI_BAND_41M;
    frequency_MIBand_41M = frequency_AM;
    SWMIBandstring = (String) "" + SWMIBandPos + " m";
  } else if (frequency_AM >= FREQ_SW_31M_START && frequency_AM <= FREQ_SW_31M_END) {
    SWMIBandPos = SW_MI_BAND_31M;
    frequency_MIBand_31M = frequency_AM;
    SWMIBandstring = (String) "" + SWMIBandPos + " m";
  } else if (frequency_AM >= FREQ_SW_25M_START && frequency_AM <= FREQ_SW_25M_END) {
    SWMIBandPos = SW_MI_BAND_25M;
    frequency_MIBand_25M = frequency_AM;
    SWMIBandstring = (String) "" + SWMIBandPos + " m";
  } else if (frequency_AM >= FREQ_SW_22M_START && frequency_AM <= FREQ_SW_22M_END) {
    SWMIBandPos = SW_MI_BAND_22M;
    frequency_MIBand_22M = frequency_AM;
    SWMIBandstring = (String) "" + SWMIBandPos + " m";
  } else if (frequency_AM >= FREQ_SW_19M_START && frequency_AM <= FREQ_SW_19M_END) {
    SWMIBandPos = SW_MI_BAND_19M;
    frequency_MIBand_19M = frequency_AM;
    SWMIBandstring = (String) "" + SWMIBandPos + " m";
  } else if (frequency_AM >= FREQ_SW_16M_START && frequency_AM <= FREQ_SW_16M_END) {
    SWMIBandPos = SW_MI_BAND_16M;
    frequency_MIBand_16M = frequency_AM;
    SWMIBandstring = (String) "" + SWMIBandPos + " m";
  } else if (frequency_AM >= FREQ_SW_15M_START && frequency_AM <= FREQ_SW_15M_END) {
    SWMIBandPos = SW_MI_BAND_15M;
    frequency_MIBand_15M = frequency_AM;
    SWMIBandstring = (String) "" + SWMIBandPos + " m";
  } else if (frequency_AM >= FREQ_SW_13M_START && frequency_AM <= FREQ_SW_13M_END) {
    SWMIBandPos = SW_MI_BAND_13M;
    frequency_MIBand_13M = frequency_AM;
    SWMIBandstring = (String) "" + SWMIBandPos + " m";
  } else if (frequency_AM >= FREQ_SW_11M_START && frequency_AM <= FREQ_SW_11M_END) {
    SWMIBandPos = SW_MI_BAND_11M;
    frequency_MIBand_11M = frequency_AM;
    SWMIBandstring = (String) "" + SWMIBandPos + " m";
  } else {
    if (SWMIBandPos > SW_MI_BAND_GAP) {
      SWMIBandPosold = SWMIBandPos;
      SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
    }
    SWMIBandPos = SW_MI_BAND_GAP;
  }
}

void ToggleSWMIBand(bool plus) {
  switch (SWMIBandPosold) {
    case SW_MI_BAND_GAP:
      if (plus) {
        SWMIBandPos = SW_MI_BAND_160M;
        SWMIBandPosold = SWMIBandPos;
        frequency_AM = FREQ_SW_160M_START;
      } else {
        SWMIBandPos = SW_MI_BAND_11M;
        SWMIBandPosold = SWMIBandPos;
        frequency_AM = FREQ_SW_11M_END;
      }
      radio.SetFreqAM(frequency_AM);
      frequency_SW = frequency_AM;
      break;
    case SW_MI_BAND_11M:
      if (plus) {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_160M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_160M == 0 ? FREQ_SW_160M_START : frequency_MIBand_160M;
      } else {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_13M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_13M == 0 ? FREQ_SW_13M_END : frequency_MIBand_13M;
      }
      radio.SetFreqAM(frequency_AM);
      frequency_SW = frequency_AM;
      break;
    case SW_MI_BAND_13M:
      if (plus) {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_11M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_11M == 0 ? FREQ_SW_11M_START : frequency_MIBand_11M;
      } else {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_15M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_15M == 0 ? FREQ_SW_15M_END : frequency_MIBand_15M;
      }
      radio.SetFreqAM(frequency_AM);
      frequency_SW = frequency_AM;
      break;
    case SW_MI_BAND_15M:
      if (plus) {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_13M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_13M == 0 ? FREQ_SW_13M_START : frequency_MIBand_13M;
      } else {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_16M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_16M == 0 ? FREQ_SW_16M_END : frequency_MIBand_16M;
      }
      radio.SetFreqAM(frequency_AM);
      frequency_SW = frequency_AM;
      break;
    case SW_MI_BAND_16M:
      if (plus) {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_15M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_15M == 0 ? FREQ_SW_15M_START : frequency_MIBand_15M;
      } else {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_19M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_19M == 0 ? FREQ_SW_19M_END : frequency_MIBand_19M;
      }
      radio.SetFreqAM(frequency_AM);
      frequency_SW = frequency_AM;
      break;
    case SW_MI_BAND_19M:
      if (plus) {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_16M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_16M == 0 ? FREQ_SW_16M_START : frequency_MIBand_16M;
      } else {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_22M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_22M == 0 ? FREQ_SW_22M_END : frequency_MIBand_22M;
      }
      radio.SetFreqAM(frequency_AM);
      frequency_SW = frequency_AM;
      break;
    case SW_MI_BAND_22M:
      if (plus) {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_19M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_19M == 0 ? FREQ_SW_19M_START : frequency_MIBand_19M;
      } else {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_25M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_25M == 0 ? FREQ_SW_25M_END : frequency_MIBand_25M;
      }
      radio.SetFreqAM(frequency_AM);
      frequency_SW = frequency_AM;
      break;
    case SW_MI_BAND_25M:
      if (plus) {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_22M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_22M == 0 ? FREQ_SW_22M_START : frequency_MIBand_22M;
      } else {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_31M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_31M == 0 ? FREQ_SW_31M_END : frequency_MIBand_31M;
      }
      radio.SetFreqAM(frequency_AM);
      frequency_SW = frequency_AM;
      break;
    case SW_MI_BAND_31M:
      if (plus) {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_25M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_25M == 0 ? FREQ_SW_25M_START : frequency_MIBand_25M;
      } else {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_41M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_41M == 0 ? FREQ_SW_41M_END : frequency_MIBand_41M;
      }
      radio.SetFreqAM(frequency_AM);
      frequency_SW = frequency_AM;
      break;
    case SW_MI_BAND_41M:
      if (plus) {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_31M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_31M == 0 ? FREQ_SW_31M_START : frequency_MIBand_31M;
      } else {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_49M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_49M == 0 ? FREQ_SW_49M_END : frequency_MIBand_49M;
      }
      radio.SetFreqAM(frequency_AM);
      frequency_SW = frequency_AM;
      break;
    case SW_MI_BAND_49M:
      if (plus) {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_41M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_41M == 0 ? FREQ_SW_41M_START : frequency_MIBand_41M;
      } else {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_60M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_60M == 0 ? FREQ_SW_60M_END : frequency_MIBand_60M;
      }
      radio.SetFreqAM(frequency_AM);
      frequency_SW = frequency_AM;
      break;
    case SW_MI_BAND_60M:
      if (plus) {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_49M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_49M == 0 ? FREQ_SW_49M_START : frequency_MIBand_49M;
      } else {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_75M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_75M == 0 ? FREQ_SW_75M_END : frequency_MIBand_75M;
      }
      radio.SetFreqAM(frequency_AM);
      frequency_SW = frequency_AM;
      break;
    case SW_MI_BAND_75M:
      if (plus) {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_60M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_60M == 0 ? FREQ_SW_60M_START : frequency_MIBand_60M;
      } else {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_90M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_90M == 0 ? FREQ_SW_90M_END : frequency_MIBand_90M;
      }
      radio.SetFreqAM(frequency_AM);
      frequency_SW = frequency_AM;
      break;
    case SW_MI_BAND_90M:
      if (plus) {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_75M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_75M == 0 ? FREQ_SW_75M_START : frequency_MIBand_75M;
      } else {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_120M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_120M == 0 ? FREQ_SW_120M_END : frequency_MIBand_120M;
      }
      radio.SetFreqAM(frequency_AM);
      frequency_SW = frequency_AM;
      break;
    case SW_MI_BAND_120M:
      if (plus) {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_90M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_90M == 0 ? FREQ_SW_90M_START : frequency_MIBand_90M;
      } else {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_160M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_160M == 0 ? FREQ_SW_160M_END : frequency_MIBand_160M;
      }
      radio.SetFreqAM(frequency_AM);
      frequency_SW = frequency_AM;
      break;
    case SW_MI_BAND_160M:
      if (plus) {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_120M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_120M == 0 ? FREQ_SW_120M_START : frequency_MIBand_120M;
      } else {
        SWMIBandstringold = (String) "" + SWMIBandPosold + " m";
        SWMIBandPos = SW_MI_BAND_11M;
        SWMIBandPosold = SWMIBandPos;
        SWMIBandstring = (String) "" + SWMIBandPos + " m";
        frequency_AM = frequency_MIBand_11M == 0 ? FREQ_SW_49M_END : frequency_MIBand_11M;
      }
      radio.SetFreqAM(frequency_AM);
      frequency_SW = frequency_AM;
      break;
  }
}

void SelectBand() {
  if (band != BAND_FM) {
    seek = false;
    if (tunemode == TUNE_AUTO) tunemode = TUNE_MAN;
    BWreset = true;
    BWset = 2;
    freqold = frequency_AM;
    LimitAMFrequency();
    radio.SetFreqAM(frequency_AM);
    doBW;
    radio.getStatusAM(SStatus, USN, WAM, OStatus, BW, MStatus);
    if (screenmute == false) BuildDisplay();
    tft.setFreeFont(FONT7);
    if (region == 0) tft.drawString("PI:", 216, 191, GFXFF);
    if (region == 1) tft.drawString("ID:", 216, 191, GFXFF);
    tft.drawString("PS:", 6, 191, GFXFF);
    tft.drawString("PTY:", 6, 164, GFXFF);
    tft.drawBitmap(110, 5, RDSLogo, 67, 22, TFT_GREYOUT);
    tft.drawRoundRect(249, 56, 30, 20, 5, TFT_GREYOUT);
    tft.setTextColor(TFT_GREYOUT);
    tft.drawCentreString("iMS", 265, 54, GFXFF);
    tft.drawRoundRect(287, 56, 30, 20, 5, TFT_GREYOUT);
    tft.setTextColor(TFT_GREYOUT);
    tft.drawCentreString("EQ", 303, 54, GFXFF);
  } else {
    LowLevelInit == false;
    BWreset = true;
    BWset = 0;
    if (tunemode == TUNE_MI_BAND) tunemode = TUNE_MAN;
    radio.power(0);
    delay(50);
    radio.SetFreq(frequency);
    freqold = frequency_AM;
    doBW;
    radio.getStatus(SStatus, USN, WAM, OStatus, BW, MStatus);
    if (screenmute == false) BuildDisplay();
  }
}

void BWButtonPress() {
  if (menu == false) {
    seek = false;
    unsigned long counterold = millis();
    unsigned long counter = millis();
    while (digitalRead(BWBUTTON) == LOW && counter - counterold <= 1000) counter = millis();

    if (counter - counterold < 1000) {
      BWset++;
      doBW();
    } else {
      doStereoToggle();
    }
  }

  while (digitalRead(BWBUTTON) == LOW) delay(50);
  delay(100);
}

void doStereoToggle() {
  if (StereoToggle == true) {
    if (screenmute == false) {
      tft.drawCircle(81, 15, 10, TFT_BLACK);
      tft.drawCircle(81, 15, 9, TFT_BLACK);
      tft.drawCircle(91, 15, 10, TFT_BLACK);
      tft.drawCircle(91, 15, 9, TFT_BLACK);
      tft.drawCircle(86, 15, 10, TFT_SKYBLUE);
      tft.drawCircle(86, 15, 9, TFT_SKYBLUE);
    }
    radio.setMono(2);
    StereoToggle = false;
  } else {
    if (screenmute == false) {
      tft.drawCircle(86, 15, 10, TFT_BLACK);
      tft.drawCircle(86, 15, 9, TFT_BLACK);
    }
    radio.setMono(0);
    Stereostatusold = false;
    StereoToggle = true;
  }
}

void ModeButtonPress() {
  if (menu == false) {
    seek = false;
    unsigned long counterold = millis();
    unsigned long counter = millis();
    while (digitalRead(MODEBUTTON) == LOW && counter - counterold <= 1000) counter = millis();

    if (counter - counterold <= 1000) {
      doTuneMode();
    } else {
      if (XDRGTKUSB == true || XDRGTKTCP == true) {
        ShowFreq(1);
        tft.setFreeFont(FONT14);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setCursor (70, 60);
        tft.drawString("NOT POSSIBLE", 70, 60, GFXFF);
        delay(1000);
        tft.setTextColor(TFT_BLACK);
        tft.drawString("NOT POSSIBLE", 70, 60, GFXFF);
        ShowFreq(0);
      } else {
        if (menu == false) {
          BuildMenu();
          menu = true;
        }
      }
    }
  } else {
    OStatusold = 1000;
    Stereostatusold = false;
    SStatusold = 2000;
    BWOld = 0;
    radio.clearRDS(fullsearchrds);
    RDSstatus = 0;
    BuildDisplay();
    ShowSignalLevel();
    ShowBW();
    menu = false;
    menuopen = false;
    LowLevelInit = true;
    EEPROM.writeInt(4, VolSet);
    EEPROM.writeInt(8, ConverterSet);
    EEPROM.writeUInt(12, LowEdgeSet);
    EEPROM.writeUInt(16, HighEdgeSet);
    EEPROM.writeByte(20, ContrastSet);
    EEPROM.writeByte(21, StereoLevel);
    EEPROM.writeByte(22, HighCutLevel);
    EEPROM.writeByte(23, HighCutOffset);
    EEPROM.writeInt(24, LevelOffset);
    EEPROM.writeByte(28, edgebeep);
    EEPROM.writeByte(29, softmuteam);
    EEPROM.writeByte(30, softmutefm);
    EEPROM.writeByte(35, language);
    EEPROM.writeByte(36, showrdserrors);
    EEPROM.writeInt(47, LowLevelSet);
    EEPROM.writeByte(52, region);
    EEPROM.writeByte(53, radio.rds.underscore);
    EEPROM.writeByte(54, USBmode);
    EEPROM.writeByte(55, wifi);
    EEPROM.writeByte(56, subnetclient);
    EEPROM.writeByte(57, showSWMIBand);
    EEPROM.writeByte(58, radio.rds.filter);
    EEPROM.writeByte(59, radio.rds.dynamicpi);
    EEPROM.writeByte(244, showsquelch);
    EEPROM.writeByte(245, showmodulation);
    EEPROM.writeByte(246, amnb);
    EEPROM.writeByte(247, fmnb);
    EEPROM.writeByte(248, audiomode);
    EEPROM.commit();
    Serial.end();
    if (wifi) remoteip = IPAddress (WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], subnetclient);
    if (USBmode) Serial.begin(19200); else Serial.begin(115200);
  }
  while (digitalRead(MODEBUTTON) == LOW) delay(50);
  delay(100);
}

void ShowStepSize() {
  tft.fillRect(224, 38, 15, 4, TFT_GREYOUT);
  tft.fillRect(193, 38, 15, 4, TFT_GREYOUT);
  if (band == BAND_FM) tft.fillRect(148, 38, 15, 4, TFT_GREYOUT); else tft.fillRect(162, 38, 15, 4, TFT_GREYOUT);
  if (band == BAND_FM) tft.fillRect(116, 38, 15, 4, TFT_GREYOUT); else tft.fillRect(128, 38, 15, 4, TFT_GREYOUT);
  if (stepsize == 1) tft.fillRect(224, 38, 15, 4, TFT_GREEN);
  if (stepsize == 2) tft.fillRect(193, 38, 15, 4, TFT_GREEN);
  if (stepsize == 3) {
    if (band == BAND_FM) tft.fillRect(148, 38, 15, 4, TFT_GREEN); else tft.fillRect(162, 38, 15, 4, TFT_GREEN);
  }
  if (stepsize == 4) {
    if (band == BAND_FM) tft.fillRect(116, 38, 15, 4, TFT_GREEN); else tft.fillRect(128, 38, 15, 4, TFT_GREEN);
  }
}

void RoundStep() {
  if (band == BAND_FM) {
    unsigned int freq = frequency;
    if (freq % 10 < 3) {
      frequency = (freq - freq % 10);
    }
    else if (freq % 10 > 2 && freq % 10 < 8) {
      frequency = (freq - (freq % 10 - 5));
    }
    else if (freq % 10 > 7) {
      frequency = (freq - (freq % 10) + 10);
    }
    radio.SetFreq(frequency);
  }
  if (band == BAND_MW) {
    unsigned int freq = frequency_AM / (region == 0 ? FREQ_MW_STEP_9K : FREQ_MW_STEP_10K);
    frequency_AM = freq * (region == 0 ? FREQ_MW_STEP_9K : FREQ_MW_STEP_10K);
    radio.SetFreqAM(frequency_AM);
  }

  while (digitalRead(ROTARY_BUTTON) == LOW) delay(50);

  if (band == BAND_FM) EEPROM.writeUInt(0, frequency); else EEPROM.writeUInt(31, frequency_AM);
  EEPROM.commit();
}

void ButtonPress() {
  if (menu == false) {
    if (tunemode == TUNE_MEM) {
      if (memorystore == false) {
        memorystore = true;
        ShowTuneMode();
      } else {
        memorystore = false;
        EEPROM.writeByte(memorypos + 60, band);
        if (band == BAND_FM) EEPROM.writeUInt((memorypos * 4) + 100, frequency); else EEPROM.writeUInt((memorypos * 4) + 100, frequency_AM);
        EEPROM.commit();
        memoryband[memorypos] = band;
        if (band == BAND_FM) memory[memorypos] = frequency; else memory[memorypos] = frequency_AM;
        ShowTuneMode();
      }
    } else {
      seek = false;
      unsigned long counterold = millis();
      unsigned long counter = millis();
      while (digitalRead(ROTARY_BUTTON) == LOW && counter - counterold <= 1000) counter = millis();

      if (counter - counterold < 1000) {
        if (tunemode == TUNE_MAN) {
          stepsize++;
          if (band == BAND_SW || band == BAND_FM) {
            if (stepsize > 4) stepsize = 0;
          }
          else {
            if (stepsize > 3) stepsize = 0;
          }

          if (screenmute == false) ShowStepSize();

          EEPROM.writeByte(40, stepsize);
          EEPROM.commit();
          if (stepsize == 0) {
            RoundStep();
            ShowFreq(0);
          }
        }
      } else {
        if (iMSEQ == 0) iMSEQ = 1;

        if (iMSEQ == 4) {
          iMSset = 0;
          EQset = 0;
          updateiMS();
          updateEQ();
          iMSEQ = 0;
        }
        if (iMSEQ == 3) {
          iMSset = 1;
          EQset = 0;
          updateiMS();
          updateEQ();
          iMSEQ = 4;
        }
        if (iMSEQ == 2) {
          iMSset = 0;
          EQset = 1;
          updateiMS();
          updateEQ();
          iMSEQ = 3;
        }
        if (iMSEQ == 1) {
          iMSset = 1;
          EQset = 1;
          updateiMS();
          updateEQ();
          iMSEQ = 2;
        }
        EEPROM.writeByte(44, iMSset);
        EEPROM.writeByte(45, EQset);
        EEPROM.commit();
      }
    }
  } else {
    if (menuopen == false) {
      menuopen = true;
      tft.drawRoundRect(20, 30, 274, 170, 5, TFT_WHITE);
      tft.fillRoundRect(22, 32, 270, 166, 5, TFT_BLACK);
      tft.setFreeFont(FONT14);
      switch (menupage) {
        case 1:
          switch (menuoption) {
            case 30:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][10], 155, 70, GFXFF);
              tft.drawString("dB", 170, 110, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (VolSet > 0) tft.drawRightString("+" + String(VolSet, DEC), 155, 110, GFXFF); else tft.drawRightString(String(VolSet, DEC), 155, 110, GFXFF);
              break;

            case 50:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][11], 155, 70, GFXFF);
              tft.drawString("MHz", 170, 110, GFXFF);
              tft.setTextColor(TFT_YELLOW);

              tft.drawRightString(String(ConverterSet, DEC), 155, 110, GFXFF);
              break;

            case 70:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][12], 155, 70, GFXFF);
              tft.drawString("MHz", 170, 110, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              tft.drawRightString(String(LowEdgeSet / 10 + ConverterSet, DEC) + "." + String(LowEdgeSet % 10 + ConverterSet, DEC), 155, 110, GFXFF);
              break;

            case 90:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][13], 155, 70, GFXFF);
              tft.drawString("MHz", 170, 110, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              tft.drawRightString(String(HighEdgeSet / 10 + ConverterSet, DEC) + "." + String(HighEdgeSet % 10 + ConverterSet, DEC), 155, 110, GFXFF);
              break;

            case 110:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][14], 155, 70, GFXFF);
              tft.drawString("dB", 170, 110, GFXFF);
              tft.drawString("dBμV", 190, 157, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (LevelOffset > 0) tft.drawRightString("+" + String(LevelOffset, DEC), 155, 110, GFXFF); else tft.drawRightString(String(LevelOffset, DEC), 155, 110, GFXFF);
              SStatusold = 2000;
              change2 = true;
              break;

            case 130:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][15], 155, 70, GFXFF);
              if (StereoLevel != 0) tft.drawString("dBμV", 170, 110, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (StereoLevel != 0) tft.drawRightString(String(StereoLevel, DEC), 155, 110, GFXFF); else tft.drawRightString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 150:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][16], 155, 70, GFXFF);
              if (HighCutLevel != 0) tft.drawString("Hz", 170, 110, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (HighCutLevel != 0) tft.drawRightString(String(HighCutLevel * 100, DEC), 155, 110, GFXFF); else tft.drawRightString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 170:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][17], 155, 70, GFXFF);
              if (HighCutOffset != 0) tft.drawString("dBμV", 170, 110, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (HighCutOffset != 0) tft.drawRightString(String(HighCutOffset, DEC), 155, 110, GFXFF); else tft.drawRightString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 190:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][18], 155, 70, GFXFF);
              tft.drawString("dBμV", 155, 110, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              tft.drawRightString(String(LowLevelSet, DEC), 145, 110, GFXFF);
              break;

            case 210:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][19], 155, 70, GFXFF);
              tft.drawString("%", 170, 110, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              tft.drawRightString(String(ContrastSet, DEC), 155, 110, GFXFF);
              break;
          }
          break;

        case 2:
          switch (menuoption) {
            case 30:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][40], 155, 70, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              tft.drawCentreString(myLanguage[language][0], 155, 110, GFXFF);
              break;

            case 50:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][38], 155, 70, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (showrdserrors) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 70:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][43], 155, 70, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (softmutefm) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 90:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][44], 155, 70, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (softmuteam) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 110:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][45], 155, 70, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (edgebeep) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 130:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][46], 155, 70, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (region == 0) tft.drawCentreString(myLanguage[language][47], 155, 110, GFXFF);
              if (region == 1) tft.drawCentreString(myLanguage[language][48], 155, 110, GFXFF);
              break;

            case 150:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][49], 155, 70, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (radio.rds.underscore) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 170:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][50], 155, 70, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (USBmode) tft.drawCentreString("RDS Spy", 155, 110, GFXFF); else tft.drawCentreString("XDR-GTK", 155, 110, GFXFF);
              break;

            case 190:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][51], 155, 70, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (wifi) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 210:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][53], 155, 50, GFXFF);
              tft.drawCentreString("ESP_" + String(ESP_getChipId()), 155, 90, GFXFF);
              tft.drawCentreString(myLanguage[language][54], 155, 130, GFXFF);
              tft.setFreeFont(FONT7);
              tft.drawCentreString("http://192.168.4.1", 155, 170, GFXFF);
              char key [9];
              XDRGTK_key.toCharArray(key, 9);
              WiFiConnectParam XDRGTK_key_text("Set XDRGTK Password: (max 8 characters)");
              WiFiConnectParam XDRGTK_key_input("XDRGTK_key", "Password", key, 9);
              wc.addParameter(&XDRGTK_key_text);
              wc.addParameter(&XDRGTK_key_input);
              wc.startConfigurationPortal(AP_WAIT);
              XDRGTK_key = XDRGTK_key_input.getValue();
              EEPROM.writeString(233, XDRGTK_key);
              EEPROM.commit();
              tryWiFi();
              delay(2000);
              BuildMenu();
              break;
          }
          break;

        case 3:
          switch (menuoption) {
            case 30:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][58], 155, 70, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              tft.drawCentreString(String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + "." + String(WiFi.localIP()[2]) + "." + String(subnetclient, DEC), 155, 110, GFXFF);
              break;

            case 50:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][59], 155, 70, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (showSWMIBand) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 70:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][60], 155, 70, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (radio.rds.filter) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 90:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][61], 155, 70, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (radio.rds.dynamicpi) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 110:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][62], 155, 70, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (showsquelch) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 130:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][63], 155, 70, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (showmodulation) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 150:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][64], 155, 70, GFXFF);
              if (amnb != 0) tft.drawString("%", 170, 110, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (amnb != 0) tft.drawRightString(String(amnb, DEC), 155, 110, GFXFF); else tft.drawRightString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 170:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][65], 155, 70, GFXFF);
              if (fmnb != 0) tft.drawString("%", 170, 110, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (fmnb != 0) tft.drawRightString(String(fmnb, DEC), 155, 110, GFXFF); else tft.drawRightString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 190:
              tft.setTextColor(TFT_WHITE);
              tft.drawCentreString(myLanguage[language][67], 155, 70, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (audiomode) tft.drawCentreString("MPX", 155, 110, GFXFF); else tft.drawCentreString("Stereo", 155, 110, GFXFF);
              break;
          }
      }
    } else {
      if (menupage == 2 && menuoption == 190 && wifi == true) {
        tryWiFi();
        delay(2000);
      }
      menuopen = false;
      BuildMenu();
    }
  }
  while (digitalRead(ROTARY_BUTTON) == LOW) delay(50);
}

void KeyUp() {
  rotary = 0;
  if (menu == false) {
    switch (tunemode) {
      case TUNE_MAN:
        TuneUp();
        break;

      case TUNE_AUTO:
        direction = true;
        seek = true;
        Seek(direction);
        break;

      case TUNE_MEM:
        memorypos++;
        if (memorypos > 29) memorypos = 0;
        ShowMemoryPos();
        if (memorystore == false) DoMemoryPosTune();
        EEPROM.writeByte(51, memorypos);
        EEPROM.commit();
        break;
      case TUNE_MI_BAND:
        if (showSWMIBand) {
          if (displayflip) {
            ToggleSWMIBand(false);
          } else {
            ToggleSWMIBand(true);
          }
        }
        break;
    }
      if (XDRGTKUSB == true || XDRGTKTCP == true) if (band == BAND_FM) DataPrint("T" + String(frequency * 10)); else DataPrint("T" + String(frequency_AM));
    radio.clearRDS(fullsearchrds);
    change = 0;
    ShowFreq(0);
    store = true;
  } else {
    if (menuopen == false) {
      tft.drawRoundRect(8, menuoption + 3, 302, 19, 5, TFT_BLACK);
      menuoption += 20;
      if (menuoption > 210) {
        menupage++;
        if (menupage > menupagestotal) menupage = 1;
        menuoption = 30;
        BuildMenu();
      }
      tft.drawRoundRect(8, menuoption + 3, 302, 19, 5, TFT_WHITE);
    } else {
      switch (menupage) {
        case 1:
          switch (menuoption) {
            case 30:
              tft.setTextColor(TFT_BLACK);
              if (VolSet > 0) tft.drawRightString("+" + String(VolSet, DEC), 155, 110, GFXFF); else tft.drawRightString(String(VolSet, DEC), 155, 110, GFXFF);
              VolSet++;
              if (VolSet > 10) VolSet = 10;
              tft.setTextColor(TFT_YELLOW);
              if (VolSet > 0) tft.drawRightString("+" + String(VolSet, DEC), 155, 110, GFXFF); else tft.drawRightString(String(VolSet, DEC), 155, 110, GFXFF);
              radio.setVolume(VolSet);
              break;

            case 50:
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(String(ConverterSet, DEC), 155, 110, GFXFF);
              ConverterSet++;
                if (ConverterSet > 2400 || ConverterSet <= 200) if (ConverterSet == 1) ConverterSet = 200; else ConverterSet = 0;
              if (ConverterSet >= 200) {
                Wire.beginTransmission(0x12);
                Wire.write(ConverterSet >> 8);
                Wire.write(ConverterSet & (0xFF));
                Wire.endTransmission();
              }
              tft.setTextColor(TFT_YELLOW);
              tft.drawRightString(String(ConverterSet, DEC), 155, 110, GFXFF);
              break;

            case 70:
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(String(LowEdgeSet / 10 + ConverterSet, DEC) + "." + String(LowEdgeSet % 10 + ConverterSet, DEC), 155, 110, GFXFF);
              LowEdgeSet ++;
              if (LowEdgeSet > 1070) LowEdgeSet = 650;
              tft.setTextColor(TFT_YELLOW);
              tft.drawRightString(String(LowEdgeSet / 10 + ConverterSet, DEC) + "." + String(LowEdgeSet % 10 + ConverterSet, DEC), 155, 110, GFXFF);
              break;

            case 90:
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(String(HighEdgeSet / 10 + ConverterSet, DEC) + "." + String(HighEdgeSet % 10 + ConverterSet, DEC), 155, 110, GFXFF);
              HighEdgeSet ++;
              if (HighEdgeSet > 1080) HighEdgeSet = 660;
              tft.setTextColor(TFT_YELLOW);
              tft.drawRightString(String(HighEdgeSet / 10 + ConverterSet, DEC) + "." + String(HighEdgeSet % 10 + ConverterSet, DEC), 155, 110, GFXFF);
              break;

            case 110:
              tft.setFreeFont(FONT14);
              tft.setTextColor(TFT_BLACK);
              if (LevelOffset > 0) tft.drawRightString("+" + String(LevelOffset, DEC), 155, 110, GFXFF); else tft.drawRightString(String(LevelOffset, DEC), 155, 110, GFXFF);
              LevelOffset++;
              if (LevelOffset > 15) LevelOffset = -25;
              tft.setTextColor(TFT_YELLOW);
              if (LevelOffset > 0) tft.drawRightString("+" + String(LevelOffset, DEC), 155, 110, GFXFF); else tft.drawRightString(String(LevelOffset, DEC), 155, 110, GFXFF);
              radio.setOffset(LevelOffset);
              change2 = true;
              break;

            case 130:
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(String(StereoLevel, DEC), 155, 110, GFXFF);
              StereoLevel ++;
                if (StereoLevel > 60 || StereoLevel <= 30) if (StereoLevel == 1) StereoLevel = 30; else StereoLevel = 0;
              tft.drawRightString(myLanguage[language][30], 155, 110, GFXFF);
              tft.drawString("dBμV", 170, 110, GFXFF);
              tft.setTextColor(TFT_WHITE);
              if (StereoLevel != 0) tft.drawString("dBμV", 170, 110, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (StereoLevel != 0) tft.drawRightString(String(StereoLevel, DEC), 155, 110, GFXFF); else tft.drawRightString(myLanguage[language][30], 155, 110, GFXFF);
              radio.setStereoLevel(StereoLevel);
              break;

            case 150:
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(String(HighCutLevel * 100, DEC), 155, 110, GFXFF);
              HighCutLevel ++;
              if (HighCutLevel > 70) HighCutLevel = 15;
              tft.setTextColor(TFT_YELLOW);
              tft.drawRightString(String(HighCutLevel * 100, DEC), 155, 110, GFXFF);
              radio.setHighCutLevel(HighCutLevel);
              break;

            case 170:
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(String(HighCutOffset, DEC), 155, 110, GFXFF);
              HighCutOffset ++;
                if (HighCutOffset > 60 || HighCutOffset <= 20) if (HighCutOffset == 1) HighCutOffset = 20; else HighCutOffset = 0;
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(myLanguage[language][30], 155, 110, GFXFF);
              tft.drawString("dBμV", 170, 110, GFXFF);
              tft.setTextColor(TFT_WHITE);
              if (HighCutOffset != 0) tft.drawString("dBμV", 170, 110, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (HighCutOffset != 0) tft.drawRightString(String(HighCutOffset, DEC), 155, 110, GFXFF); else tft.drawRightString(myLanguage[language][30], 155, 110, GFXFF);
              radio.setHighCutOffset(HighCutOffset);
              break;

            case 190:
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(String(LowLevelSet, DEC), 145, 110, GFXFF);
              LowLevelSet++;
              if (LowLevelSet > 40) LowLevelSet = -10;
              tft.setTextColor(TFT_YELLOW);
              tft.drawRightString(String(LowLevelSet, DEC), 145, 110, GFXFF);
              break;

            case 210:
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(String(ContrastSet, DEC), 155, 110, GFXFF);
              ContrastSet ++;
              if (ContrastSet > 100) ContrastSet = 1;
              tft.setTextColor(TFT_YELLOW);
              tft.drawRightString(String(ContrastSet, DEC), 155, 110, GFXFF);
              analogWrite(CONTRASTPIN, ContrastSet * 2 + 27);
              break;
          }
          break;

        case 2:
          switch (menuoption) {
            case 30:
              tft.setTextColor(TFT_BLACK);
              tft.drawCentreString(myLanguage[language][0], 155, 110, GFXFF);
              language ++;
              if (language == (sizeof (myLanguage) / sizeof (myLanguage[0]))) language = 0;
              tft.setTextColor(TFT_YELLOW);
              tft.drawCentreString(myLanguage[language][0], 155, 110, GFXFF);
              break;

            case 50:
              tft.setTextColor(TFT_BLACK);
              if (showrdserrors) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (showrdserrors) showrdserrors = false; else showrdserrors = true;
              tft.setTextColor(TFT_YELLOW);
              if (showrdserrors) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 70:
              tft.setTextColor(TFT_BLACK);
              if (softmutefm) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (softmutefm) softmutefm = false; else softmutefm = true;
              tft.setTextColor(TFT_YELLOW);
              if (softmutefm) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              radio.setSoftmuteFM(softmutefm);
              break;

            case 90:
              tft.setTextColor(TFT_BLACK);
              if (softmuteam) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (softmuteam) softmuteam = false; else softmuteam = true;
              tft.setTextColor(TFT_YELLOW);
              if (softmuteam) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              radio.setSoftmuteAM(softmuteam);
              break;

            case 110:
              tft.setTextColor(TFT_BLACK);
              if (edgebeep) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (edgebeep) edgebeep = false; else edgebeep = true;
              tft.setTextColor(TFT_YELLOW);
              if (edgebeep) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 130:
              tft.setTextColor(TFT_BLACK);
              if (region == 0) region = 1; else region = 0;
              if (regionold == 0) tft.drawCentreString(myLanguage[language][47], 155, 110, GFXFF);
              if (regionold == 1) tft.drawCentreString(myLanguage[language][48], 155, 110, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (region == 0) tft.drawCentreString(myLanguage[language][47], 155, 110, GFXFF);
              if (region == 1) tft.drawCentreString(myLanguage[language][48], 155, 110, GFXFF);
              regionold = region;
              if (region == 0) radio.setDeemphasis(1);
              if (region == 1) radio.setDeemphasis(2);
              radio.rds.region = region;
              break;

            case 150:
              tft.setTextColor(TFT_BLACK);
              if (radio.rds.underscore) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (radio.rds.underscore) radio.rds.underscore = false; else radio.rds.underscore = true;
              tft.setTextColor(TFT_YELLOW);
              if (radio.rds.underscore) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 170:
              tft.setTextColor(TFT_BLACK);
              if (USBmode) tft.drawCentreString("RDS Spy", 155, 110, GFXFF); else tft.drawCentreString("XDR-GTK", 155, 110, GFXFF);
              if (USBmode) USBmode = false; else USBmode = true;
              tft.setTextColor(TFT_YELLOW);
              if (USBmode) tft.drawCentreString("RDS Spy", 155, 110, GFXFF); else tft.drawCentreString("XDR-GTK", 155, 110, GFXFF);
              break;

            case 190:
              tft.setTextColor(TFT_BLACK);
              if (wifi) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (wifi) wifi = false; else wifi = true;
              tft.setTextColor(TFT_YELLOW);
              if (wifi) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;
          }
          break;

        case 3:
          switch (menuoption) {
            case 30:
              tft.setTextColor(TFT_BLACK);
              tft.drawCentreString(String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + "." + String(WiFi.localIP()[2]) + "." + String(subnetclient, DEC), 155, 110, GFXFF);
              subnetclient ++;
              if (subnetclient > 254) subnetclient = 1;
              tft.setTextColor(TFT_YELLOW);
              tft.drawCentreString(String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + "." + String(WiFi.localIP()[2]) + "." + String(subnetclient, DEC), 155, 110, GFXFF);
              break;

            case 50:
              tft.setTextColor(TFT_BLACK);
              if (showSWMIBand) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (showSWMIBand) showSWMIBand = false; else showSWMIBand = true;
              tft.setTextColor(TFT_YELLOW);
              if (showSWMIBand) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 70:
              tft.setTextColor(TFT_BLACK);
              if (radio.rds.filter) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (radio.rds.filter) radio.rds.filter = false; else radio.rds.filter = true;
              tft.setTextColor(TFT_YELLOW);
              if (radio.rds.filter) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 90:
              tft.setTextColor(TFT_BLACK);
              if (radio.rds.dynamicpi) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (radio.rds.dynamicpi) radio.rds.dynamicpi = false; else radio.rds.dynamicpi = true;
              tft.setTextColor(TFT_YELLOW);
              if (radio.rds.dynamicpi) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 110:
              tft.setTextColor(TFT_BLACK);
              if (showsquelch) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (showsquelch) showsquelch = false; else showsquelch = true;
              tft.setTextColor(TFT_YELLOW);
              if (showsquelch) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 130:
              tft.setTextColor(TFT_BLACK);
              if (showmodulation) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (showmodulation) showmodulation = false; else showmodulation = true;
              tft.setTextColor(TFT_YELLOW);
              if (showmodulation) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 150:
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(String(amnb, DEC), 155, 110, GFXFF);
              amnb ++;
                if (amnb > 150 || amnb <= 50) if (amnb == 1) amnb = 50; else amnb = 0;
              tft.drawRightString(myLanguage[language][30], 155, 110, GFXFF);
              tft.drawString("%", 170, 110, GFXFF);
              tft.setTextColor(TFT_WHITE);
              if (amnb != 0) tft.drawString("%", 170, 110, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (amnb != 0) tft.drawRightString(String(amnb, DEC), 155, 110, GFXFF); else tft.drawRightString(myLanguage[language][30], 155, 110, GFXFF);
              radio.setAMNoiseBlanker(amnb);
              break;

            case 170:
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(String(fmnb, DEC), 155, 110, GFXFF);
              fmnb ++;
                if (fmnb > 150 || fmnb <= 50) if (fmnb == 1) fmnb = 50; else fmnb = 0;
              tft.drawRightString(myLanguage[language][30], 155, 110, GFXFF);
              tft.drawString("%", 170, 110, GFXFF);
              tft.setTextColor(TFT_WHITE);
              if (fmnb != 0) tft.drawString("%", 170, 110, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (fmnb != 0) tft.drawRightString(String(fmnb, DEC), 155, 110, GFXFF); else tft.drawRightString(myLanguage[language][30], 155, 110, GFXFF);
              radio.setFMNoiseBlanker(fmnb);
              break;

            case 190:
              tft.setTextColor(TFT_BLACK);
              if (audiomode) tft.drawCentreString("MPX", 155, 110, GFXFF); else tft.drawCentreString("Stereo", 155, 110, GFXFF);
              if (audiomode) audiomode = false; else audiomode = true;
              tft.setTextColor(TFT_YELLOW);
              if (audiomode) tft.drawCentreString("MPX", 155, 110, GFXFF); else tft.drawCentreString("Stereo", 155, 110, GFXFF);
              radio.setAudio(audiomode);
              break;
          }
      }
    }
  }
}

void KeyDown() {
  rotary = 0;
  if (menu == false) {
    switch (tunemode) {
      case TUNE_MAN:
        TuneDown();
        break;

      case TUNE_AUTO:
        direction = false;
        seek = true;
        Seek(direction);
        break;

      case TUNE_MEM:
        memorypos--;
        if (memorypos > 29) memorypos = 29;
        ShowMemoryPos();
        if (memorystore == false) DoMemoryPosTune();
        EEPROM.writeByte(51, memorypos);
        EEPROM.commit();
        break;
      case TUNE_MI_BAND:
        if (showSWMIBand) {
          if (displayflip) {
            ToggleSWMIBand(true);
          } else {
            ToggleSWMIBand(false);
          }
        }
        break;
    }
      if (XDRGTKUSB == true || XDRGTKTCP == true) if (band == BAND_FM) DataPrint("T" + String(frequency * 10)); else DataPrint("T" + String(frequency_AM));
    radio.clearRDS(fullsearchrds);
    change = 0;
    ShowFreq(0);
    store = true;
  } else {
    if (menuopen == false) {
      tft.drawRoundRect(8, menuoption + 3, 302, 19, 5, TFT_BLACK);
      menuoption -= 20;
      if (menuoption < 30) {
        menupage--;
        if (menupage == 0) menupage = menupagestotal;
        menuoption = 210;
        BuildMenu();
      }
      tft.drawRoundRect(8, menuoption + 3, 302, 19, 5, TFT_WHITE);
    } else {
      switch (menupage) {
        case 1:
          switch (menuoption) {
            case 30:
              tft.setTextColor(TFT_BLACK);
              if (VolSet > 0) tft.drawRightString("+" + String(VolSet, DEC), 155, 110, GFXFF); else tft.drawRightString(String(VolSet, DEC), 155, 110, GFXFF);
              VolSet--;
              if (VolSet < -10) VolSet = -10;
              tft.setTextColor(TFT_YELLOW);
              if (VolSet > 0) tft.drawRightString("+" + String(VolSet, DEC), 155, 110, GFXFF); else tft.drawRightString(String(VolSet, DEC), 155, 110, GFXFF);
              radio.setVolume(VolSet);
              break;

            case 50:
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(String(ConverterSet, DEC), 155, 110, GFXFF);
              ConverterSet--;
                if (ConverterSet < 200 || ConverterSet > 2400) if (ConverterSet > 2400) ConverterSet = 2400; else ConverterSet = 0;
              if (ConverterSet >= 200) {
                Wire.beginTransmission(0x12);
                Wire.write(ConverterSet >> 8);
                Wire.write(ConverterSet & (0xFF));
                Wire.endTransmission();
              }
              tft.setTextColor(TFT_YELLOW);
              tft.drawRightString(String(ConverterSet, DEC), 155, 110, GFXFF);
              break;

            case 70:
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(String(LowEdgeSet / 10 + ConverterSet, DEC) + "." + String(LowEdgeSet % 10 + ConverterSet, DEC), 155, 110, GFXFF);
              LowEdgeSet -= 10;
              if (LowEdgeSet < 650) LowEdgeSet = 1079;
              tft.setTextColor(TFT_YELLOW);
              tft.drawRightString(String(LowEdgeSet / 10 + ConverterSet, DEC) + "." + String(LowEdgeSet % 10 + ConverterSet, DEC), 155, 110, GFXFF);
              break;

            case 90:
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(String(HighEdgeSet / 10 + ConverterSet, DEC) + "." + String(HighEdgeSet % 10 + ConverterSet, DEC), 155, 110, GFXFF);
              HighEdgeSet -= 10;
              if (HighEdgeSet < 660) HighEdgeSet = 1080;
              tft.setTextColor(TFT_YELLOW);
              tft.drawRightString(String(HighEdgeSet / 10 + ConverterSet, DEC) + "." + String(HighEdgeSet % 10 + ConverterSet, DEC), 155, 110, GFXFF);
              break;

            case 110:
              tft.setFreeFont(FONT14);
              tft.setTextColor(TFT_BLACK);
              if (LevelOffset > 0) tft.drawRightString("+" + String(LevelOffset, DEC), 155, 110, GFXFF); else tft.drawRightString(String(LevelOffset, DEC), 155, 110, GFXFF);
              LevelOffset--;
              if (LevelOffset < -25) LevelOffset = 15;
              tft.setTextColor(TFT_YELLOW);
              if (LevelOffset > 0) tft.drawRightString("+" + String(LevelOffset, DEC), 155, 110, GFXFF); else tft.drawRightString(String(LevelOffset, DEC), 155, 110, GFXFF);
              radio.setOffset(LevelOffset);
              change2 = true;
              break;

            case 130:
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(String(StereoLevel, DEC), 155, 110, GFXFF);
              StereoLevel --;
                if (StereoLevel < 30 || StereoLevel > 60) if (StereoLevel > 60) StereoLevel = 60; else StereoLevel = 0;
              tft.drawRightString(myLanguage[language][30], 155, 110, GFXFF);
              tft.drawString("dBμV", 170, 110, GFXFF);
              tft.setTextColor(TFT_WHITE);
              if (StereoLevel != 0) tft.drawString("dBμV", 170, 110, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (StereoLevel != 0) tft.drawRightString(String(StereoLevel, DEC), 155, 110, GFXFF); else tft.drawRightString(myLanguage[language][30], 155, 110, GFXFF);
              radio.setStereoLevel(StereoLevel);
              break;

            case 150:
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(String(HighCutLevel * 100, DEC), 155, 110, GFXFF);
              HighCutLevel --;
              if (HighCutLevel < 15 || HighCutLevel > 70) HighCutLevel = 70;
              tft.setTextColor(TFT_YELLOW);
              tft.drawRightString(String(HighCutLevel * 100, DEC), 155, 110, GFXFF);
              radio.setHighCutLevel(HighCutLevel);
              break;

            case 170:
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(String(HighCutOffset, DEC), 155, 110, GFXFF);
              HighCutOffset --;
                if (HighCutOffset < 20 || HighCutOffset > 60) if (HighCutOffset > 60) HighCutOffset = 60; else HighCutOffset = 0;
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(myLanguage[language][30], 155, 110, GFXFF);
              tft.drawString("dBμV", 170, 110, GFXFF);
              tft.setTextColor(TFT_WHITE);
              if (HighCutOffset != 0) tft.drawString("dBμV", 170, 110, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (HighCutOffset != 0) tft.drawRightString(String(HighCutOffset, DEC), 155, 110, GFXFF); else tft.drawRightString(myLanguage[language][30], 155, 110, GFXFF);
              radio.setHighCutOffset(HighCutOffset);
              break;

            case 190:
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(String(LowLevelSet, DEC), 145, 110, GFXFF);
              LowLevelSet--;
              if (LowLevelSet < -10) LowLevelSet = 40;
              tft.setTextColor(TFT_YELLOW);
              tft.drawRightString(String(LowLevelSet, DEC), 145, 110, GFXFF);
              break;


            case 210:
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(String(ContrastSet, DEC), 155, 110, GFXFF);
              ContrastSet --;
              if (ContrastSet < 1) ContrastSet = 100;
              tft.setTextColor(TFT_YELLOW);
              tft.drawRightString(String(ContrastSet, DEC), 155, 110, GFXFF);
              analogWrite(CONTRASTPIN, ContrastSet * 2 + 27);
              break;
          }
          break;

        case 2:
          switch (menuoption) {
            case 30:
              tft.setTextColor(TFT_BLACK);
              tft.drawCentreString(myLanguage[language][0], 155, 110, GFXFF);
              language --;
              if (language > (sizeof (myLanguage) / sizeof (myLanguage[0]))) language = (sizeof (myLanguage) / sizeof (myLanguage[0])) - 1;
              tft.setTextColor(TFT_YELLOW);
              tft.drawCentreString(myLanguage[language][0], 155, 110, GFXFF);
              break;

            case 50:
              tft.setTextColor(TFT_BLACK);
              if (showrdserrors) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (showrdserrors) showrdserrors = false; else showrdserrors = true;
              tft.setTextColor(TFT_YELLOW);
              if (showrdserrors) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 70:
              tft.setTextColor(TFT_BLACK);
              if (softmutefm) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (softmutefm) softmutefm = false; else softmutefm = true;
              tft.setTextColor(TFT_YELLOW);
              if (softmutefm) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              radio.setSoftmuteFM(softmutefm);
              break;

            case 90:
              tft.setTextColor(TFT_BLACK);
              if (softmuteam) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (softmuteam) softmuteam = false; else softmuteam = true;
              tft.setTextColor(TFT_YELLOW);
              if (softmuteam) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              radio.setSoftmuteAM(softmuteam);
              break;

            case 110:
              tft.setTextColor(TFT_BLACK);
              if (edgebeep) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (edgebeep) edgebeep = false; else edgebeep = true;
              tft.setTextColor(TFT_YELLOW);
              if (edgebeep) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 130:
              tft.setTextColor(TFT_BLACK);
              if (region == 0) region = 1; else region = 0;
              if (regionold == 0) tft.drawCentreString(myLanguage[language][47], 155, 110, GFXFF);
              if (regionold == 1) tft.drawCentreString(myLanguage[language][48], 155, 110, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (region == 0) tft.drawCentreString(myLanguage[language][47], 155, 110, GFXFF);
              if (region == 1) tft.drawCentreString(myLanguage[language][48], 155, 110, GFXFF);
              regionold = region;
              if (region == 0) radio.setDeemphasis(1);
              if (region == 1) radio.setDeemphasis(2);
              radio.rds.region = region;
              break;

            case 150:
              tft.setTextColor(TFT_BLACK);
              if (radio.rds.underscore) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (radio.rds.underscore) radio.rds.underscore = false; else radio.rds.underscore = true;
              tft.setTextColor(TFT_YELLOW);
              if (radio.rds.underscore) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 170:
              tft.setTextColor(TFT_BLACK);
              if (USBmode) tft.drawCentreString("RDS Spy", 155, 110, GFXFF); else tft.drawCentreString("XDR-GTK", 155, 110, GFXFF);
              if (USBmode) USBmode = false; else USBmode = true;
              tft.setTextColor(TFT_YELLOW);
              if (USBmode) tft.drawCentreString("RDS Spy", 155, 110, GFXFF); else tft.drawCentreString("XDR-GTK", 155, 110, GFXFF);
              break;

            case 190:
              tft.setTextColor(TFT_BLACK);
              if (wifi) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (wifi) wifi = false; else wifi = true;
              tft.setTextColor(TFT_YELLOW);
              if (wifi) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;
          }
          break;

        case 3:
          switch (menuoption) {
            case 30:
              tft.setTextColor(TFT_BLACK);
              tft.drawCentreString(String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + "." + String(WiFi.localIP()[2]) + "." + String(subnetclient, DEC), 155, 110, GFXFF);
              subnetclient --;
              if (subnetclient < 1) subnetclient = 254;
              tft.setTextColor(TFT_YELLOW);
              tft.drawCentreString(String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + "." + String(WiFi.localIP()[2]) + "." + String(subnetclient, DEC), 155, 110, GFXFF);
              break;

            case 50:
              tft.setTextColor(TFT_BLACK);
              if (showSWMIBand) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (showSWMIBand) showSWMIBand = false; else showSWMIBand = true;
              tft.setTextColor(TFT_YELLOW);
              if (showSWMIBand) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 70:
              tft.setTextColor(TFT_BLACK);
              if (radio.rds.filter) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (radio.rds.filter) radio.rds.filter = false; else radio.rds.filter = true;
              tft.setTextColor(TFT_YELLOW);
              if (radio.rds.filter) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 90:
              tft.setTextColor(TFT_BLACK);
              if (radio.rds.dynamicpi) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (radio.rds.dynamicpi) radio.rds.dynamicpi = false; else radio.rds.dynamicpi = true;
              tft.setTextColor(TFT_YELLOW);
              if (radio.rds.dynamicpi) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 110:
              tft.setTextColor(TFT_BLACK);
              if (showsquelch) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (showsquelch) showsquelch = false; else showsquelch = true;
              tft.setTextColor(TFT_YELLOW);
              if (showsquelch) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 130:
              tft.setTextColor(TFT_BLACK);
              if (showmodulation) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              if (showmodulation) showmodulation = false; else showmodulation = true;
              tft.setTextColor(TFT_YELLOW);
              if (showmodulation) tft.drawCentreString(myLanguage[language][42], 155, 110, GFXFF); else tft.drawCentreString(myLanguage[language][30], 155, 110, GFXFF);
              break;

            case 150:
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(String(amnb, DEC), 155, 110, GFXFF);
              amnb --;
                if (amnb > 150 || amnb <= 50) if (amnb > 150) amnb = 150; else amnb = 0;
              tft.drawRightString(myLanguage[language][30], 155, 110, GFXFF);
              tft.drawString("%", 170, 110, GFXFF);
              tft.setTextColor(TFT_WHITE);
              if (amnb != 0) tft.drawString("%", 170, 110, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (amnb != 0) tft.drawRightString(String(amnb, DEC), 155, 110, GFXFF); else tft.drawRightString(myLanguage[language][30], 155, 110, GFXFF);
              radio.setAMNoiseBlanker(amnb);
              break;

            case 170:
              tft.setTextColor(TFT_BLACK);
              tft.drawRightString(String(fmnb, DEC), 155, 110, GFXFF);
              fmnb --;
                if (fmnb > 150 || fmnb <= 50) if (fmnb > 150) fmnb = 150; else fmnb = 0;
              tft.drawRightString(myLanguage[language][30], 155, 110, GFXFF);
              tft.drawString("%", 170, 110, GFXFF);
              tft.setTextColor(TFT_WHITE);
              if (fmnb != 0) tft.drawString("%", 170, 110, GFXFF);
              tft.setTextColor(TFT_YELLOW);
              if (fmnb != 0) tft.drawRightString(String(fmnb, DEC), 155, 110, GFXFF); else tft.drawRightString(myLanguage[language][30], 155, 110, GFXFF);
              radio.setFMNoiseBlanker(fmnb);
              break;

            case 190:
              tft.setTextColor(TFT_BLACK);
              if (audiomode) tft.drawCentreString("MPX", 155, 110, GFXFF); else tft.drawCentreString("Stereo", 155, 110, GFXFF);
              if (audiomode) audiomode = false; else audiomode = true;
              tft.setTextColor(TFT_YELLOW);
              if (audiomode) tft.drawCentreString("MPX", 155, 110, GFXFF); else tft.drawCentreString("Stereo", 155, 110, GFXFF);
              radio.setAudio(audiomode);
              break;
          }
      }
    }
  }
}

void ShowMemoryPos() {
  tft.setFreeFont(FONT7);
  tft.setTextColor(TFT_BLACK);
  tft.drawString(String(memoryposold + 1), 80, 26, GFXFF);
  tft.setTextColor(TFT_SKYBLUE);
  tft.drawString(String(memorypos + 1), 80, 26, GFXFF);
  memoryposold = memorypos;
}

void DoMemoryPosTune() {
  if (band != memoryband[memorypos]) {
    band = memoryband[memorypos];
    SelectBand();
  } else {
    band = memoryband[memorypos];
  }

  if (band == BAND_FM) {
    frequency = memory[memorypos];
    radio.SetFreq(frequency);
  } else {
    frequency_AM = memory[memorypos];
    radio.SetFreqAM(frequency_AM);
  }
  ShowFreq(0);
}

void readRds() {
  radio.readRDS(showrdserrors);
  if (band == BAND_FM) {
    RDSstatus = radio.rds.hasRDS;
    ShowRDSLogo(RDSstatus);
    if (RDSstatus == 0 && screenmute == false) {
      tft.setTextColor(TFT_SKYBLUE);
      tft.setFreeFont(FONT14);
      tft.drawString(PIold, 244, 183, GFXFF);
      tft.setFreeFont(FONT14);
      tft.drawString(PSold, 38, 183, GFXFF);
      tft.setFreeFont(FONT7);
      tft.drawString(PTYold, 38, 164, GFXFF);
      tft.setTextColor(TFT_BLACK);
      tft.setFreeFont(FONT7);
      tft.drawString(RTold, 1, 223, GFXFF);
      dropout = true;
    } else {
      if (dropout == true && PIold.length() != 0) {
        tft.setTextColor(TFT_YELLOW);
        tft.setFreeFont(FONT14);
        tft.drawString(PIold, 244, 183, GFXFF);
        tft.setFreeFont(FONT14);
        tft.drawString(PSold, 38, 183, GFXFF);
        tft.setFreeFont(FONT7);
        tft.drawString(PTYold, 38, 164, GFXFF);
        dropout = false;
      }
    }

    if ((RDSstatus == 1 && RDSSPYUSB == true) || (RDSstatus == 1 && RDSSPYTCP == true)) {
      RDSSPYRDS = "G:\r\n";
      if (radio.rds.rdsAerror) RDSSPYRDS += "----"; else RDSSPYRDS += String(((radio.rds.rdsA >> 8) >> 4) & 0xF, HEX) + String((radio.rds.rdsA >> 8) & 0xF, HEX) + String(((radio.rds.rdsA) >> 4) & 0xF, HEX) + String((radio.rds.rdsA) & 0xF, HEX);
      if (radio.rds.rdsBerror) RDSSPYRDS += "----"; else RDSSPYRDS += String(((radio.rds.rdsB >> 8) >> 4) & 0xF, HEX) + String((radio.rds.rdsB >> 8) & 0xF, HEX) + String(((radio.rds.rdsB) >> 4) & 0xF, HEX) + String((radio.rds.rdsB) & 0xF, HEX);
      if (radio.rds.rdsCerror) RDSSPYRDS += "----"; else RDSSPYRDS += String(((radio.rds.rdsC >> 8) >> 4) & 0xF, HEX) + String((radio.rds.rdsC >> 8) & 0xF, HEX) + String(((radio.rds.rdsC) >> 4) & 0xF, HEX) + String((radio.rds.rdsC) & 0xF, HEX);
      if (radio.rds.rdsDerror) RDSSPYRDS += "----"; else RDSSPYRDS += String(((radio.rds.rdsD >> 8) >> 4) & 0xF, HEX) + String((radio.rds.rdsD >> 8) & 0xF, HEX) + String(((radio.rds.rdsD) >> 4) & 0xF, HEX) + String((radio.rds.rdsD) & 0xF, HEX);
      RDSSPYRDS += "\r\n\r\n";

      if (RDSSPYRDS != RDSSPYRDSold) {
        if (RDSSPYUSB) Serial.print(RDSSPYRDS); else RemoteClient.print(RDSSPYRDS);
        RDSSPYRDSold = RDSSPYRDS;
      }
    }

    if ((RDSstatus == 1 && XDRGTKUSB == true) || (RDSstatus == 1 && XDRGTKTCP == true)) {
      DataPrint ("P");
      DataPrint (String(((radio.rds.rdsA >> 8) >> 4) & 0xF, HEX) + String((radio.rds.rdsA >> 8) & 0xF, HEX));
      DataPrint (String(((radio.rds.rdsA) >> 4) & 0xF, HEX) + String((radio.rds.rdsA) & 0xF, HEX));
      if (radio.rds.correct == false) DataPrint("?");
      DataPrint ("\n");

      XDRGTKRDS = "R";
      XDRGTKRDS += String(((radio.rds.rdsB >> 8) >> 4) & 0xF, HEX) + String((radio.rds.rdsB >> 8) & 0xF, HEX);
      XDRGTKRDS += String(((radio.rds.rdsB) >> 4) & 0xF, HEX) + String((radio.rds.rdsB) & 0xF, HEX);
      XDRGTKRDS += String(((radio.rds.rdsC >> 8) >> 4) & 0xF, HEX) + String((radio.rds.rdsC >> 8) & 0xF, HEX);
      XDRGTKRDS += String(((radio.rds.rdsC) >> 4) & 0xF, HEX) + String((radio.rds.rdsC) & 0xF, HEX);
      XDRGTKRDS += String(((radio.rds.rdsD >> 8) >> 4) & 0xF, HEX) + String((radio.rds.rdsD >> 8) & 0xF, HEX);
      XDRGTKRDS += String(((radio.rds.rdsD) >> 4) & 0xF, HEX) + String((radio.rds.rdsD) & 0xF, HEX);
      XDRGTKRDS += String(((radio.rds.rdsErr >> 8) >> 4) & 0xF, HEX) + String((radio.rds.rdsErr >> 8) & 0xF, HEX);
      XDRGTKRDS += "\n";

      if (XDRGTKRDS != XDRGTKRDSold) {
        DataPrint(XDRGTKRDS);
        XDRGTKRDSold = XDRGTKRDS;
      }
    }
  }
}

void showAF() {
  if (radio.af_counter != af_counterold && radio.rds.hasAF == true) {
    if (wifi) {
      Udp.beginPacket(remoteip, 9030);
      Udp.print("AF=");
    }
    for (byte af_scan = 0; af_scan < radio.af_counter; af_scan++) {
      if (wifi) {
        if ((radio.af[af_scan].frequency - 8750) / 10 < 0x10) {
          Udp.print("0");
        }
        Udp.print((radio.af[af_scan].frequency - 8750) / 10, HEX);
      }
    }
    af_counterold = radio.af_counter;

    if (wifi) {
      for (int i = 0; i < 25 - radio.af_counter; i++) Udp.print("00");
      Udp.endPacket();
    }
  }
}

void showECC() {
  if (ECCold != radio.rds.ECC) {
    if (wifi) {
      Udp.beginPacket(remoteip, 9030);
      Udp.print("ECC=");
      if (radio.rds.ECC < 0x10) Udp.print("0");
      Udp.print(radio.rds.ECC, HEX);
      Udp.endPacket();
    }
    ECCold = radio.rds.ECC;
  }
}

void showPI() {
  if (strcmp(radio.rds.picode, radioIdPrevious)) {
    tft.setFreeFont(FONT14);
    tft.setTextColor(TFT_BLACK);
    tft.drawString(PIold, 244, 183, GFXFF);
    tft.setTextColor(TFT_YELLOW);
    tft.drawString(radio.rds.picode, 244, 183, GFXFF);
    PIold = radio.rds.picode;
    if (wifi) {
      Udp.beginPacket(remoteip, 9030);
      Udp.print("from=TEF tuner;PI=" + String(radio.rds.picode, 4));
      Udp.endPacket();
    }
    strcpy(radioIdPrevious, radio.rds.picode);
  }
}

void showPTY() {
  if (strcmp(radio.rds.stationType, programTypePrevious)) {
    tft.setFreeFont(FONT7);
    tft.setTextColor(TFT_BLACK);
    tft.drawString(PTYold, 38, 164, GFXFF);
    tft.setTextColor(TFT_YELLOW);
    tft.drawString(radio.rds.stationType, 38, 164, GFXFF);
    PTYold = radio.rds.stationType;
    if (wifi) {
      Udp.beginPacket(remoteip, 9030);
      Udp.print("from=TEF tuner;PTY=");
      Udp.print(radio.rds.stationTypeCode, HEX);
      Udp.endPacket();
    }
    strcpy(programTypePrevious, radio.rds.stationType);
  }
}

void showPS() {
  if (radio.rds.stationName != programServicePrevious) {
    tft.setFreeFont(FONT14);
    tft.setTextColor(TFT_BLACK);
    tft.drawString(PSold, 38, 183, GFXFF);
    tft.setTextColor(TFT_YELLOW);
    tft.drawString(radio.rds.stationName, 38, 183, GFXFF);
    PSold = radio.rds.stationName;
    if (wifi) {
      Udp.beginPacket(remoteip, 9030);
      Udp.print("from=TEF tuner;PS=");
      char PShex[9];
      radio.rds.stationName.toCharArray(PShex, 9);
      for (int i = 0; i < 8; i++)
      {
        if (PShex[i] < 0x10) Udp.print("0");
        if (PShex[i] == 0x20) PShex[i] =  '_';
        Udp.print(PShex[i], HEX);
      }
      Udp.endPacket();
    }
    programServicePrevious = radio.rds.stationName;
    tft.drawLine(0, 187, 320, 187, TFT_BLUE);
    tft.drawLine(0, 218, 320, 218, TFT_BLUE);
  }
}

void showRadioText() {
  if (RDSstatus == 1) {
    if (millis() - rtticker >= 350) {
      xPos -= charWidth;
      if (xPos < -tft.textWidth(radio.rds.stationText) + (charWidth * 24)) xPos = 6;
      sprite.setFreeFont(FONT7);
      sprite.setTextDatum(ML_DATUM);
      sprite.fillSprite(TFT_BLACK);
      sprite.setTextColor(TFT_YELLOW);
      sprite.drawString(radio.rds.stationText, xPos, yPos, GFXFF);
      sprite.pushSprite(1, 222);
      rtticker = millis();
      RTold = radio.rds.stationText;
      radioTextPrevious = radio.rds.stationText;
      cleanup = true;
    }
  } else if (cleanup == true) {
    sprite.fillSprite(TFT_BLACK);
    sprite.pushSprite(1, 222);
    cleanup = false;
  }

  if (wifi) {
    Udp.beginPacket(remoteip, 9030);
    Udp.print("from=TEF tuner;RT1=");
    char RThex[65];
    radio.rds.stationText.toCharArray(RThex, 65);
    for (int i = 0; i < 64; i++)
    {
      if (RThex[i] < 0x10) Udp.print("0");
      if (RThex[i] == ' ') RThex[i] =  '_';
      Udp.print(RThex[i], HEX);
    }
    Udp.endPacket();
  }
  tft.drawLine(0, 239, 320, 239, TFT_BLUE);
}

void showTP() {
  if (TPold != radio.rds.hasTP) {
    tft.setFreeFont(FONT7);
    if (radio.rds.hasTP == true) tft.setTextColor(TFT_SKYBLUE); else tft.setTextColor(TFT_GREYOUT);
    tft.drawRightString("TP", 205, 184, GFXFF);
    TPold = radio.rds.hasTP;
  }
}

void showTA() {
  if (TAold != radio.rds.hasTA) {
    tft.setFreeFont(FONT7);
    if (radio.rds.hasTA == true) tft.setTextColor(TFT_SKYBLUE); else tft.setTextColor(TFT_GREYOUT);
    tft.drawRightString("TA", 205, 198, GFXFF);
    TAold = radio.rds.hasTA;
  }
}

void showEON() {
  if (EONold != radio.rds.hasEON) {
    tft.setFreeFont(FONT7);
    if (radio.rds.hasEON == true) tft.setTextColor(TFT_SKYBLUE); else tft.setTextColor(TFT_GREYOUT);
    tft.drawRightString("EON", 183, 198, GFXFF);
    EONold = radio.rds.hasEON;
  }
}

void showMS() {
  if (MSold != radio.rds.MS) {
    tft.setFreeFont(FONT7);
    switch (radio.rds.MS) {
      case 0:
        tft.setTextColor(TFT_GREYOUT);
        tft.drawRightString("M", 183, 184, GFXFF);
        tft.drawString("S", 160, 184, GFXFF);
        break;

      case 1:
        tft.setTextColor(TFT_SKYBLUE);
        tft.drawRightString("M", 183, 184, GFXFF);
        tft.setTextColor(TFT_GREYOUT);
        tft.drawString("S", 160, 184, GFXFF);
        break;

      case 2:
        tft.setTextColor(TFT_GREYOUT);
        tft.drawRightString("M", 183, 184, GFXFF);
        tft.setTextColor(TFT_SKYBLUE);
        tft.drawString("S", 160, 184, GFXFF);
        break;
    }
    MSold = radio.rds.MS;
  }
}

void showCT() {
  if (radio.rds.hasCT == true) {
    int timeoffset;
    if (radio.rds.offsetplusmin == true) timeoffset = (-1 * radio.rds.offset) / 2; else timeoffset = radio.rds.offset / 2;
    byte rdshour = radio.rds.hours + timeoffset;
    rdshour = (((int)rdshour + 24) % 24);
    setTime(rdshour, radio.rds.minutes, 0, 0, 0, 0);
    rds_clock = ((hour() < 10 ? "0" : "") + String(hour()) + ":" + (minute() < 10 ? "0" : "") + String(minute()));
  } else {
    rds_clock = "";
  }
  if (rds_clock != rds_clockold) {
    tft.setFreeFont(FONT7);
    tft.setTextColor(TFT_BLACK);
    tft.drawRightString(rds_clockold, 205, 164, GFXFF);
    tft.setTextColor(TFT_YELLOW);
    tft.drawRightString(rds_clock, 205, 164, GFXFF);
    rds_clockold = rds_clock;
  }
}

void BuildMenu() {
  tft.setFreeFont(FONT7);
  tft.fillScreen(TFT_BLACK);
  tft.drawRect(0, 0, 320, 240, TFT_BLUE);
  tft.drawLine(0, 23, 320, 23, TFT_BLUE);
  tft.setTextColor(TFT_SKYBLUE);
  tft.drawString(myLanguage[language][41], 10, 1, GFXFF);
  tft.setTextColor(TFT_WHITE);
  tft.drawRightString(String(menupage) + "/" + String(menupagestotal), 305, 1, GFXFF);
  tft.drawRoundRect(8, menuoption + 3, 302, 19, 5, TFT_WHITE);
  tft.setTextColor(TFT_WHITE);
  switch (menupage) {
    case 1:
      tft.drawRightString("dB", 305, 30, GFXFF);
      tft.drawRightString("MHz", 305, 52, GFXFF);
      tft.drawRightString("MHz", 305, 70, GFXFF);
      tft.drawRightString("MHz", 305, 90, GFXFF);
      tft.drawRightString("dB", 305, 110, GFXFF);
      if (StereoLevel != 0) tft.drawRightString("dBμV", 305, 130, GFXFF);
      if (HighCutLevel != 0) tft.drawRightString("Hz", 305, 150, GFXFF);
      if (HighCutOffset != 0) tft.drawRightString("dBμV", 305, 170, GFXFF);
      tft.drawRightString("dBμV", 305, 190, GFXFF);
      tft.drawRightString("%", 305, 210, GFXFF);
      tft.drawString(myLanguage[language][20], 14, 30, GFXFF);
      tft.drawString(myLanguage[language][21], 14, 50, GFXFF);
      tft.drawString(myLanguage[language][22], 14, 70, GFXFF);
      tft.drawString(myLanguage[language][23], 14, 90, GFXFF);
      tft.drawString(myLanguage[language][24], 14, 110, GFXFF);
      tft.drawString(myLanguage[language][25], 14, 130, GFXFF);
      tft.drawString(myLanguage[language][26], 14, 150, GFXFF);
      tft.drawString(myLanguage[language][27], 14, 170, GFXFF);
      tft.drawString(myLanguage[language][28], 14, 190, GFXFF);
      tft.drawString(myLanguage[language][29], 14, 210, GFXFF);
      tft.setTextColor(TFT_YELLOW);
      if (VolSet > 0) tft.drawRightString("+" + String(VolSet, DEC), 265, 30, GFXFF); else tft.drawRightString(String(VolSet, DEC), 265, 30, GFXFF);
      tft.drawRightString(String(ConverterSet, DEC), 265, 50, GFXFF);
      tft.drawRightString(String(LowEdgeSet / 10 + ConverterSet, DEC) + "." + String(LowEdgeSet % 10 + ConverterSet, DEC), 265, 70, GFXFF);
      tft.drawRightString(String(HighEdgeSet / 10 + ConverterSet, DEC) + "." + String(HighEdgeSet % 10 + ConverterSet, DEC), 265, 90, GFXFF);
      if (LevelOffset > 0) tft.drawRightString("+" + String(LevelOffset, DEC), 265, 110, GFXFF); else tft.drawRightString(String(LevelOffset, DEC), 265, 110, GFXFF);
      if (StereoLevel != 0) tft.drawRightString(String(StereoLevel, DEC), 265, 130, GFXFF); else tft.drawRightString(myLanguage[language][30], 265, 130, GFXFF);
      if (HighCutLevel != 0) tft.drawRightString(String(HighCutLevel * 100, DEC), 265, 150, GFXFF); else tft.drawRightString(myLanguage[language][30], 265, 150, GFXFF);
      if (HighCutOffset != 0) tft.drawRightString(String(HighCutOffset, DEC), 265, 170, GFXFF); else tft.drawRightString(myLanguage[language][30], 265, 170, GFXFF);
      tft.drawRightString(String(LowLevelSet, DEC), 265, 190, GFXFF);
      tft.drawRightString(String(ContrastSet, DEC), 265, 210, GFXFF);
      break;

    case 2:
      tft.drawString(myLanguage[language][39], 14, 30, GFXFF);
      tft.drawString(myLanguage[language][38], 14, 50, GFXFF);
      tft.drawString(myLanguage[language][43], 14, 70, GFXFF);
      tft.drawString(myLanguage[language][44], 14, 90, GFXFF);
      tft.drawString(myLanguage[language][45], 14, 110, GFXFF);
      tft.drawString(myLanguage[language][46], 14, 130, GFXFF);
      tft.drawString(myLanguage[language][49], 14, 150, GFXFF);
      tft.drawString(myLanguage[language][50], 14, 170, GFXFF);
      if (wifi) tft.drawString(String(myLanguage[language][51]) + " IP: " + String(WiFi.localIP().toString()), 14, 190, GFXFF); else tft.drawString(myLanguage[language][51], 14, 190, GFXFF);
      tft.drawString(myLanguage[language][52], 14, 210, GFXFF);
      tft.setTextColor(TFT_YELLOW);
      tft.drawRightString(myLanguage[language][0], 305, 30, GFXFF);
      if (showrdserrors) tft.drawRightString(myLanguage[language][42], 305, 50, GFXFF); else tft.drawRightString(myLanguage[language][30], 305, 50, GFXFF);
      if (softmutefm) tft.drawRightString(myLanguage[language][42], 305, 70, GFXFF); else tft.drawRightString(myLanguage[language][30], 305, 70, GFXFF);
      if (softmuteam) tft.drawRightString(myLanguage[language][42], 305, 90, GFXFF); else tft.drawRightString(myLanguage[language][30], 305, 90, GFXFF);
      if (edgebeep) tft.drawRightString(myLanguage[language][42], 305, 110, GFXFF); else tft.drawRightString(myLanguage[language][30], 305, 110, GFXFF);
      if (region == 0) tft.drawRightString(myLanguage[language][47], 305, 130, GFXFF);
      if (region == 1) tft.drawRightString(myLanguage[language][48], 305, 130, GFXFF);
      if (radio.rds.underscore) tft.drawRightString(myLanguage[language][42], 305, 150, GFXFF); else tft.drawRightString(myLanguage[language][30], 305, 150, GFXFF);
      if (USBmode) tft.drawRightString("RDS Spy", 305, 170, GFXFF); else tft.drawRightString("XDR-GTK", 305, 170, GFXFF);
      if (wifi) tft.drawRightString(myLanguage[language][42], 305, 190, GFXFF); else tft.drawRightString(myLanguage[language][30], 305, 190, GFXFF);
      tft.drawRightString("→", 305, 210, GFXFF);
      break;

    case 3:
      tft.drawString(myLanguage[language][58], 14, 30, GFXFF);
      tft.drawString(myLanguage[language][59], 14, 50, GFXFF);
      tft.drawString(myLanguage[language][60], 14, 70, GFXFF);
      tft.drawString(myLanguage[language][61], 14, 90, GFXFF);
      tft.drawString(myLanguage[language][62], 14, 110, GFXFF);
      tft.drawString(myLanguage[language][63], 14, 130, GFXFF);
      tft.drawString(myLanguage[language][64], 14, 150, GFXFF);
      tft.drawString(myLanguage[language][65], 14, 170, GFXFF);
      tft.drawString(myLanguage[language][67], 14, 190, GFXFF);
      if (amnb != 0) tft.drawRightString("%", 305, 150, GFXFF); else tft.drawRightString(myLanguage[language][30], 265, 150, GFXFF);
      if (fmnb != 0) tft.drawRightString("%", 305, 170, GFXFF); else tft.drawRightString(myLanguage[language][30], 265, 170, GFXFF);
      tft.setTextColor(TFT_YELLOW);
      tft.drawRightString(String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + "." + String(WiFi.localIP()[2]) + "." + String(subnetclient, DEC), 305, 30, GFXFF);
      if (showSWMIBand) tft.drawRightString(myLanguage[language][42], 305, 50, GFXFF); else tft.drawRightString(myLanguage[language][30], 305, 50, GFXFF);
      if (radio.rds.filter) tft.drawRightString(myLanguage[language][42], 305, 70, GFXFF); else tft.drawRightString(myLanguage[language][30], 305, 70, GFXFF);
      if (radio.rds.dynamicpi) tft.drawRightString(myLanguage[language][42], 305, 90, GFXFF); else tft.drawRightString(myLanguage[language][30], 305, 90, GFXFF);
      if (showsquelch) tft.drawRightString(myLanguage[language][42], 305, 110, GFXFF); else tft.drawRightString(myLanguage[language][30], 305, 110, GFXFF);
      if (showmodulation) tft.drawRightString(myLanguage[language][42], 305, 130, GFXFF); else tft.drawRightString(myLanguage[language][30], 305, 130, GFXFF);
      if (amnb != 0) tft.drawRightString(String(amnb, DEC), 265, 150, GFXFF); else tft.drawRightString(myLanguage[language][30], 265, 150, GFXFF);
      if (fmnb != 0) tft.drawRightString(String(fmnb, DEC), 265, 170, GFXFF); else tft.drawRightString(myLanguage[language][30], 265, 170, GFXFF);
      if (audiomode) tft.drawRightString("MPX", 305, 190, GFXFF); else tft.drawRightString("Stereo", 305, 190, GFXFF);
  }
  analogWrite(SMETERPIN, 0);
}

void MuteScreen(int setting) {
  if (setting == 0 && screenmute == 1) {
    screenmute = false;
    setupmode = true;
    BuildDisplay();
    setupmode = false;
  }

  if (setting == 1 && screenmute == 0) {
    screenmute = true;
    tft.setFreeFont(FONT14);
    tft.fillScreen(TFT_BLACK);
    tft.drawRect(0, 0, 320, 240, TFT_BLUE);
    tft.setTextColor(TFT_WHITE);
    tft.drawCentreString(myLanguage[language][31], 160, 30, GFXFF);
    tft.drawCentreString(myLanguage[language][32], 160, 90, GFXFF);
  }
}

void BuildDisplay() {
  if (theme == 0) {
    tft.fillScreen(TFT_BLACK);
    tft.drawRect(0, 0, 320, 240, TFT_BLUE);
    tft.drawLine(0, 30, 320, 30, TFT_BLUE);
    tft.drawLine(0, 100, 320, 100, TFT_BLUE);
    tft.drawLine(64, 30, 64, 0, TFT_BLUE);
    tft.drawLine(210, 100, 210, 218, TFT_BLUE);
    tft.drawLine(268, 30, 268, 0, TFT_BLUE);
    tft.drawLine(0, 165, 210, 165, TFT_BLUE);
    tft.drawLine(0, 187, 320, 187, TFT_BLUE);
    tft.drawLine(0, 218, 320, 218, TFT_BLUE);
    tft.drawLine(108, 30, 108, 0, TFT_BLUE);
    tft.drawLine(174, 30, 174, 0, TFT_BLUE);
    tft.drawLine(20, 120, 200, 120, TFT_DARKGREY);
    if (!showmodulation) tft.drawLine(20, 150, 200, 150, TFT_GREYOUT); else tft.drawLine(20, 150, 200, 150, TFT_DARKGREY);
    for (uint16_t segments = 0; segments < 94; segments++) {
      if (segments > 54) {
        if (((segments - 53) % 10) == 0)
          tft.fillRect(16 + (2 * segments), 117, 2, 3, TFT_RED);
      } else {
        if (((segments + 1) % 6) == 0)
          tft.fillRect(16 + (2 * segments), 117, 2, 3, TFT_GREEN);
      }
    }
    tft.setTextColor(TFT_WHITE);
    tft.setFreeFont(FONT7);
    if (showsquelch) tft.drawString("SQ:", 216, 150, GFXFF);
    tft.drawString("S/N", 250, 164, GFXFF);
    tft.drawString("dB",  300, 164, GFXFF);
    tft.drawString("S", 6, 100, GFXFF);
    if (region == 0) tft.drawString("PI:", 216, 191, GFXFF);
    if (region == 1) tft.drawString("ID:", 216, 191, GFXFF);
    tft.drawString("PS:", 6, 191, GFXFF);
    tft.drawString("PTY:", 6, 164, GFXFF);
    tft.drawString("1", 24, 116, GFXFF);
    tft.drawString("3", 48, 116, GFXFF);
    tft.drawString("5", 72, 116, GFXFF);
    tft.drawString("7", 96, 116, GFXFF);
    tft.drawString("9", 120, 116, GFXFF);
    tft.drawString("+10", 134, 116, GFXFF);
    tft.drawString("+30", 174, 116, GFXFF);
    if (!showmodulation) tft.setTextColor(TFT_GREYOUT);
    tft.drawString("M", 6, 132, GFXFF);
    tft.drawString("20", 20, 146, GFXFF);
    tft.drawString("40", 50, 146, GFXFF);
    tft.drawString("60", 80, 146, GFXFF);
    tft.drawString("80", 110, 146, GFXFF);
    tft.drawString("100", 134, 146, GFXFF);
    tft.drawString("120", 164, 146, GFXFF);
    tft.drawString("%", 196, 146, GFXFF);
    tft.setTextColor(TFT_WHITE);
    tft.setFreeFont(FONT14);
    tft.drawString("kHz", 225, -5, GFXFF);
    tft.setTextColor(TFT_WHITE);
    if (band == BAND_FM) tft.drawString("MHz", 258, 67, GFXFF); else tft.drawString("kHz", 258, 67, GFXFF);
    tft.drawCircle(81, 15, 10, TFT_GREYOUT);
    tft.drawCircle(81, 15, 9, TFT_GREYOUT);
    tft.drawCircle(91, 15, 10, TFT_GREYOUT);
    tft.drawCircle(91, 15, 9, TFT_GREYOUT);
    tft.drawBitmap(110, 5, RDSLogo, 67, 22, TFT_GREYOUT);
    if (StereoToggle == false) {
      tft.drawCircle(86, 15, 10, TFT_SKYBLUE);
      tft.drawCircle(86, 15, 9, TFT_SKYBLUE);
    }
    tft.setTextColor(TFT_SKYBLUE);
    tft.setFreeFont(FONT7);
    if (band == BAND_LW) tft.drawString("LW", 50, 26, GFXFF);
    else if (band == BAND_MW) tft.drawString("MW", 50, 26, GFXFF);
    else if (band == BAND_SW) tft.drawString("SW", 50, 26, GFXFF);
    else tft.drawString("FM", 50, 26, GFXFF);
    tft.setTextColor(TFT_GREYOUT);
    tft.drawString("S", 160, 184, GFXFF);
    tft.drawRightString("M", 183, 184, GFXFF);
    tft.drawRightString("EON", 183, 198, GFXFF);
    tft.drawRightString("TA", 205, 198, GFXFF);
    tft.drawRightString("TP", 205, 184, GFXFF);
    radio.clearRDS(fullsearchrds);
  }


  // WORKING ON THIS!
  /*
    if (theme == 1) {
    tft.setFreeFont(FONT7);
    tft.fillScreen(TFT_BLACK);
    tft.fillRect(12, 105, 2, 50, TFT_RED);
    tft.fillRect(305, 105, 2, 50, TFT_RED);
    tft.fillRect(0, 208, 50, 2, TFT_RED);
    tft.fillRect(270, 208, 50, 2, TFT_RED);
    tft.fillRect(12, 18, 293, 2, TFT_RED);
    tft.fillRect(30, 220, 98, 6, TFT_RED);
    tft.fillRect(204, 220, 98, 6, TFT_RED);

    tft.fillRect(44, 138, 2, 14, TFT_WHITE);
    tft.fillRect(70, 138, 2, 14, TFT_WHITE);
    tft.fillRect(96, 38, 20, 4, TFT_DARKGREY);
    tft.fillRect(129, 38, 20, 4, TFT_DARKGREY);
    tft.fillRect(173, 38, 20, 4, TFT_DARKGREY);
    tft.fillRect(207, 38, 20, 4, TFT_DARKGREY);

    tft.setFreeFont(GUI2_FONT12);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("PI:", 26, 102, GFXFF);

    tft.setFreeFont(GUI2_FONT12);
    tft.setTextColor(TFT_SKYBLUE);
    tft.drawString("232F", 56, 102, GFXFF);

    tft.setFreeFont(GUI2_FONT12);
    tft.setTextColor(TFT_SKYBLUE);
    tft.drawRightString("36.2", 250, 102, GFXFF);

    tft.setFreeFont(GUI2_FONT12);
    tft.setTextColor(TFT_WHITE);
    tft.drawRightString("dBf", 294, 102, GFXFF);

    tft.setTextColor(TFT_WHITE);
    tft.setFreeFont(GUI2_FONT7);
    tft.drawRightString("dB S/N", 294, 132, GFXFF);

    tft.setFreeFont(GUI2_FONT7);
    tft.setTextColor(TFT_SKYBLUE);
    tft.drawRightString("20", 244, 132, GFXFF);

    tft.setTextColor(TFT_SKYBLUE);
    tft.setFreeFont(GUI2_FONT12);
    tft.drawCentreString("R-ZURNAL", 160, 156, GFXFF);

    tft.setTextColor(TFT_DARKGREY);
    tft.setFreeFont(GUI2_FONT7);
    tft.drawCentreString("Zelena vlna - volejte bezplatne 800 500 553 !", 160, 180, GFXFF);

    tft.setFreeFont(GUI2_FONT7);
    tft.setTextColor(TFT_DARKGREY);
    tft.drawCentreString("Information", 160, 140, GFXFF);

    tft.setTextColor(TFT_SKYBLUE);
    tft.setFreeFont(GUI2_FONT7);
    tft.drawRightString("TP", 40, 132, GFXFF);

    tft.setTextColor(TFT_DARKGREY);
    tft.setFreeFont(GUI2_FONT7);
    tft.drawCentreString("TA", 57, 132, GFXFF);

    tft.setTextColor(TFT_DARKGREY);
    tft.setFreeFont(GUI2_FONT7);
    tft.drawString("AF", 76, 132, GFXFF);

    tft.setTextColor(TFT_WHITE);
    tft.setFreeFont(GUI2_FONT12);
    tft.drawString("kHz", 162, 92, GFXFF);

    tft.setTextColor(TFT_SKYBLUE);
    tft.setFreeFont(GUI2_FONT12);
    tft.drawRightString("236", 158, 92, GFXFF);

    tft.setTextColor(TFT_SKYBLUE);
    tft.setFreeFont(FONTDEC);
    tft.drawCentreString("88.50", 160, 49, GFXFF);

    tft.setFreeFont(GUI2_FONT7);
    tft.setTextColor(TFT_DARKGREY);
    tft.drawCentreString("MANUAL", 160, 16, GFXFF);

    tft.setFreeFont(GUI2_FONT7);
    tft.setTextColor(TFT_WHITE);
    tft.drawCentreString("21:35", 160, -4, GFXFF);

    tft.setFreeFont(GUI2_FONT7);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("89 %", 243, -4, GFXFF);

    tft.setFreeFont(GUI2_FONT12);
    tft.setTextColor(TFT_SKYBLUE);
    tft.drawString("S", 14, 204, GFXFF);
    tft.drawString("M", 182, 204, GFXFF);

    tft.setFreeFont(GUI2_FONT7);
    tft.setTextColor(TFT_DARKGREY);
    tft.drawString("1 3 5 7 9  +10  +30", 30, 220, GFXFF);
    tft.drawString("20 50 70 100 120", 204, 220, GFXFF);

    for (;;);
    }
  */
  RDSstatusold = false;
  Stereostatusold = false;
  ShowFreq(0);
  ShowTuneMode();
  updateBW();
  ShowStepSize();
  ShowMemoryPos();
  updateiMS();
  updateEQ();
  Squelchold = -2;
  SStatusold = 2000;
  SStatus = 100;
  rssiold = 2000;
  batteryold = 6;
  rds_clockold = "";
  strcpy(programTypePrevious, "0");
  strcpy(radioIdPrevious, "0");
  programServicePrevious = "0";
  radioTextPrevious = "0";
}

void ShowFreq(int mode) {
  if (screenmute == false) {
    if (setupmode == false) {
      if (band == 1) { // Fix Me :take care of 9K/10K Step
        if (freqold < 2000 && frequency_AM >= 2000 && stepsize == 0) if (frequency_AM != 27000 && freqold != 144) radio.SetFreqAM(2000);
        if (freqold >= 2000 && frequency_AM < 2000 && stepsize == 0) if (frequency_AM != 144 && freqold != 27000) radio.SetFreqAM(1998);
      }
    }

    if (screenmute == false) {
      detachInterrupt(digitalPinToInterrupt(ROTARY_PIN_A));
      detachInterrupt(digitalPinToInterrupt(ROTARY_PIN_B));
      if (band != BAND_FM ) {
        unsigned int freq = frequency_AM;
        String count = String(freq, DEC);
        if (setupmode == false && count.length() != freqoldcount || mode != 0) {
          tft.setTextColor(TFT_BLACK);
          tft.drawRightString(String(freqold), 248, 45, 7);
        }
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        tft.drawRightString(String(freq), 248, 45, 7);
        freqold = freq;
        freqoldcount = count.length();

        if (band == BAND_SW && showSWMIBand) {
          DivdeSWMIBand();
          updateSWMIBand();
        }

      } else {
        unsigned int freq = frequency + ConverterSet * 100;
        String count = String(freq / 100, DEC);
        if (setupmode == false && count.length() != freqoldcount || mode != 0) {
          tft.setTextColor(TFT_BLACK);
          if (freqoldcount <= 2) tft.setCursor (108, 45);
          if (freqoldcount == 3) tft.setCursor (76, 45);
          if (freqoldcount >= 4) tft.setCursor (44, 45);
          tft.setTextFont(7);
          tft.print(freqold / 100);
          if (band == BAND_FM) {
            tft.print(".");
            if (freqold % 100 < 10) tft.print("0");
            tft.print(freqold % 100);
          }
        }

        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        if (mode == 0) {
          if (count.length() <= 2) tft.setCursor (108, 45);
          if (count.length() == 3) tft.setCursor (76, 45);
          if (count.length() >= 4) tft.setCursor (44, 45);
          tft.setTextFont(7);
          tft.print(freq / 100);
          tft.print(".");
          if (freq % 100 < 10) tft.print("0");
          tft.print(freq % 100);
          freqold = freq;
          freqoldcount = count.length();
        } else if (mode == 1) {
          tft.setTextColor(TFT_BLACK);
          if (freqoldcount <= 2) tft.setCursor (98, 45);
          if (freqoldcount == 3) tft.setCursor (71, 45);
          if (freqoldcount == 4) tft.setCursor (44, 45);
          tft.setTextFont(1);
          tft.print(freqold / 100);
          tft.print(".");
          if (freqold % 100 < 10) tft.print("0");
          tft.print(freqold % 100);
        }
      }
      attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_A), read_encoder, CHANGE);
      attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_B), read_encoder, CHANGE);
    }
    strcpy(programTypePrevious, "0");
    strcpy(radioIdPrevious, "0");
    programServicePrevious = "0";
    radioTextPrevious = "0";
    if (wifi) {
      Udp.beginPacket(remoteip, 9030);
      Udp.print("from=TEF tuner;freq=");
      if (band != BAND_FM) Udp.print(String(frequency_AM) + "000;ClearRDS=1"); else Udp.print(String(frequency) + "0000;ClearRDS=1");
      Udp.endPacket();
    }
  }
}

void ShowSignalLevel() {
  if (screenmute == false) {
    if (band == BAND_FM) SNR = int(0.46222375 * (float)(SStatus / 10) - 0.082495118 * (float)(USN / 10)) + 10; else SNR = -((int8_t)(USN / 10));

    if (millis() >= snrupdatetimer + TIMER_SNR_TIMER) {
      snrupdatetimer = millis();

      if (SNR > (SNRold + 1) || SNR < (SNRold - 1)) {
        tft.setFreeFont(FONT7);
        tft.setTextColor(TFT_BLACK);
        if (SNRold == 99) tft.drawRightString("--", 294, 164, GFXFF); else  tft.drawRightString(String(SNRold), 294, 164, GFXFF);
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        if (tuned == true) {
          tft.drawRightString(String(SNR), 294, 164, GFXFF);
          SNRold = SNR;
        } else {
          tft.drawRightString("--", 294, 164, GFXFF);
          SNRold = 99;
        }
      }
    }

    SAvg = (((SAvg * 9) + 5) / 10) + SStatus;
    SAvg2 = (((SAvg2 * 9) + 5) / 10) + SNR;

    float sval = 0;
    int16_t smeter = 0;
    int16_t segments;

    if (SStatus > 0) {
      if (SStatus < 1000) {
        sval = 51 * ((pow(10, (((float)SStatus) / 1000))) - 1);
        smeter = int16_t(sval);
      } else {
        smeter = 511;
      }
    }

    smeter = int16_t(sval);
    SStatus = SAvg / 10;
    SNR = SAvg2 / 10;

    if (menu == false) analogWrite(SMETERPIN, smeter);

    if (SStatus > (SStatusold + 3) || SStatus < (SStatusold - 3)) {
      if (SStatus > 1200) SStatus = 1200;
      if (SStatus < -400) SStatus = -400;
      tft.setFreeFont(FONT24);
      tft.setTextColor(TFT_BLACK);
      if (SStatusold / 10 != SStatus / 10) tft.drawRightString(String(SStatusold / 10), 290, 106, GFXFF);
      tft.setFreeFont(FONT14);
      tft.drawString("." + String(abs(SStatusold % 10)), 296, 97, GFXFF);
      tft.setFreeFont(FONT24);
      tft.setTextColor(TFT_YELLOW);
      tft.drawRightString(String(SStatus / 10), 290, 106, GFXFF);
      tft.setFreeFont(FONT14);
      tft.drawString("." + String(abs(SStatus % 10)), 296, 97, GFXFF);

      if (band == BAND_FM) segments = (SStatus + 200) / 10; else segments = (SStatus + 200) / 10;

      tft.fillRect(16, 109, 2 * constrain(segments, 0, 54), 8, TFT_GREEN);
      tft.fillRect(16 + 2 * 54, 109, 2 * (constrain(segments, 54, 94) - 54), 8, TFT_RED);
      tft.fillRect(16 + 2 * constrain(segments, 0, 94), 109, 2 * (94 - constrain(segments, 0, 94)), 8, TFT_GREYOUT);

      SStatusold = SStatus;
      tft.setTextColor(TFT_WHITE);
      tft.setFreeFont(FONT7);
      tft.drawString("dBμV", 282, 144, GFXFF);
      if (wifi) {
        Udp.beginPacket(remoteip, 9030);
        Udp.print("from=TEF tuner;RcvLevel=");
        Udp.print(SStatus / 10);
        Udp.endPacket();
      }
    }
  }
}

void ShowRDSLogo(bool RDSstatus) {
  if (screenmute == false) {
    if (RDSstatus != RDSstatusold) {
      if (RDSstatus == true) tft.drawBitmap(110, 5, RDSLogo, 67, 22, TFT_SKYBLUE); else tft.drawBitmap(110, 5, RDSLogo, 67, 22, TFT_GREYOUT);
      RDSstatusold = RDSstatus;
    }
  }
}

void ShowStereoStatus() {
  if (StereoToggle == true) {
    if (band == BAND_FM) Stereostatus = radio.getStereoStatus(); else Stereostatus = 0;
    if (Stereostatus != Stereostatusold) {
      if (Stereostatus == true && screenmute == false) {
        tft.drawCircle(81, 15, 10, TFT_RED);
        tft.drawCircle(81, 15, 9, TFT_RED);
        tft.drawCircle(91, 15, 10, TFT_RED);
        tft.drawCircle(91, 15, 9, TFT_RED);
      } else {
        if (screenmute == false) {
          tft.drawCircle(81, 15, 10, TFT_GREYOUT);
          tft.drawCircle(81, 15, 9, TFT_GREYOUT);
          tft.drawCircle(91, 15, 10, TFT_GREYOUT);
          tft.drawCircle(91, 15, 9, TFT_GREYOUT);
        }
      }
      Stereostatusold = Stereostatus;
    }
  }
}

void ShowOffset() {
  if (OStatus != OStatusold) {
    if (band == BAND_FM) {
      if (OStatus < -500) {
        tft.fillTriangle(6, 8, 6, 22, 14, 14, TFT_GREYOUT);
        tft.fillTriangle(18, 8, 18, 22, 26, 14, TFT_GREYOUT);
        tft.fillCircle(32, 15, 3, TFT_GREYOUT);
        tft.fillTriangle(38, 14, 46, 8, 46, 22, TFT_GREYOUT);
        tft.fillTriangle(50, 14, 58, 8, 58, 22, TFT_RED);
        tuned = false;
      } else if (OStatus < -250 && OStatus > -500) {
        tft.fillTriangle(6, 8, 6, 22, 14, 14, TFT_GREYOUT);
        tft.fillTriangle(18, 8, 18, 22, 26, 14, TFT_GREYOUT);
        tft.fillCircle(32, 15, 3, TFT_GREYOUT);
        tft.fillTriangle(38, 14, 46, 8, 46, 22, TFT_RED);
        tft.fillTriangle(50, 14, 58, 8, 58, 22, TFT_GREYOUT);
        tuned = false;
      } else if (USN < 250 && WAM < 250 && OStatus > -250 && OStatus < 250 && SQ == false) {
        tft.fillTriangle(6, 8, 6, 22, 14, 14, TFT_GREYOUT);
        tft.fillTriangle(18, 8, 18, 22, 26, 14, TFT_GREYOUT);
        tft.fillCircle(32, 15, 3, TFT_GREEN);
        tft.fillTriangle(38, 14, 46, 8, 46, 22, TFT_GREYOUT);
        tft.fillTriangle(50, 14, 58, 8, 58, 22, TFT_GREYOUT);
        tuned = true;
      } else if (OStatus > 250 && OStatus < 500) {
        tft.fillTriangle(6, 8, 6, 22, 14, 14, TFT_GREYOUT);
        tft.fillTriangle(18, 8, 18, 22, 26, 14, TFT_RED);
        tft.fillCircle(32, 15, 3, TFT_GREYOUT);
        tft.fillTriangle(38, 14, 46, 8, 46, 22, TFT_GREYOUT);
        tft.fillTriangle(50, 14, 58, 8, 58, 22, TFT_GREYOUT);
        tuned = false;
      } else if (OStatus > 500) {
        tft.fillTriangle(6, 8, 6, 22, 14, 14, TFT_RED);
        tft.fillTriangle(18, 8, 18, 22, 26, 14, TFT_GREYOUT);
        tft.fillCircle(32, 15, 3, TFT_GREYOUT);
        tft.fillTriangle(38, 14, 46, 8, 46, 22, TFT_GREYOUT);
        tft.fillTriangle(50, 14, 58, 8, 58, 22, TFT_GREYOUT);
        tuned = false;
      } else {
        tft.fillTriangle(6, 8, 6, 22, 14, 14, TFT_GREYOUT);
        tft.fillTriangle(18, 8, 18, 22, 26, 14, TFT_GREYOUT);
        tft.fillCircle(32, 15, 3, TFT_GREYOUT);
        tft.fillTriangle(38, 14, 46, 8, 46, 22, TFT_GREYOUT);
        tft.fillTriangle(50, 14, 58, 8, 58, 22, TFT_GREYOUT);
        tuned = false;
      }
    } else {
      if (OStatus <= -3) {
        tft.fillTriangle(6, 8, 6, 22, 14, 14, TFT_GREYOUT);
        tft.fillTriangle(18, 8, 18, 22, 26, 14, TFT_GREYOUT);
        tft.fillCircle(32, 15, 3, TFT_GREYOUT);
        tft.fillTriangle(38, 14, 46, 8, 46, 22, TFT_GREYOUT);
        tft.fillTriangle(50, 14, 58, 8, 58, 22, TFT_RED);
        tuned = false;
      } else if (OStatus < -2 && OStatus > -3) {
        tft.fillTriangle(6, 8, 6, 22, 14, 14, TFT_GREYOUT);
        tft.fillTriangle(18, 8, 18, 22, 26, 14, TFT_GREYOUT);
        tft.fillCircle(32, 15, 3, TFT_GREYOUT);
        tft.fillTriangle(38, 14, 46, 8, 46, 22, TFT_RED);
        tft.fillTriangle(50, 14, 58, 8, 58, 22, TFT_GREYOUT);
        tuned = false;
      } else if (OStatus > -2 && OStatus < 2 && SQ == false) {
        tft.fillTriangle(6, 8, 6, 22, 14, 14, TFT_GREYOUT);
        tft.fillTriangle(18, 8, 18, 22, 26, 14, TFT_GREYOUT);
        tft.fillCircle(32, 15, 3, TFT_GREEN);
        tft.fillTriangle(38, 14, 46, 8, 46, 22, TFT_GREYOUT);
        tft.fillTriangle(50, 14, 58, 8, 58, 22, TFT_GREYOUT);
        tuned = true;
      } else if (OStatus > 2 && OStatus < 3) {
        tft.fillTriangle(6, 8, 6, 22, 14, 14, TFT_GREYOUT);
        tft.fillTriangle(18, 8, 18, 22, 26, 14, TFT_RED);
        tft.fillCircle(32, 15, 3, TFT_GREYOUT);
        tft.fillTriangle(38, 14, 46, 8, 46, 22, TFT_GREYOUT);
        tft.fillTriangle(50, 14, 58, 8, 58, 22, TFT_GREYOUT);
        tuned = false;
      } else if (OStatus >= 3) {
        tft.fillTriangle(6, 8, 6, 22, 14, 14, TFT_RED);
        tft.fillTriangle(18, 8, 18, 22, 26, 14, TFT_GREYOUT);
        tft.fillCircle(32, 15, 3, TFT_GREYOUT);
        tft.fillTriangle(38, 14, 46, 8, 46, 22, TFT_GREYOUT);
        tft.fillTriangle(50, 14, 58, 8, 58, 22, TFT_GREYOUT);
        tuned = false;
      } else {
        tft.fillTriangle(6, 8, 6, 22, 14, 14, TFT_GREYOUT);
        tft.fillTriangle(18, 8, 18, 22, 26, 14, TFT_GREYOUT);
        tft.fillCircle(32, 15, 3, TFT_GREYOUT);
        tft.fillTriangle(38, 14, 46, 8, 46, 22, TFT_GREYOUT);
        tft.fillTriangle(50, 14, 58, 8, 58, 22, TFT_GREYOUT);
        tuned = false;
      }
    }
    OStatusold = OStatus;
  }
}

void ShowBW() {
  if (millis() >= bwupdatetimer + TIMER_BW_TIMER) {
    bwupdatetimer = millis();
  }
  else {
    return;
  }

  if (BW != BWOld || BWreset == true) {
    tft.setFreeFont(FONT14);
    tft.setTextColor(TFT_BLACK);
    tft.drawRightString(String (BWOld, DEC), 218, -4, GFXFF);
    if (BWset == 0) tft.setTextColor(TFT_SKYBLUE); else tft.setTextColor(TFT_YELLOW);
    tft.drawRightString(String (BW, DEC), 218, -4, GFXFF);
    BWOld = BW;
    BWreset = false;
    if (wifi) {
      Udp.beginPacket(remoteip, 9030);
      Udp.print("from=TEF tuner;Bandwidth=");
      Udp.print(BW * 1000);
      Udp.endPacket();
    }
  }
}

void ShowModLevel() {
  if (showmodulation) {
    int segments;
    int color;
    int hold = 0;

    if (SQ != false) {
      MStatus = 0;
      MStatusold = 1;
    }

    if (MStatus != MStatusold || MStatus < 10) {
      for (segments = 0; segments < 13; segments++) {
        color = TFT_GREEN;
        if (segments > 8) color = TFT_ORANGE;
        if (segments > 9) color = TFT_RED;
        if (MStatus > (segments + 1) * 10) {
          hold = segments;
          tft.fillRect(20 + segments * 14, 139, 12, 8, color);
        } else {
          if (segments != peakholdold) tft.fillRect(20 + segments * 14, 139, 12, 8, TFT_GREYOUT);
        }
      }

      if (peakholdold < hold) peakholdold = hold;

      if (peakholdmillis > peakholdtimer + 3000) {
        peakholdtimer += 3000;
        peakholdold = hold;
      }
      peakholdmillis = millis();
      MStatusold = MStatus;
    }
  }
}

void doSquelch() {
  tft.setFreeFont(FONT7);
  if (XDRGTKUSB == false && XDRGTKTCP == false) {
    Squelch = analogRead(PIN_POT) / 4 - 100;
    if (Squelch > 920) Squelch = 920;
    if (showsquelch) {
      if (seek == false && menu == false && Squelch != Squelchold) {
        tft.setTextColor(TFT_BLACK);
        if (Squelchold == -100) {
          tft.drawCentreString(myLanguage[language][33], 224, 164, GFXFF);
        } else if (Squelchold == 920) {
          tft.drawCentreString("ST", 224, 164, GFXFF);
        } else {
          tft.drawCentreString(String(Squelchold / 10), 224, 164, GFXFF);
        }
        tft.setTextColor(TFT_WHITE);
        if (Squelch == -100) {
          tft.drawCentreString(myLanguage[language][33], 224, 164, GFXFF);
        } else if (Squelch == 920) {
          tft.drawCentreString("ST", 224, 164, GFXFF);
        } else {
          tft.drawCentreString(String(Squelch / 10), 224, 164, GFXFF);
        }
      }
      Squelchold = Squelch;
    }
  }
  if (seek == false && (XDRGTKUSB == true || XDRGTKTCP == true)) {
    if (XDRMute == false) {
      if (Squelch != -1) {
        if (Squelch < SStatus || Squelch == -100) {
          radio.setUnMute();
          SQ = false;
        } else {
          radio.setMute();
          SQ = true;
        }
      } else {
        if (Stereostatus == true) {
          radio.setUnMute();
          SQ = false;
        } else {
          radio.setMute();
          SQ = true;
        }
      }
      if (screenmute == false && showsquelch == true) {
        if (Squelch != Squelchold) {
          tft.setTextColor(TFT_BLACK);
          if (Squelchold == -1) tft.drawCentreString("ST", 224, 164, GFXFF); else tft.drawCentreString(String(Squelchold / 10), 224, 164, GFXFF);
          tft.setTextColor(TFT_WHITE);
          if (Squelch == -1) tft.drawCentreString("ST", 224, 164, GFXFF); else tft.drawCentreString(String(Squelch / 10), 224, 164, GFXFF);
          Squelchold = Squelch;
        }
      }
    }
  } else {
    if (seek == false && Squelch != 920) {
      if (Squelch < SStatus || Squelch == -100) {
        radio.setUnMute();
        SQ = false;
      } else {
        radio.setMute();
        SQ = true;
      }
    } else {
      if (seek == false && Stereostatus == true) {
        radio.setUnMute();
        SQ = false;
      } else {
        radio.setMute();
        SQ = true;
      }
    }
  }
}

void updateBW() {
  tft.setFreeFont(FONT7);
  if (BWset == 0) {
    if (screenmute == false) {
      tft.drawRoundRect(249, 35, 68, 20, 5, TFT_WHITE);
      tft.setTextColor(TFT_WHITE);
    }
    radio.setFMABandw();
  } else {
    if (screenmute == false) {
      tft.drawRoundRect(249, 35, 68, 20, 5, TFT_GREYOUT);
      tft.setTextColor(TFT_GREYOUT);
    }
  }
  tft.drawCentreString("AUTO BW", 282, 33, GFXFF);
}

void updateiMS() {
  tft.setFreeFont(FONT7);
  if (band == BAND_FM) {
    if (iMSset == 0) {
      if (screenmute == false) {
        tft.drawRoundRect(249, 56, 30, 20, 5, TFT_WHITE);
        tft.setTextColor(TFT_WHITE);
      }
      radio.setiMS(1);
    } else {
      if (screenmute == false) {
        tft.drawRoundRect(249, 56, 30, 20, 5, TFT_GREYOUT);
        tft.setTextColor(TFT_GREYOUT);
      }
      radio.setiMS(0);
    }
  } else {
    tft.setTextColor(TFT_GREYOUT);
  }
  tft.drawCentreString("iMS", 264, 54, GFXFF);
}

void updateEQ() {
  tft.setFreeFont(FONT7);
  if (band == BAND_FM) {
    if (EQset == 0) {
      if (screenmute == false) {
        tft.drawRoundRect(287, 56, 30, 20, 5, TFT_WHITE);
        tft.setTextColor(TFT_WHITE);
      }
      radio.setEQ(1);
    } else {
      if (screenmute == false) {
        tft.drawRoundRect(287, 56, 30, 20, 5, TFT_GREYOUT);
        tft.setTextColor(TFT_GREYOUT);
      }
      radio.setEQ(0);
    }
  } else {
    tft.setTextColor(TFT_GREYOUT);
  }
  tft.drawCentreString("EQ", 302, 54, GFXFF);
}

void updateSWMIBand() {
  switch (SWMIBandPos) {
    case SW_MI_BAND_11M:
    case SW_MI_BAND_13M:
    case SW_MI_BAND_15M:
    case SW_MI_BAND_16M:
    case SW_MI_BAND_19M:
    case SW_MI_BAND_22M:
    case SW_MI_BAND_25M:
    case SW_MI_BAND_31M:
    case SW_MI_BAND_41M:
    case SW_MI_BAND_49M:
    case SW_MI_BAND_60M:
    case SW_MI_BAND_75M:
    case SW_MI_BAND_90M:
    case SW_MI_BAND_120M:
    case SW_MI_BAND_160M:
      tft.setTextColor(TFT_BLACK);
      tft.drawString(SWMIBandstringold, 50, 45, GFXFF);

      tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
      tft.drawString(SWMIBandstring, 50, 45, GFXFF);
      break;
    case SW_MI_BAND_GAP:

      tft.setTextColor(TFT_BLACK);
      tft.drawString(SWMIBandstringold, 50, 45, GFXFF);

      break;
  }
}

void doBW() {
  if (band == BAND_FM) {
    if (BWset > 16) BWset = 0;

    switch (BWset) {
      case 1:
        radio.setFMBandw(56);
        break;

      case 2:
        radio.setFMBandw(64);
        break;

      case 3:
        radio.setFMBandw(72);
        break;

      case 4:
        radio.setFMBandw(84);
        break;

      case 5:
        radio.setFMBandw(97);
        break;

      case 6:
        radio.setFMBandw(114);
        break;

      case 7:
        radio.setFMBandw(133);
        break;

      case 8:
        radio.setFMBandw(151);
        break;

      case 9:
        radio.setFMBandw(168);
        break;

      case 10:
        radio.setFMBandw(184);
        break;

      case 11:
        radio.setFMBandw(200);
        break;

      case 12:
        radio.setFMBandw(217);
        break;

      case 13:
        radio.setFMBandw(236);
        break;

      case 14:
        radio.setFMBandw(254);
        break;

      case 15:
        radio.setFMBandw(287);
        break;

      case 16:
        radio.setFMBandw(311);
        break;
    }
  } else {
    if (BWset > 4) BWset = 1;

    switch (BWset) {
      case 1:
        radio.setAMBandw(3);
        break;

      case 2:
        radio.setAMBandw(4);
        break;

      case 3:
        radio.setAMBandw(6);
        break;

      case 4:
        radio.setAMBandw(8);
        break;
    }
  }
  updateBW();
  BWreset = true;
}

void doTuneMode() {
  switch (tunemode) {
    case TUNE_MAN:
      if (band == BAND_FM) {
        tunemode = TUNE_AUTO;
        if (stepsize != 0) {
          stepsize = 0;
          RoundStep();
          ShowStepSize();
        }
      } else if (band == BAND_SW && showSWMIBand) {
        tunemode = TUNE_MI_BAND;
      } else {
        tunemode = TUNE_MEM;
      }
      break;
    case TUNE_MI_BAND:
    case TUNE_AUTO:
      tunemode = TUNE_MEM;
      break;

    case TUNE_MEM:
      tunemode = TUNE_MAN;
      break;
  }
  ShowTuneMode();
  ShowFreq(0);
  EEPROM.writeByte(41, tunemode);
  EEPROM.commit();
}

void ShowTuneMode() {
  tft.setFreeFont(FONT7);
  switch (tunemode) {
    case TUNE_MAN:
      if (band == BAND_SW) {
        tft.drawRoundRect(3, 57, 40, 20, 5, TFT_GREYOUT);
        tft.setTextColor(TFT_GREYOUT);
        tft.drawCentreString("BAND", 22, 55, GFXFF);
      } else {
        tft.drawRoundRect(3, 57, 40, 20, 5, TFT_GREYOUT);
        tft.setTextColor(TFT_GREYOUT);
        tft.drawCentreString("AUTO", 22, 55, GFXFF);
      }

      tft.drawRoundRect(3, 35, 40, 20, 5, TFT_WHITE);
      tft.setTextColor(TFT_WHITE);
      tft.drawCentreString("MAN", 22, 33, GFXFF);

      tft.drawRoundRect(3, 79, 40, 20, 5, TFT_GREYOUT);
      tft.setTextColor(TFT_GREYOUT);
      tft.drawCentreString("MEM", 22, 77, GFXFF);
      break;

    case TUNE_AUTO:
      tft.drawRoundRect(3, 57, 40, 20, 5, TFT_WHITE);
      tft.setTextColor(TFT_WHITE);
      tft.drawCentreString("AUTO", 22, 55, GFXFF);

      tft.drawRoundRect(3, 35, 40, 20, 5, TFT_GREYOUT);
      tft.setTextColor(TFT_GREYOUT);
      tft.drawCentreString("MAN", 22, 33, GFXFF);

      tft.drawRoundRect(3, 79, 40, 20, 5, TFT_GREYOUT);
      tft.setTextColor(TFT_GREYOUT);
      tft.drawCentreString("MEM", 22, 77, GFXFF);
      break;

    case TUNE_MEM:
      if (band == BAND_SW) {
        tft.drawRoundRect(3, 57, 40, 20, 5, TFT_GREYOUT);
        tft.setTextColor(TFT_GREYOUT);
        tft.drawCentreString("BAND", 22, 55, GFXFF);
      } else {
        tft.drawRoundRect(3, 57, 40, 20, 5, TFT_GREYOUT);
        tft.setTextColor(TFT_GREYOUT);
        tft.drawCentreString("AUTO", 22, 55, GFXFF);
      }

      tft.drawRoundRect(3, 35, 40, 20, 5, TFT_GREYOUT);
      tft.setTextColor(TFT_GREYOUT);
      tft.drawCentreString("MAN", 22, 33, GFXFF);

      if (memorystore == true) {
        tft.drawRoundRect(3, 79, 40, 20, 5, TFT_RED);
        tft.setTextColor(TFT_RED);
      } else {
        tft.drawRoundRect(3, 79, 40, 20, 5, TFT_WHITE);
        tft.setTextColor(TFT_WHITE);
      }
      tft.drawCentreString("MEM", 22, 77, GFXFF);
      break;
    case TUNE_MI_BAND:
      tft.drawRoundRect(3, 57, 40, 20, 5, TFT_WHITE);
      tft.setTextColor(TFT_WHITE);
      tft.drawCentreString("BAND", 22, 55, GFXFF);

      tft.drawRoundRect(3, 35, 40, 20, 5, TFT_GREYOUT);
      tft.setTextColor(TFT_GREYOUT);
      tft.drawCentreString("MAN", 22, 33, GFXFF);

      tft.drawRoundRect(3, 79, 40, 20, 5, TFT_GREYOUT);
      tft.setTextColor(TFT_GREYOUT);
      tft.drawCentreString("MEM", 22, 77, GFXFF);
      break;
  }
}

void ShowRSSI() {
  if (wifi) rssi = WiFi.RSSI(); else rssi = 0;
  if (rssiold != rssi) {
    rssiold = rssi;
    if (rssi == 0) {
      tft.drawBitmap(272, 4, WiFi4, 25, 25, TFT_GREYOUT);
    } else if (rssi > -50 && rssi < 0) {
      tft.drawBitmap(272, 4, WiFi4, 25, 25, TFT_SKYBLUE);
    } else if (rssi > -60) {
      tft.drawBitmap(272, 4, WiFi4, 25, 25, TFT_GREYOUT);
      tft.drawBitmap(272, 4, WiFi3, 25, 25, TFT_SKYBLUE);
    } else if (rssi > -70) {
      tft.drawBitmap(272, 4, WiFi4, 25, 25, TFT_GREYOUT);
      tft.drawBitmap(272, 4, WiFi2, 25, 25, TFT_SKYBLUE);
    } else if (rssi < -70) {
      tft.drawBitmap(272, 4, WiFi4, 25, 25, TFT_GREYOUT);
      tft.drawBitmap(272, 4, WiFi1, 25, 25, TFT_SKYBLUE);
    }
  }
}

void ShowBattery() {
  if (analogRead(BATTERY_PIN) < 200) {
    if (batteryold != battery) {
      tft.drawRect(300, 8, 12, 20, TFT_GREYOUT);
      tft.fillRect(303, 4, 6, 4, TFT_GREYOUT);
      tft.fillRect(302, 10, 8, 16, TFT_BLACK);
      battery = 0;
      batteryold = battery;
    }
  } else {
    battery = map(constrain(analogRead(BATTERY_PIN), BAT_LEVEL_EMPTY, BAT_LEVEL_FULL), BAT_LEVEL_EMPTY, BAT_LEVEL_FULL, 0, BAT_LEVEL_STAGE);
    if (batteryold != battery) {
      tft.drawRect(300, 8, 12, 20, TFT_WHITE);
      tft.fillRect(303, 4, 6, 4, TFT_WHITE);
      tft.fillRect(302, 10, 8, 16, TFT_BLACK);
      tft.fillRect(302, 26 - (battery * 4), 8, battery * 4, TFT_GREEN);
      batteryold = battery;
    }
  }
}

void Communication() {
  if (menu == false) {
    if (wifi) {
      int packetSize = Udp.parsePacket();
      if (packetSize) {
        char packetBuffer[packetSize];
        Udp.read(packetBuffer, packetSize);
        Udp.endPacket();
        String packet = String(packetBuffer);
        if (packetBuffer == "from=StationList;freq=?;bandwidth=?") {
          ShowFreq(0);
          Udp.beginPacket(remoteip, 9030);
          Udp.print("from=TEF tuner;Bandwidth=");
          Udp.print(BW * 1000);
          Udp.endPacket();
        } else {
          int symPos = packet.indexOf("freq=");
          String stlfreq = packet.substring(symPos + 5, packetSize);
          if ((stlfreq.toInt()) / 10000 > 6500 && (stlfreq.toInt()) / 10000 < 10800) {
            if (band != BAND_FM) {
              band = BAND_FM;
              SelectBand();
            }
            frequency = (stlfreq.toInt()) / 10000;
            radio.SetFreq(frequency);
          }

          // To Do: AM
          //          if ((stlfreq.toInt()) / 1000 > 144 && (stlfreq.toInt()) / 1000 < 27000) {
          //            if (band != 5) {
          //              band = 5;
          //            SelectBand();
          //            }
          //            frequency5 = (stlfreq.toInt()) / 1000;
          //            radio.SetFreqAM(frequency5);
          //          }
          radio.clearRDS(fullsearchrds);
          ShowFreq(0);
          store = true;
        }
      }
    }

    if (Server.hasClient())
    {
      if (RemoteClient.connected())
      {
        Server.available().stop();
      } else {
        wificonnected = true;
        RemoteClient = Server.available();
        passwordcrypt();
        RemoteClient.print(saltkey + "\n");
      }
    } else {
      if (Server.hasClient()) Server.available().stop();
    }

    if (wificonnected == true && !RemoteClient.connected()) {
      wificonnected = false;
      RDSSPYTCP = false;
      XDRGTKTCP = false;
    }


    if (XDRGTKTCP == false && wificonnected == true && RemoteClient.available()) {
      String data_str = RemoteClient.readStringUntil('\n');
      int data = data_str.toInt();
      if (data_str.length() > 30 && data_str.equals(cryptedpassword))
      {
        radio.setFMABandw();
        if (band != BAND_FM) {
          band = BAND_FM;
          SelectBand();
        }
        XDRGTKTCP = true;
        RemoteClient.print("o1,0\n");
        store = true;
      } else if (RDSSPYTCP == false && XDRGTKTCP == false && data_str.length() < 5 && data_str == ("*R?F"))
      {
        RDSSPYTCP = true;
      } else if (RDSSPYTCP == true) {
        int symPos = data_str.indexOf("*F");
        if (symPos >= 5) {
          String freq = data_str.substring(0, symPos);
          freq = freq.substring(0, freq.length() - 1);
          frequency = freq.toInt();
          radio.SetFreq(frequency);
          radio.clearRDS(fullsearchrds);
          if (band != BAND_FM) {
            band = BAND_FM;
            SelectBand();
          }
          ShowFreq(0);
          store = true;
        }
      } else {
        RemoteClient.print("a0\n");
      }
    }

    if (XDRGTKUSB == false && Serial.available())
    {
      String data_str = Serial.readStringUntil('\n');
      int data = data_str.toInt();
      if (data_str.length() > 1 && data_str == ("*D*R?F")) RDSSPYUSB = true;
      int symPos = data_str.indexOf("*F");
      if (symPos >= 5) {
        String freq = data_str.substring(0, symPos);
        freq = freq.substring(0, freq.length() - 1);
        frequency = freq.toInt();
        radio.SetFreq(frequency);
        if (band != BAND_FM) {
          band = BAND_FM;
          BWreset = true;
          BWset = 0;
          radio.power(0);
          delay(50);
          radio.SetFreq(frequency);
          radio.clearRDS(fullsearchrds);
          freqold = frequency_AM;
          doBW;
          radio.getStatus(SStatus, USN, WAM, OStatus, BW, MStatus);
          if (screenmute == false) BuildDisplay();
        }
        ShowFreq(0);
        store = true;
      }
      if (data_str.charAt(0) == 'x') {
        radio.setFMABandw();
        if (band != BAND_FM) {
          band = BAND_FM;
          SelectBand();
        }
        Serial.print("OK\nT" + String(frequency * 10) + "\n");
        XDRGTKUSB = true;
        if (menu == true) ModeButtonPress();
        if (Squelch != Squelchold) {
          if (screenmute == false) {
            tft.setFreeFont(FONT7);
            tft.setTextColor(TFT_BLACK);
            if (Squelchold == -100) tft.drawCentreString(myLanguage[language][33], 224, 164, GFXFF); else if (Squelchold > 920) tft.drawCentreString("ST", 224, 164, GFXFF); else tft.drawCentreString(String(Squelchold / 10), 224, 164, GFXFF);
          }
        }
      }
    }
    if (XDRGTKUSB || XDRGTKTCP) XDRGTKRoutine();
  }
}


void DataPrint(String string) {
  if (XDRGTKUSB) Serial.print(string);
  if (XDRGTKTCP) RemoteClient.print(string);
}

void XDRGTKRoutine() {
  if (XDRGTKUSB) {
    if (Serial.available())
    {
      buff[buff_pos] = Serial.read();
      if (buff[buff_pos] != '\n' && buff_pos != 16 - 1)
      {
        buff_pos++;
      } else {
        buff[buff_pos] = 0;
        buff_pos = 0;
        XDRGTKdata = true;
      }
    }
  }

  if (XDRGTKTCP) {
    if (RemoteClient.available() > 0) {
      buff[buff_pos] = RemoteClient.read();
      if (buff[buff_pos] != '\n' && buff_pos != 16 - 1)
      {
        buff_pos++;
      } else {
        buff[buff_pos] = 0;
        buff_pos = 0;
        XDRGTKdata = true;
      }
    }
  }

  if (XDRGTKdata) {
    switch (buff[0])
    {
      case 'A':
        int AGC;
        AGC = atol(buff + 1);
        DataPrint("A" + String(AGC) + "\n");
        radio.setAGC(AGC);
        break;

      case 'C':
        byte scanmethod;
        scanmethod = atol(buff + 1);
        if (scanmethod == 1) {
          DataPrint("C1\n");
          direction = false;
          Seek(direction);
          ShowFreq(0);
        }
        if (scanmethod == 2) {
          DataPrint("C2\n");
          direction = true;
          Seek(direction);
          ShowFreq(0);
        }
        DataPrint("C0\n");
        break;

      case 'N':
        doStereoToggle();
        break;

      case 'D':
        DeEmphasis = atol(buff + 1);
        DataPrint("D" + String(DeEmphasis) + "\n");
        radio.setDeemphasis(DeEmphasis);
        break;

      case 'F':
        XDRBWset = atol(buff + 1);
        DataPrint("F" + String(XDRBWset) + "\n");
        if (XDRBWset < 0) {
          XDRBWsetold = XDRBWset;
          BWset = 0;
        } else if (XDRBWset < 16) {
          BWset = XDRBWset + 1;
          XDRBWsetold = XDRBWset;
        } else {
          XDRBWset = XDRBWsetold;
        }
        doBW();
        break;

      case 'G':
        LevelOffset =  atol(buff + 1);
        if (LevelOffset == 0) {
          MuteScreen(0);
          LowLevelSet = EEPROM.readInt(47);
          softmuteam = EEPROM.readByte(29);
          softmutefm = EEPROM.readByte(30);
          radio.setSoftmuteFM(softmutefm);
          radio.setSoftmuteAM(softmuteam);
          DataPrint("G00\n");
        }
        if (LevelOffset == 10) {
          MuteScreen(1);
          LowLevelSet = EEPROM.readInt(47);
          softmuteam = EEPROM.readByte(29);
          softmutefm = EEPROM.readByte(30);
          radio.setSoftmuteFM(softmutefm);
          radio.setSoftmuteAM(softmuteam);
          DataPrint("G10\n");
        }
        if (LevelOffset == 1) {
          MuteScreen(0);
          radio.setSoftmuteFM(1);
          radio.setSoftmuteAM(1);
          DataPrint("G01\n");
        }
        if (LevelOffset == 11) {
          MuteScreen(1);
          radio.setSoftmuteFM(1);
          radio.setSoftmuteAM(1);
          DataPrint("G11\n");
        }
        break;

      case 'M':
        byte XDRband;
        XDRband = atol(buff + 1);
        if (XDRband == 0) DataPrint("M0\n"); else DataPrint("M1\n");
        if (XDRband == 1) {
          if (frequency_AM >= LWLowEdgeSet && frequency_AM <= LWHighEdgeSet) {
            if (band != BAND_LW) {
              band = BAND_LW;
              SelectBand();
            }
          }
          if (frequency_AM >= MWLowEdgeSet && frequency_AM <= MWHighEdgeSet) {
            if (band != BAND_MW) {
              band = BAND_MW;
              SelectBand();
            }
          }
          if (frequency_AM >= SWLowEdgeSet && frequency_AM <= SWHighEdgeSet) {
            if (band != BAND_SW) {
              band = BAND_SW;
              SelectBand();
            }
          }
          radio.SetFreqAM(frequency_AM);
          DataPrint("M1\n");
          DataPrint("T" + String(frequency_AM) + "\n");
        } else {
          if (band != BAND_FM) {
            band = BAND_FM;
            SelectBand();
          }
          DataPrint("M0\n");
          DataPrint("T" + String(frequency * 10) + "\n");
          radio.SetFreq(frequency);
          radio.clearRDS(fullsearchrds);
          RDSstatus = 0;
        }
        store == true;
        break;

      case 'T':
        unsigned int freqtemp;
        freqtemp = atoi(buff + 1);
        if (seek == true) seek = false;
        if (freqtemp >= LWLowEdgeSet && freqtemp <= LWHighEdgeSet) {
          frequency_AM = freqtemp;
          if (band != BAND_LW) {
            band = BAND_LW;
            SelectBand();
          }
          radio.SetFreqAM(frequency_AM);
          DataPrint("M1\n");
        }
        if (freqtemp >= MWLowEdgeSet && freqtemp <= MWHighEdgeSet) {
          frequency_AM = freqtemp;
          if (band != BAND_MW) {
            band = BAND_MW;
            SelectBand();
          }
          radio.SetFreqAM(frequency_AM);
          DataPrint("M1\n");
        }
        if (freqtemp >= SWLowEdgeSet && freqtemp <= SWHighEdgeSet) {
          frequency_AM = freqtemp;
          if (band != BAND_SW) {
            band = BAND_SW;
            SelectBand();
          }
          radio.SetFreqAM(frequency_AM);
          DataPrint("M1\n");
        }
        if (freqtemp >= FREQ_FM_START && freqtemp < FREQ_FM_END) {
          frequency = freqtemp / 10;
          if (band != BAND_FM) {
            band = BAND_FM;
            SelectBand();
          }
          radio.SetFreq(frequency);
          DataPrint("M0\n");
        }
        if (band == BAND_FM) DataPrint("T" + String(frequency * 10) + "\n"); else DataPrint("T" + String(frequency_AM) + "\n");
        ShowFreq(0);
        radio.clearRDS(fullsearchrds);
        RDSstatus = 0;
        store == true;
        break;

      case 'Q':
        Squelch = atoi(buff + 1);
        if (Squelch == -1) {
          DataPrint("Q-1\n");
        } else {
          Squelch *= 10;
          DataPrint("Q\n");
          DataPrint(String(Squelch / 10));
        }
        break;

      case 'S':
        if (buff[1] == 'a') {
          scanner_start = (atol(buff + 2) + 5) / 10;
        } else if (buff[1] == 'b') {
          scanner_end = (atol(buff + 2) + 5) / 10;
        } else if (buff[1] == 'c') {
          scanner_step = (atol(buff + 2) + 5) / 10;
        } else if (buff[1] == 'f') {
          scanner_filter = atol(buff + 2);
        } else if (scanner_start > 0 && scanner_end > 0 && scanner_step > 0 && scanner_filter >= 0) {
          frequencyold = frequency;
          radio.SetFreq(scanner_start);
          DataPrint("U");
          if (scanner_filter < 0) {
            BWset = 0;
          } else if (scanner_filter == 0) {
            BWset = 1;
          } else if (scanner_filter == 26) {
            BWset = 2;
          } else if (scanner_filter == 1) {
            BWset = 3;
          } else if (scanner_filter == 28) {
            BWset = 4;
          } else if (scanner_filter == 29) {
            BWset = 5;
          } else if (scanner_filter == 3) {
            BWset = 6;
          } else if (scanner_filter == 4) {
            BWset = 7;
          } else if (scanner_filter == 5) {
            BWset = 8;
          } else if (scanner_filter == 7) {
            BWset = 9;
          } else if (scanner_filter == 8) {
            BWset = 10;
          } else if (scanner_filter == 9) {
            BWset = 11;
          } else if (scanner_filter == 10) {
            BWset = 12;
          } else if (scanner_filter == 11) {
            BWset = 13;
          } else if (scanner_filter == 12) {
            BWset = 14;
          } else if (scanner_filter == 13) {
            BWset = 15;
          } else if (scanner_filter == 15) {
            BWset = 16;
          }
          doBW();
          if (screenmute == false) {
            ShowFreq(1);
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.setCursor (90, 60);
            tft.setFreeFont(FONT14);
            tft.drawCentreString(myLanguage[language][34], 140, 60, GFXFF);
          }
          frequencyold = frequency / 10;
          for (freq_scan = scanner_start; freq_scan <= scanner_end; freq_scan += scanner_step) {
            radio.SetFreq(freq_scan);
            DataPrint(String(freq_scan * 10, DEC));
            DataPrint("=");
            delay(10);
            if (band == BAND_FM) radio.getStatus(SStatus, USN, WAM, OStatus, BW, MStatus); else  radio.getStatusAM(SStatus, USN, WAM, OStatus, BW, MStatus);
            DataPrint(String((SStatus / 10) + 10, DEC));
            DataPrint(",");
          }
          DataPrint("\n");
          if (screenmute == false) {
            tft.setTextColor(TFT_BLACK);
            tft.drawCentreString(myLanguage[language][34], 140, 60, GFXFF);
          }
          radio.SetFreq(frequencyold);
          ShowFreq(0);
          radio.setFMABandw();
          BWset = 0;
        }
        break;

      case 'Y':
        VolSet = atoi(buff + 1);
        if (VolSet == 0) {
          radio.setMute();
          XDRMute = true;
          SQ = true;
        } else {
          radio.setUnMute();
          radio.setVolume((VolSet - 40) / 10);
          XDRMute = false;
        }
        DataPrint("Y" + String(VolSet) + "\n");
        VolSet /= 10;
        break;

      case 'x':
        DataPrint("OK\nT" + String(frequency * 10) + "\n");
        break;

      case 'X':
        XDRGTKTCP = false;
        XDRGTKUSB = false;
        store = true;
        XDRMute = false;
        radio.setUnMute();
        VolSet = EEPROM.readInt(4);
        LowLevelSet = EEPROM.readInt(47);
        softmuteam = EEPROM.readByte(29);
        softmutefm = EEPROM.readByte(30);
        radio.setVolume(VolSet);
        radio.setSoftmuteFM(softmutefm);
        radio.setSoftmuteAM(softmuteam);
        if (screenmute) MuteScreen(0);
        break;

      case 'Z':
        byte iMSEQX;
        iMSEQX = atol(buff + 1);
        switch (iMSEQX) {
          case 0:
            iMSset = 1;
            EQset = 1;
            iMSEQ = 2;
            break;

          case 1:
            iMSset = 0;
            EQset = 1;
            iMSEQ = 3;
            break;

          case 2:
            iMSset = 1;
            EQset = 0;
            iMSEQ = 4;
            break;

          case 3:
            iMSset = 0;
            EQset = 0;
            iMSEQ = 1;
            break;
        }
        updateiMS();
        updateEQ();
        DataPrint("Z" + String(iMSEQX) + "\n");
        break;
    }
    XDRGTKdata = false;
  }

  if (band != BAND_FM) {
    DataPrint("Sm");
  } else {
    if (StereoToggle == false) {
      DataPrint("SS");
    } else if (Stereostatus == true) {
      DataPrint("Ss");
    } else {
      DataPrint("Sm");
    }
  }
  DataPrint(String(((SStatus * 100) + 10875) / 1000) + "." + String(((SStatus * 100) + 10875) / 100 % 10) + "," + String(WAM / 10) + "," + String(SNR) + "\n");
}

void TuneUp() {
  unsigned int temp;
  if (stepsize == 0) {
    if (band != BAND_FM) {
      if (frequency_AM < MWHighEdgeSet) {
        if (region == 0) {
          temp = FREQ_MW_STEP_9K;
          frequency_AM = (frequency_AM / FREQ_MW_STEP_9K) * FREQ_MW_STEP_9K;
        } else if (region == 1) {
          temp = FREQ_MW_STEP_10K;
          frequency_AM = (frequency_AM / FREQ_MW_STEP_10K) * FREQ_MW_STEP_10K;
        }
      } else {
        temp = FREQ_SW_STEP_5K;
        frequency_AM = (frequency_AM / FREQ_SW_STEP_5K) * FREQ_SW_STEP_5K;
      }
    } else {
      temp = 5;
    }
  }
  if (stepsize == 1) temp = 1;
  if (stepsize == 2) temp = 10;
  if (stepsize == 3) temp = 100;
  if (stepsize == 4) temp = 1000;

  if (band == BAND_FM) {
    frequency += temp;
    if (frequency >= (HighEdgeSet * 10) + 1) {
      frequency = LowEdgeSet * 10;
      if (edgebeep) EdgeBeeper();
    }
    radio.SetFreq(frequency);
  }

  if (band == BAND_LW) {
    frequency_AM += temp;
    if (frequency_AM > LWHighEdgeSet) {
      frequency_AM = LWLowEdgeSet;
      if (edgebeep) EdgeBeeper();
    }
    radio.SetFreqAM(frequency_AM);
    frequency_LW = frequency_AM;
  } else if (band == BAND_MW) {
    frequency_AM += temp;
    if (frequency_AM > MWHighEdgeSet) {
      frequency_AM = MWLowEdgeSet;
      if (edgebeep) EdgeBeeper();
    }
    radio.SetFreqAM(frequency_AM);
    frequency_MW = frequency_AM;
  } else if (band == BAND_SW) {
    frequency_AM += temp;
    if (frequency_AM > SWHighEdgeSet) {
      frequency_AM = SWLowEdgeSet;
      if (edgebeep) EdgeBeeper();
    }
    radio.SetFreqAM(frequency_AM);
    frequency_SW = frequency_AM;
  }
  radio.clearRDS(fullsearchrds);
  if (RDSSPYUSB == true) Serial.print("G:\r\nRESET-------\r\n\r\n");
  if (RDSSPYTCP == true) RemoteClient.print("G:\r\nRESET-------\r\n\r\n");
}

void TuneDown() {
  unsigned int temp;
  if (stepsize == 0) {
    if (band != BAND_FM) {
      if (frequency_AM <= MWHighEdgeSet) {
        if (frequency_AM == 2000) { // Fix Me :take care of 9K/10K Step
          frequency_AM = 1998;
          temp = 0;
        } else {
          temp = region == 0 ? FREQ_MW_STEP_9K : FREQ_MW_STEP_10K;
          frequency_AM = (frequency_AM / temp) * temp;
        }
      } else {
        temp = FREQ_SW_STEP_5K;
        frequency_AM = (frequency_AM / temp) * temp;
      }
    } else {
      temp = 5;
    }
  }
  if (stepsize == 1) temp = 1;
  if (stepsize == 2) temp = 10;
  if (stepsize == 3) temp = 100;
  if (stepsize == 4) temp = 1000;

  if (band == BAND_FM) {
    frequency -= temp;
    if (frequency < LowEdgeSet * 10) {
      frequency = HighEdgeSet * 10;
      if (edgebeep) EdgeBeeper();
    }
    radio.SetFreq(frequency);
  }

  if (band == BAND_LW) {
    frequency_AM -= temp;
    if (frequency_AM < LWLowEdgeSet) {
      frequency_AM = LWHighEdgeSet;
      if (edgebeep) EdgeBeeper();
    }
    radio.SetFreqAM(frequency_AM);
    frequency_LW = frequency_AM;
  } else if (band == BAND_MW) {
    frequency_AM -= temp;
    if (frequency_AM < MWLowEdgeSet) {
      frequency_AM = MWHighEdgeSet;
      if (edgebeep) EdgeBeeper();
    }
    radio.SetFreqAM(frequency_AM);
    frequency_MW = frequency_AM;
  } else if (band == BAND_SW) {
    frequency_AM -= temp;
    if (frequency_AM < SWLowEdgeSet) {
      frequency_AM = SWHighEdgeSet;
      if (edgebeep) EdgeBeeper();
    }
    radio.SetFreqAM(frequency_AM);
    frequency_SW = frequency_AM;
  }
  radio.clearRDS(fullsearchrds);
  if (RDSSPYUSB == true) Serial.print("G:\r\nRESET-------\r\n\r\n");
  if (RDSSPYTCP == true) RemoteClient.print("G:\r\nRESET-------\r\n\r\n");
}

void EdgeBeeper() {
  bool x = radio.mute;
  radio.tone(50, -5, 2000);
  if (x) radio.setMute(); else radio.setUnMute();
}

void Seek(bool mode) {
  if (band == BAND_FM) {
    radio.setMute();
    if (mode == false) TuneDown(); else TuneUp();
    delay(50);
    ShowFreq(0);
      if (XDRGTKUSB == true || XDRGTKTCP == true) if (band == BAND_FM) DataPrint("T" + String(frequency * 10) + "\n"); else DataPrint("T" + String(frequency_AM) + "\n");
    radio.getStatus(SStatus, USN, WAM, OStatus, BW, MStatus);

    if ((USN < 200) && (WAM < 230) && (OStatus < 80 && OStatus > -80) && (Squelch < SStatus || Squelch == 920)) {
      seek = false;
      radio.setUnMute();
      store = true;
    } else {
      seek = true;
      if (RDSSPYUSB == true) Serial.print("G:\r\nRESET-------\r\n\r\n");
      if (RDSSPYTCP == true) RemoteClient.print("G:\r\nRESET-------\r\n\r\n");
    }
  }
}

void SetTunerPatch() {
  if (TEF != 101 && TEF != 102 && TEF != 205) {
    radio.init(102);
    uint16_t device;
    uint16_t hw;
    uint16_t sw;
    radio.getIdentification(device, hw, sw);
    TEF = highByte(hw) * 100 + highByte(sw);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setFreeFont(FONT14);
    analogWrite(CONTRASTPIN, ContrastSet * 2 + 27);

    if (TEF == 0) tft.drawCentreString(myLanguage[language][35], 150, 70, GFXFF); else tft.drawCentreString(String(myLanguage[language][36]) + String(TEF), 150, 70, GFXFF);
    tft.drawCentreString(myLanguage[language][37], 150, 100, GFXFF);
    EEPROM.writeByte(37, TEF);
    EEPROM.commit();
    while (true);
    for (;;);
  }
}

void read_encoder() {
  static uint8_t old_AB = 3;
  static int8_t encval = 0;
  static const int8_t enc_states[]  = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

  old_AB <<= 2;

  if (digitalRead(ROTARY_PIN_A)) old_AB |= 0x02;
  if (digitalRead(ROTARY_PIN_B)) old_AB |= 0x01;
  encval += enc_states[( old_AB & 0x0f )];

  if (optenc == 1) {
    if (encval > 2) {
      if (rotarymode == true) rotary = -1; else rotary = 1;
      encval = 0;
    } else if (encval < -2) {
      if (rotarymode == true) rotary = 1; else rotary = -1;
      encval = 0;
    }
  } else {
    if (encval > 3) {
      if (rotarymode == true) rotary = -1; else rotary = 1;
      encval = 0;
    } else if (encval < -3) {
      if (rotarymode == true) rotary = 1; else rotary = -1;
      encval = 0;
    }
  }
}

void tryWiFi() {
  tft.drawRoundRect(1, 60, 319, 140, 5, TFT_WHITE);
  tft.fillRoundRect(3, 62, 315, 136, 5, TFT_BLACK);
  tft.setFreeFont(FONT14);
  tft.setTextColor(TFT_WHITE);
  tft.drawCentreString(myLanguage[language][55], 155, 80, GFXFF);
  if (wc.autoConnect()) {
    Server.begin();
    Udp.begin(9031);
    remoteip = IPAddress (WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], subnetclient);
    tft.setTextColor(TFT_GREEN);
    tft.drawCentreString(myLanguage[language][57], 155, 120, GFXFF);
    wifi = true;
  } else {
    Server.end();
    Udp.stop();
    tft.setTextColor(TFT_RED);
    tft.drawCentreString(myLanguage[language][56], 155, 120, GFXFF);
    wifi = false;
    XDRGTKTCP = false;
    RDSSPYTCP = false;
  }
}

void passwordcrypt() {
  int generated = 0;
  while (generated < 16)
  {
    byte randomValue = random(0, 26);
    char letter = randomValue + 'a';
    if (randomValue > 26) letter = (randomValue - 26);
    saltkey.setCharAt(generated, letter);
    generated ++;
  }
  salt = saltkey + XDRGTK_key;
  cryptedpassword = String(sha1(salt));
}

void DefaultSettings() {
  EEPROM.writeByte(43, 27);
  EEPROM.writeUInt(0, 10000);
  EEPROM.writeInt(4, 0);
  EEPROM.writeUInt(8, 0);
  EEPROM.writeUInt(12, 875);
  EEPROM.writeUInt(16, 1080);
  EEPROM.writeByte(20, 50);
  EEPROM.writeByte(21, 0);
  EEPROM.writeByte(22, 70);
  EEPROM.writeByte(23, 0);
  EEPROM.writeInt(24, 0);
  EEPROM.writeByte(28, 0);
  EEPROM.writeByte(29, 0);
  EEPROM.writeByte(30, 0);
  EEPROM.writeUInt(31, 828);
  EEPROM.writeByte(35, 0);
  EEPROM.writeByte(36, 1);
  EEPROM.writeByte(37, 0);
  EEPROM.writeByte(38, 0);
  EEPROM.writeByte(39, 0);
  EEPROM.writeByte(40, 0);
  EEPROM.writeByte(41, 0);
  EEPROM.writeByte(42, 0);
  EEPROM.writeByte(44, 1);
  EEPROM.writeByte(45, 1);
  EEPROM.writeByte(46, 0);
  EEPROM.writeInt(47, -10);
  EEPROM.writeByte(51, 0);
  EEPROM.writeByte(52, 0);
  EEPROM.writeByte(53, 0);
  EEPROM.writeByte(54, 0);
  EEPROM.writeByte(55, 0);
  EEPROM.writeByte(56, 1);
  EEPROM.writeByte(57, 1);
  EEPROM.writeByte(58, 1);
  EEPROM.writeByte(59, 0);
  for (int i = 0; i < 30; i++) EEPROM.writeByte(i + 60, 0);
  for (int i = 0; i < 30; i++) EEPROM.writeUInt((i * 4) + 100, 8750);
  EEPROM.writeUInt(221, 180);
  EEPROM.writeUInt(225, 540);
  EEPROM.writeUInt(229, 1800);
  EEPROM.writeString(233, "password");
  EEPROM.writeByte(244, 1);
  EEPROM.writeByte(245, 1);
  EEPROM.writeByte(246, 0);
  EEPROM.writeByte(247, 0);
  EEPROM.writeByte(248, 0);
  EEPROM.commit();
}
