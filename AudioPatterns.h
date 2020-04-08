CRGB c[] = { CRGB(255,0,0), CRGB(255,96,0), CRGB(255,255,0), CRGB(0,255,0), CRGB(0,255,255), CRGB(0,0,255), CRGB(255,0,255) };

uint8_t f = 1;

void addGlitter(uint8_t chanceOfGlitter){
  if(random8()<chanceOfGlitter)
    leds[random16(NUM_LEDS)] += CRGB::White;
}
void fadeAll(){
  if(f%6==0)
    for(uint16_t i=0; i<NUM_LEDS; ++i)
      leds[i].fadeToBlackBy(1);
  ++f;
  if(f==0)
    f = 1;
}
void fadeAll(uint8_t value){
  for(int i=0; i<NUM_LEDS; ++i)
    leds[i].fadeToBlackBy(value);
}
void fadeRGB(uint8_t value){
  if(f%6==0)
    for(int i=0; i<NUM_LEDS; ++i)
      if(!(leds[i].r==leds[i].g&&leds[i].g==leds[i].b) && max(max(leds[i].r,leds[i].g),leds[i].b)>1)
        leds[i].fadeToBlackBy(value);
  ++f;
  if(f==0)
    f = 1;
}
void fadeWhites(uint8_t value){
  for(int i=0; i<NUM_LEDS; ++i)
    if(leds[i].r==leds[i].g && leds[i].g==leds[i].b)
      leds[i].fadeToBlackBy(value);
}
void freshWhites(){
    for(int i=0; i<NUM_LEDS; ++i)
      if(leds[i].r==leds[i].g && leds[i].g==leds[i].b)
        leds[i] = CRGB::White;  
}
void goGray(){
  for(uint16_t i=0; i<NUM_LEDS; ++i)
    if(leds[i]){
      newHue = max(max(leds[i].r, leds[i].g), leds[i].b);
      leds[i] = CRGB(newHue,newHue,newHue);
    }
}
void push(uint16_t idx){
    for(uint16_t i=idx ; i>0; --i)
      leds[i] = leds[i-1];
}
void pull(uint16_t idx){
    for(uint16_t i=0; i<idx; ++i)
      leds[i] = leds[i+1];
}
void mirror(uint16_t k){
    for(int i=0; i<k; ++i)
      leds[k-i]=leds[i];
}
void copyIt(uint8_t k){ memcpy(leds+NUM_LEDS/k, leds, NUM_LEDS*3^(k-1)/k); }
void goUp()     { mrs = !mrs;             morphs+=13; }
void goDown()   { mrs = !mrs; morphs!=25? morphs-=12: morphs=0; }
void morph()    { mrs? goUp(): goDown(); }


void newFlow(){ EVERY_N_SECONDS      (12){ morph(); }
                EVERY_N_MILLISECONDS(50){ if(rgbRate>0) --rgbRate;
                                          if(rgbRate>4) --rgbRate;}
                if(bfd) fadeWhites(32);
                if(tfd) fadeRGB(1);
                if(mor) speed = morphs;
  bool   zero = speed==0? true: false;
  //uint8_t bri = pow((trebleAvg/15),2);
  uint8_t bri = trebleAvg;
  uint16_t  k = edges[speed];
  if(!bmd&&!sv2){
    memcpy(leds, last, NUM_LEDS*3);
    sv2 = !sv2;
  }
  inv? pull(zero? k: k>>1): push(zero? k: k>>1);
  if(beatDetect()){ //++rgbRate;
    if(!bmd){
      if(sv2){
        memcpy(last, leds, NUM_LEDS*3);
        fmd? goGray(): fill_solid(leds, NUM_LEDS, CRGB::White);
        sv2 = !sv2;
      }
    } else{ // !bmd
      freshWhites();
      leds[inv? zero? k: k>>1: 0] = CRGB::White;
      if(!hup) hmd? huey=gHue: dir? huey+=rgbRate: huey-=rgbRate;
    }
  } else{ // beatDetect()
    if(trebDetect()) rgbRate+=3;
    if(hup) hmd? huey=gHue: dir? huey+=rgbRate: huey-=rgbRate;
    leds[inv? zero? k: k>>1: 0] = ColorFromPalette(pmd? palettes[currPalIdx]: gCurrentPalette, huey, fmd? bri: 255, LINEARBLEND);
  }
  if(bri==0) leds[inv? zero? k: k>>1: 0] = CRGB(0,1,0);
  if(!zero) mirror(k);
  if(speed>1) copyIt(speed);
}
/*
void torriesFlow(){
  const uint16_t columnSize = NUM_LEDS/7;
  for(uint8_t i=0; i<7; i++){
    uint16_t columnStart = i * columnSize;
    int columnEnd = columnStart + columnSize;
    if(columnEnd >= NUM_LEDS)
      columnEnd = NUM_LEDS - 1;

    
  }
}*/


void besinFlow(){
  bool   zero = speed==0? true: false;
  uint16_t  k = edges[speed];
  inv? pull(zero? k: k>>1): push(zero? k: k>>1);
  leds[0] = ColorFromPalette(pmd? palettes[currPalIdx]: gCurrentPalette, millis(), spectrumAvg, NOBLEND);
  if(!zero) mirror(k);
  if(speed>1) copyIt(speed);
}

