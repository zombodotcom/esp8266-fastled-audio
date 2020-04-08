#define FASTLED_INTERRUPT_RETRY_COUNT 0
//#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
FASTLED_USING_NAMESPACE

extern "C" {
  #include "user_interface.h"
}
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WebSocketsServer.h>
#include <FS.h>
#include <EEPROM.h>
#include "GradientPalettes.h"

#define ARRAY_SIZE(A) (sizeof(A)/sizeof((A)[0]))

#define HOSTNAME "Evo-"
// Change apMode to true if you don't have a router
const bool      apMode = false;
const char WiFiAPPSK[] = "yourAPpasswordHERE";
// Change ssid and password to match your own network
const char*       ssid = "This";
const char*   password = "JorTor32";

ESP8266WebServer webServer(80);
WebSocketsServer webSocketsServer = WebSocketsServer(81);
ESP8266HTTPUpdateServer httpUpdateServer;

#include "Field.h"
#include "FSBrowser.h"

#define NUM_LEDS    300
#define NUM_STRIPS    1
#define MILLI_AMPS 5000

uint8_t edges[26];
uint8_t x[NUM_LEDS];
uint8_t y[NUM_LEDS];

CRGB leds[NUM_STRIPS * NUM_LEDS];
// Snapshot
CRGB last[NUM_LEDS];

// Pallete Mode
bool pmd = true;  // PaletteMode[Picker|AUTO]
// Fade Style
bool bfd = true;  // Bass Fade [ON|OFF] (Whites)
bool tfd = true;  // Treb Fade [ON|OFF] (Colors)
// Flow Modes
bool hup = true;  // HueUpdat   [treb|bass]
bool hmd = false; // Hue Mode [hueSpd|rgbrate]
bool bmd = true;  // Bass Mode[Whites|Flash]
bool fmd = true;  // FlashMode   [RGB|FULL]
bool dir = true;  // Hue Dir   [rgb++|rgb--]
// Mirror Styles
bool inv = true;  // Mir Dir    [pull|push]
bool mor = true;  // Morphin      [ON|OFF]
bool mrs = true;  // Morph Teeter-Totter
// Snapshot
bool sv1 = true;
bool sv2 = true;
bool sv3 = true;
// Auto Play
bool apt = false; // Auto Play Pattern
bool apl = true;  // Auto Play Palette

uint8_t                  gHue =   0;
uint8_t                oldHue =   0;
uint8_t                newHue =   0;
uint8_t                  huey =   0;
uint8_t               rgbRate =   0;
uint8_t                morphs =   0;

uint8_t                 power =   1;
uint8_t            brightness = 255;
uint8_t                 speed =   2;
uint8_t                hueSpd =   2;
uint8_t                  glit =  25;
uint8_t               cooling =  49;
uint8_t              sparking =  60;
uint8_t               twnkSpd =   4;
uint8_t               twnkDns =   3;

uint8_t            autoPatDur =  10;
uint8_t            autoPalDur =  10;
uint8_t     secondsPerPalette =  10;
unsigned long  autoPatTimeout =   0;
unsigned long  autoPalTimeout =   0;

#include "Palettes.h"
#include "Audio.h"
#include "AudioPatterns.h"
#include "BasicPatterns.h"
#include "Twinkles.h"

typedef void (*Pattern)();
typedef Pattern PatternList[];
typedef struct {
  Pattern pattern;
  String name;
} PatternAndName;
typedef PatternAndName PatternAndNameList[];

PatternAndNameList patterns = {
{     pauseAnimation, "Pause"           },
{         giantRuler, "Giant Ruler"     },
{        giantRuler2, "Giant Ruler 2"   },
{        byteColumns, "Byte Analysis"   },
{        peakColumns, "Peak Analysis"   },
{      audioAnalyzer, "AudioAnalyzer"   },

{            newFlow, "New Flow"        },
{          besinFlow, "Besin Flow"      },
{             plasma, "Plasma"          },
{           rainbowG, "Rainbow G"       },
{                 VU, "VU"              },
{           newFlash, "New Flash"       },
  
{             TLava1, "T Lava"          },
{             TLava2, "T Lava"          },
{             TLava3, "T Lava"          },
//{           lavaLamp, "Lava Lamp"       },
{              pride, "Pride"           },
{         colorWaves, "Color Waves"     },
{       drawTwinkles, "Twinkles"        },
{            rainbow, "Rainbow"         },
{     rainbowGlitter, "Rainbow Glitter" },
{       rainbowSolid, "Solid Rainbow"   },
{        solidBreath, "Solid Breath"    },
{           confetti, "Confetti"        },
{            sinelon, "Sinelon"         },
{                bpm, "Beat"            },
{             juggle, "Juggle"          },
{             flames, "Flames"          },
{     showSolidColor, "Solid Color"     }
};

