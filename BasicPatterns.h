/*
   ESP8266 + FastLED + Audio: https://github.com/jasoncoon/esp8266-fastled-audio
   Copyright (C) 2015-2017 Jason Coon
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

void pauseAnimation(){}
void giantRuler(){
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  for(uint16_t k=0; k<NUM_LEDS; ++k)
    if(k%10==9) leds[k] = CHSV(80, 255, 100);
}

void giantRuler2(){
  CRGB c;
  if(rgbRate==255) rgbRate = 0;
  c = CHSV(rgbRate,255,255);
  rgbRate+=rgbRate==250 ? 5:10;
  for(uint16_t k=0; k<NUM_LEDS; ++k)
    if(k%10==9) leds[k] = c;
  leds[0] = c;
}

void showSolidColor(){ fill_solid(leds, NUM_LEDS, solidColor); }

void rainbow(){
  if(hueSpd==0 && sv1){
    sv1 = !sv1;
    oldHue = gHue;
  } else if(hueSpd>0 && !sv1){
    sv1 = !sv1;
    gHue = oldHue;
  }
  fill_rainbow(leds, NUM_LEDS, gHue, speed+1);
}

void rainbowGlitter(){
  rainbow();
  addGlitter(glit*10-80);
}

void solidBreath(){
  int pos = beatsin16(speed,0,300);
  if(pos>255) pos = 255;
  fill_solid(leds, NUM_LEDS, CHSV(gHue, 255, pos));
  addGlitter(glit*10);
}
  
void rainbowSolid(){
  if(hueSpd==0 && !sv3){
    sv3 = !sv3;
    oldHue = newHue;
  }
  if(hueSpd>0 && sv3){
    sv3 = !sv3;
    gHue = oldHue;
    oldHue = 0;
  }
  newHue = gHue+oldHue;
  fill_solid(leds, NUM_LEDS, CHSV(newHue, 255, 255));
  addGlitter(glit*10);
}

void confetti(){
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += ColorFromPalette(pmd? palettes[currPalIdx]:gCurrentPalette, gHue + random8(64));
}

void sinelon(){
  fadeToBlackBy(leds, NUM_LEDS, 1+speed);
  int pos = beatsin16(speed*2, 0, NUM_LEDS-1);
  static int prevpos = 0;
  CRGB color = ColorFromPalette(pmd? palettes[currPalIdx]:gCurrentPalette, gHue, 255);
  if(pos<prevpos)
    fill_solid(leds+pos, (prevpos-pos)+1, color);
  else
    fill_solid(leds+prevpos, (pos-prevpos)+1, color);
  prevpos = pos;
}

void bpm(){
  uint8_t beat = beatsin8(glit, 64, 255);
  for(int i=0; i<NUM_LEDS; i++)
    leds[i] = ColorFromPalette(pmd? palettes[currPalIdx]:gCurrentPalette, gHue+(i*2), beat-gHue+(i*10));
}

void juggle(){
  fadeToBlackBy(leds, NUM_LEDS, 20);
  byte dothue = 0;
  for(int i=0; i<8; i++){
    leds[beatsin16(i+speed, 0, NUM_LEDS-1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void pride(){
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);
  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;
  for(uint16_t i=0 ; i<NUM_LEDS; i++){
    hue16 += hueinc16;
    uint8_t hue8 = hue16/256;
    brightnesstheta16 += brightnessthetainc16;
    uint16_t b16 = sin16(brightnesstheta16)+32768;
    uint16_t bri16 = (uint32_t)((uint32_t)b16*(uint32_t)b16)/65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16)*brightdepth)/65536;
    bri8 += (255 - brightdepth);
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS - 1) - pixelnumber;
    nblend(leds[pixelnumber], newcolor, 64);
  }
}

void heatMap(CRGBPalette16 palette, bool up){
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  static byte heat[NUM_LEDS];
  byte colorindex;
  for(uint16_t i=0; i<NUM_LEDS; i++)
    heat[i] = qsub8(heat[i], random8(0, ((cooling*10)/NUM_LEDS)+2));
  for(uint16_t k=NUM_LEDS-1; k>=2; k--)
    heat[k] = (heat[k-1]+heat[k-2]+heat[k-2])/3;
  if(random8()<sparking){
    int y = random8(7);
    heat[y] = qadd8(heat[y], random8(160, 255));
  }
  for(uint16_t j=0; j<NUM_LEDS; j++){
    colorindex = scale8(heat[j], 190);
    CRGB color = ColorFromPalette(palette, colorindex);
    if(up)
      leds[j] = color;
    else
      leds[(NUM_LEDS - 1) - j] = color;
  }
}

void flames(){ heatMap(pmd? palettes[currPalIdx]:gCurrentPalette, dir); }

extern const TProgmemRGBGradientPalettePtr gGradientPalettes[];
extern const uint8_t gGradientPaletteCount;

uint8_t beatsaw8(accum88 beats_per_minute, uint8_t lowest = 0, uint8_t highest = 255,
                  uint32_t timebase = 0, uint8_t phase_offset = 0)
{
  uint8_t beat = beat8( beats_per_minute, timebase);
  uint8_t beatsaw = beat + phase_offset;
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8( beatsaw, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}

void colorWaves(){
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);
  uint16_t hue16 = sHue16;
  uint16_t hueinc16 = beatsin88(113, 300, 1500);
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;
  for(uint16_t i=0 ; i<NUM_LEDS; i++){
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;
    uint16_t h16_128 = hue16 >> 7;
    if(h16_128 & 0x100)
      hue8 = 255 - (h16_128 >> 1);
    else
      hue8 = h16_128 >> 1;
    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16(brightnesstheta16  ) + 32768;
    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    uint8_t index = hue8;
    index = scale8(index, 240);
    CRGB newcolor = ColorFromPalette(pmd? palettes[currPalIdx]:gCurrentPalette, index, bri8);
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS - 1) - pixelnumber;
    nblend(leds[pixelnumber], newcolor, 128);
  }
}

void fillnoise8(){
  uint8_t spd = map(speed,0,25,0,50);
  uint8_t scl = map(hueSpd,0,25,0,50);
  uint8_t dataSmoothing = 0;
  if(spd<50) dataSmoothing = 200 - (spd * 4);
  for(int i=0; i<MAX_DIMENSION; i++){
    int ioffset = scl * i;
    for(int j=0; j<MAX_DIMENSION; j++){
      int joffset = scl * j;
      uint8_t data = inoise8(x + ioffset,y + joffset,z);
      data = qsub8(data,16);
      data = qadd8(data,scale8(data,39));
      if(dataSmoothing){
        uint8_t olddata = noise[i][j];
        uint8_t newdata = scale8(olddata, dataSmoothing) + scale8(data, 256 - dataSmoothing);
        data = newdata;
      }
      noise[i][j] = data;
    }
  }
  z += spd;
  x += spd / 8;
  y -= spd / 16;
}

uint16_t XY(uint8_t x, uint8_t y){
  uint16_t i;
  if(kMatrixSerpentineLayout == false)
    i = (y * kMatrixWidth) + x;
  if(kMatrixSerpentineLayout == true){
    if(y & 0x01) {
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    } else {
      i = (y * kMatrixWidth) + x;
    }
  }
  return i;
}

void mapNoiseToLEDsUsingPalette(){
  static uint8_t ihue=0;
  for(int i=0; i<kMatrixWidth; i++){
    for(int j=0; j<kMatrixHeight; j++){
      uint8_t index = noise[j][i];
      uint8_t   bri = noise[i][j];
      if(hmd) index += ihue;
      bri = (bri>127)? 255: dim8_raw(bri*2);
      CRGB color = ColorFromPalette(pmd? palettes[currPalIdx]:gCurrentPalette, index, bri);
      leds[XY(i,j)] = color;
    }
  }
  ihue+=1;
}

void lavaLamp(){
  fillnoise8();
  mapNoiseToLEDsUsingPalette();
}

// moves a noise up and down while slowly shifting to the side
void TLava1() {
  uint8_t scale = glit;                               // the "zoom factor" for the noise
  for(uint16_t i=0; i<NUM_LEDS*2; i++){
    uint16_t shift_x = beatsin8(17);                  // the x position of the noise field swings @ 17 bpm
    uint16_t shift_y = millis() / 100;                // the y position becomes slowly incremented
    uint32_t real_x = (j[i] + shift_x) * scale;       // calculate the coordinates within the noise field
    uint32_t real_y = (k[i] + shift_y) * scale;       // based on the precalculated positions
    uint8_t noise = inoise16(real_x, real_y, 4223) >> 8;           // get the noise data and scale it down
    uint8_t index = noise * speed;                        // map led color based on noise data
    //CRGB color = CHSV(index, 255, noise);
    CRGB color = ColorFromPalette(pmd? palettes[currPalIdx]:gCurrentPalette, index, noise);
    leds[i] = color;
  }
}

// just moving along one axis = "lavalamp effect"
void TLava2() {
  uint8_t scale = glit;                               // the "zoom factor" for the noise
  for(uint16_t i=0; i<NUM_LEDS*2; i++){
    uint16_t shift_x = millis() / 10;                 // x as a function of time
    uint16_t shift_y = 0;
    uint32_t real_x = (j[i] + shift_x) * scale;       // calculate the coordinates within the noise field
    uint32_t real_y = (k[i] + shift_y) * scale;       // based on the precalculated positions
    uint8_t noise = inoise16(real_x, real_y, 4223) >> 8;           // get the noise data and scale it down
    uint8_t index = noise * speed;                        // map led color based on noise data
    //CRGB color = CHSV(index, 255, noise);
    CRGB color = ColorFromPalette(pmd? palettes[currPalIdx]:gCurrentPalette, index, noise);
    leds[i] = color;
  }
}

// no x/y shifting but scrolling along z
void TLava3(){
  uint8_t scale = glit;    // 232                           // the "zoom factor" for the noise
  for(uint16_t i=0; i<NUM_LEDS*2; i++){
    uint16_t shift_x = 0;                             // no movement along x and y
    uint16_t shift_y = 0;
    uint32_t real_x = (j[i] + shift_x) * scale;       // calculate the coordinates within the noise field
    uint32_t real_y = (k[i] + shift_y) * scale;       // based on the precalculated positions
    uint32_t real_z = millis() * 20;                  // increment z linear
    uint8_t noise = inoise16(real_x, real_y, real_z) >> 8;           // get the noise data and scale it down
    uint8_t index = noise * speed; // (higher = more colors)  [3-8 recommended]
    //CRGB color = CHSV(index, 255, noise);
    CRGB color = ColorFromPalette(pmd? palettes[currPalIdx]:gCurrentPalette, index, noise);
    leds[i] = color;
  }
}