void newFlash(){
  fadeAll(bmd? 32: 96);
  if(bmd){
    if(beatDetect())
      for(int i=0; i<NUM_LEDS; i++)
        leds[i] = ColorFromPalette(pmd? palettes[currPalIdx]: gCurrentPalette, (gHue+i/2)*(speed+1), 255);
  } else{
    if(trebDetect())
      for(int i=0; i<NUM_LEDS; i++)
        leds[i] = ColorFromPalette(pmd? palettes[currPalIdx]: gCurrentPalette, (gHue+i/2)*(speed+1), 255);
  }
  addGlitter(spectrumAvg/8*glit);
}

void plasma(){
  static int16_t thisphase = 0;
  static int16_t thatphase = 0;
  uint16_t thisbright;
  uint16_t colorIndex;
  thisphase += beatsin8(6,-4,4);
  thatphase += beatsin8(7,-4,4);
  for(int k=0; k<NUM_LEDS; k++){
    thisbright = cubicwave8((k*8)+thisphase)/2;
    thisbright += cos8((k*10)+thatphase)/2;
    colorIndex = thisbright;
    thisbright = qsuba(thisbright, 255-spectrumAvg);
    leds[k] = ColorFromPalette(pmd? palettes[currPalIdx]:gCurrentPalette, colorIndex, thisbright, currentBlending);
  }
  addGlitter(spectrumAvg+glit*10-80);
}

void rainbowG() {
  uint8_t beatA = beatsin8(17, 0, 255);
  uint8_t beatB = beatsin8(13, 0, 255);
  uint8_t beatC = beatsin8(11, 0, 255);
  for(int i=0; i<NUM_LEDS; i++){
    int colorIndex = (beatA+beatB+beatC)/3*i*4/NUM_LEDS;
    leds[i] = ColorFromPalette(pmd? palettes[currPalIdx]: gCurrentPalette, colorIndex, spectrumAvg, currentBlending);
  }
  addGlitter(spectrumAvg+glit*10-80);
}

void VU(){
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  bool   zero = speed==0? 1: 0;
  uint16_t  k = edges[speed]+1;
  uint8_t  hk = k/2;
  if(k%2 > 0) ++hk;
  uint16_t newSpec = map(spectrumAvg,0,255,0,(zero? k-1: hk-1));
  for(uint16_t i=0; i<zero? k: hk; ++i){
    uint8_t j = map(i,0, zero? k-1: hk-1, 0, zero? 255: 127);
    if(i<=newSpec){
      leds[i]     = ColorFromPalette(pmd? palettes[currPalIdx]: gCurrentPalette, j);
      if(!zero)
      leds[k-1-i] = ColorFromPalette(pmd? palettes[currPalIdx]: gCurrentPalette, 255-j);
    } else{
      leds[i]     = CRGB::Black;
      if(!zero)
      leds[k-1-i] = CRGB::Black;
    }
  }
  if(speed>1) copyIt(speed);
}

void rainbowBit() {
  uint8_t beatA = beatsin8(2, 0, 255);
  if(beatDetect()){
    uint16_t start = random8(0, NUM_LEDS*5/6);
    uint8_t  leng  = random8(0, NUM_LEDS*5/12);
    if(start+leng>NUM_LEDS)
      leng -= start+leng-NUM_LEDS;
    fill_rainbow(leds+start, leng, beatA, 2);
  }
  fadeToBlackBy(leds, NUM_LEDS, 40);
  addGlitter(spectrumAvg/2+glit*10-80);
}

void byteColumns(){
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  const uint16_t columnSize = NUM_LEDS/7;
  for(uint8_t i=0; i<7; i++){
    uint16_t columnStart = i * columnSize;
    int columnEnd = columnStart + columnSize;
    if(columnEnd >= NUM_LEDS)
      columnEnd = NUM_LEDS - 1;
    uint8_t columnHeight = map8(spectrumByte[i], 1, columnSize);
    for(uint16_t j = columnStart; j < columnStart + columnHeight; j++){
      if(j>=NUM_LEDS || j>=columnEnd)
        continue;
      leds[j] = c[i];
    }
  }
}

void peakColumns(){ fill_solid(leds, NUM_LEDS, CRGB::Black);
  const uint16_t columnSize = NUM_LEDS/7;
  for(uint8_t i=0; i<7; i++){
    uint8_t columnHeight = map(spectrumValue[i], 0, 1865, 0, columnSize);
    uint8_t   peakHeight = map(spectrumByte[i], 0, 255, 0, columnSize);
    uint16_t columnStart = i * columnSize;
    uint16_t   columnEnd = columnStart + columnSize;
    if(columnEnd>=NUM_LEDS) columnEnd = NUM_LEDS - 1;
    for(uint16_t j=columnStart; j<columnStart+columnHeight; j++)
      if(j<NUM_LEDS && j<=columnEnd)
        leds[j] = CHSV(i * 40, 255, 96);
    uint16_t k = columnStart + peakHeight;
    if(k<NUM_LEDS && k<=columnEnd)
      leds[k] = CHSV(i * 40, 255, 255);
  }
}

void audioAnalyzer(){ fill_solid(leds, NUM_LEDS, CRGB::Black);
  uint8_t ch = speed<7? speed: 6;
  uint16_t audio = map(spectrumByte[ch], 0, 255, 0, NUM_LEDS-1);
  for(uint16_t i=0; i<NUM_LEDS; ++i)
    if(i<=audio)
      leds[i] = ColorFromPalette(pmd? palettes[currPalIdx]: gCurrentPalette, i);
}
