#ifndef GUI_H
#define GUI_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "TEF6686.h"
#include "WiFiConnect.h"
#include "WiFiConnectParam.h"

#define SMETERPIN       27
#define CONTRASTPIN     2

#define ITEM_GAP        20
#define ITEM1           30
#define ITEM2           50
#define ITEM3           70
#define ITEM4           90
#define ITEM5           110
#define ITEM6           130
#define ITEM7           150
#define ITEM8           170
#define ITEM9           190
#define ITEM10          210

enum menupage {INDEX, MAINSETTINGS, AUDIOSETTINGS, DISPLAYSETTINGS, RDSSETTINGS, FMSETTINGS, AMSETTINGS, CONNECTIVITY, DXMODE};

extern byte menuitem;
extern byte items[9];

extern bool advancedRDS;
extern bool afmethodBold;
extern bool afpage;
extern bool afscreen;
extern bool artheadold;
extern bool autosquelch;
extern bool BWreset;
extern bool change;
extern bool compressedold;
extern bool dropout;
extern bool dynamicPTYold;
extern bool dynamicspi;
extern bool edgebeep;
extern bool fmsi;
extern bool fullsearchrds;
extern bool hasafold;
extern bool haseonold;
extern bool hasrtplusold;
extern bool hastmcold;
extern bool LowLevelInit;
extern bool menu;
extern bool menuopen;
extern bool mwstepsize;
extern bool rdsreset;
extern bool RDSstatus;
extern bool RDSstatusold;
extern bool rdsstereoold;
extern bool usesquelch;
extern bool scandxmode;
extern bool scanmem;
extern bool scanmute;
extern bool softmuteam;
extern bool softmutefm;
extern bool Stereostatusold;
extern bool StereoToggle;
extern bool TAold;
extern bool TPold;
extern bool USBmode;
extern bool XDRGTKMuteScreen;
extern bool wifi;
extern byte af;
extern byte af_counterold;
extern byte afpagenr;
extern byte amagc;
extern byte amnb;
extern byte amscansens;
extern byte audiomode;
extern byte band;
extern byte bandAM;
extern byte bandFM;
extern byte bandforbidden;
extern byte batteryold;
extern byte batteryoptions;
extern byte ContrastSet;
extern byte CurrentSkin;
extern byte CurrentTheme;
extern byte ECCold;
extern byte fmagc;
extern byte fmscansens;
extern byte fmdefaultstepsize;
extern byte fmnb;
extern byte fmdeemphasis;
extern byte freqfont;
extern byte amcodect;
extern byte amcodectcount;
extern byte amgain;
extern byte hardwaremodel;
extern byte HighCutLevel;
extern byte HighCutOffset;
extern byte language;
extern byte licold;
extern byte memorypos;
extern byte menupage;
extern byte MSold;
extern byte poweroptions;
extern byte eonptyold[20];
extern byte rdsblockold;
extern byte region;
extern byte scancancel;
extern byte scanstart;
extern byte scanstop;
extern byte scanhold;
extern byte scanmodeold;
extern byte screensaverOptions[5];
extern byte screensaverset;
extern byte showmodulation;
extern byte showSWMIBand;
extern byte showrdserrors;
extern byte stationlistid;
extern int8_t CNold;
extern byte StereoLevel;
extern byte StereoLevel;
extern byte submenu;
extern byte subnetclient;
extern byte tot;
extern byte touchrotating;
extern byte tunemode;
extern byte unit;
extern byte spispeed;
extern char eonpicodeold[20][6];
extern char programTypePrevious[18];
extern float batteryVold;
extern float vPerold;
extern int ActiveColor;
extern int ActiveColorSmooth;
extern int AMLevelOffset;
extern int BackgroundColor;
extern int BackgroundColor1;
extern int BackgroundColor2;
extern int BackgroundColor3;
extern int BackgroundColor4;
extern int BackgroundColor5;
extern int BarInsignificantColor;
extern int BarSignificantColor;
extern int BatteryValueColor;
extern int BatteryValueColorSmooth;
extern int BWAutoColor;
extern int BWAutoColorSmooth;
extern int FrameColor;
extern int FreqColor;
extern int FreqColorSmooth;
extern int GreyoutColor;
extern int InsignificantColor;
extern int InsignificantColorSmooth;
extern int menuoption;
extern int ModBarInsignificantColor;
extern int ModBarSignificantColor;
extern int PrimaryColor;
extern int PrimaryColorSmooth;
extern int RDSColor;
extern int RDSColorSmooth;
extern int RDSDropoutColor;
extern int RDSDropoutColorSmooth;
extern int rssiold;
extern int SecondaryColor;
extern int SecondaryColorSmooth;
extern int SignificantColor;
extern int SignificantColorSmooth;
extern int Squelchold;
extern int SStatusold;
extern int StereoColor;
extern int StereoColorSmooth;
extern int xPos;
extern int xPos2;
extern int xPos3;
extern int xPos4;
extern int16_t SStatus;
extern int8_t LevelOffset;
extern int8_t LowLevelSet;
extern int8_t VolSet;
extern String eonpsold[20];
extern String PIold;
extern String pinstringold;
extern String PSold;
extern String ptynold;
extern String rds_clockold;
extern String stationIDold;
extern String stationStateold;
extern String XDRGTK_key;
extern unsigned int ConverterSet;
extern unsigned int HighEdgeSet;
extern unsigned int LowEdgeSet;
extern unsigned int mappedfreqold[20];
extern unsigned int mappedfreqold2[20];
extern unsigned int mappedfreqold3[20];
extern unsigned long scantimer;

extern TFT_eSPI tft;
extern TEF6686 radio;
extern WiFiConnect wc;
extern TFT_eSprite MenuInfobox;
extern TFT_eSprite FullLineSprite;
extern TFT_eSprite OneBigLineSprite;

void BuildAFScreen();
void BuildMenu();
void BuildAdvancedRDS();
void BuildDisplay();
void MenuUp();
void MenuDown();
void DoMenu();
void doTheme();
void Infoboxprint(const char* input);
String removeNewline(String inputString);

extern void ShowFreq(int mode);
extern void ShowBandSelectionFM(bool notglanceview, bool normaldisplay);
extern void ShowBandSelectionAM(bool notglanceview, bool normaldisplay);
extern void ScreensaverTimerSet(byte value);
extern void ShowMemoryPos();
extern void ShowTuneMode();
extern void updateBW();
extern void ShowStepSize();
extern void updateiMS();
extern void updateEQ();
extern void doTheme();
extern void tryWiFi();
extern void TuneUp();
extern void endMenu();
extern void startFMDXScan();
extern void DoMemoryPosTune();
extern void UpdateFonts(bool mode);
extern void tftPrint(int8_t offset, const String & text, int16_t x, int16_t y, int color, int smoothcolor, uint8_t fontsize);
extern void setAutoSpeedSPI();
extern void showAutoSquelch(bool mode);
#endif