const uint8_t patternCount = ARRAY_SIZE(patterns);

void dimAll(byte value){
  for(int i=0; i<NUM_LEDS; i++)
    leds[i].nscale8(value);
}

#include "Fields.h"
#include "settings.h"
void setup(){
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  //Serial.begin(115200);
  delay(100);
  //Serial.setDebugOutput(true);
  initializeAudio();
  FastLED.addLeds<WS2812B, 6, GRB>(leds, 300);
  //FastLED.addLeds<WS2812B, 7, GRB>(leds, 300);
  FastLED.addLeds<WS2812B, 5, GRB>(leds, 264);
  FastLED.addLeds<WS2811,  8, RGB>(leds, 150);
  
  // ESP8266 Pin Order = 6,7,5,8
  //FastLED.addLeds<WS2811_PORTA,NUM_STRIPS>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setDither(false);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(brightness);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  #include "web.h"
  for(byte b=0; b<=25; ++b)
    edges[b] = b==0? NUM_LEDS-1: NUM_LEDS/b-1;
  for(uint16_t i=0; i<NUM_LEDS; i++){
    uint8_t angle = (i*256)/NUM_LEDS;
    x[i] = cos8(angle);
    y[i] = sin8(angle);
  }
}

void sendInt(uint8_t value){ sendString(String(value)); }
void sendString(String value){ webServer.send(200, "text/plain", value); }
void broadcastInt(String name, uint8_t value){
  String json = "{\"name\":\"" + name + "\",\"value\":" + String(value) + "}";
  webSocketsServer.broadcastTXT(json);
}
void broadcastString(String name, String value){
  String json = "{\"name\":\"" + name + "\",\"value\":\"" + String(value) + "\"}";
  webSocketsServer.broadcastTXT(json);
}

void loop(){
  webSocketsServer.loop();
  webServer.handleClient();
  if(power==0){
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    return;
  }
  currentMillis = millis();
  if(currentMillis-audioMillis>AUDIODELAY){
    audioMillis = currentMillis;
    readAudio();
  }
  random16_add_entropy(analogRead(MSGEQ7_AUDIO_PIN));
  EVERY_N_SECONDS(secondsPerPalette){
    gCurrentPaletteNumber = addmod8(gCurrentPaletteNumber, 1, gGradientPaletteCount);
    gTargetPalette        = gGradientPalettes[gCurrentPaletteNumber];
  }
  EVERY_N_MILLISECONDS(50){ dir? gHue+=hueSpd: gHue-=hueSpd; }
  EVERY_N_MILLISECONDS(50){ nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 8); }
  
  if(apt&&(millis()>autoPatTimeout)){
    adjustPat();
    autoPatTimeout = millis()+(autoPatDur*1000);
  }
  if(apl&&(millis()>autoPalTimeout)){
    adjustPal();
    autoPalTimeout = millis()+(autoPalDur*1000);
  }
  patterns[currPatIdx].pattern();
  FastLED.show();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
  switch (type){
    case WStype_DISCONNECTED:
      break;

    case WStype_CONNECTED:{
        IPAddress ip = webSocketsServer.remoteIP(num);
      }
      break;

    case WStype_TEXT:
      break;

    case WStype_BIN:
      hexdump(payload, length);
      break;
  }
}

void loadSettings(){
  power = EEPROM.read(0);
  brightness = EEPROM.read(1);
  currPatIdx = EEPROM.read(2);
  if(currPatIdx>=patternCount)
    currPatIdx = patternCount-1;
  apt = EEPROM.read(3);
  autoPatDur = EEPROM.read(4);
  currPalIdx = EEPROM.read(5);
  if(currPalIdx>=paletteCount)
    currPalIdx = paletteCount-1;
  apl = EEPROM.read(6);
  autoPalDur = EEPROM.read(7);
  speed = EEPROM.read(8);
  hueSpd = EEPROM.read(9);
  glit = EEPROM.read(10);
  pmd = EEPROM.read(11);
  dir = EEPROM.read(12);
  inv = EEPROM.read(13);
  bfd = EEPROM.read(14);
  tfd = EEPROM.read(15);
  hup = EEPROM.read(16);
  hmd = EEPROM.read(17);
  bmd = EEPROM.read(18);
  fmd = EEPROM.read(19);
  mor = EEPROM.read(20);
  byte r = EEPROM.read(21);
  byte g = EEPROM.read(22);
  byte b = EEPROM.read(23);
  if(r==0&&g==0&&b==0){}
  else solidColor = CRGB(r, g, b);
  cooling = EEPROM.read(24);
  sparking = EEPROM.read(25);
  twnkSpd = EEPROM.read(26);
  twnkDns = EEPROM.read(27);
}
