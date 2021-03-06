#define qsubd(x, b)  ((x>b)?b:0)
#define qsuba(x, b)  ((x>b)?x-b:0)

#define  MSGEQ7_AUDIO_PIN A0
#define MSGEQ7_STROBE_PIN D3
#define  MSGEQ7_RESET_PIN D4
#define     AUDIODELAY  0
#define SPECTRUMSMOOTH  0.08
#define      PEAKDECAY  0.01
#define     NOISEFLOOR 65
#define      AGCSMOOTH  0.006
#define GAINUPPERLIMIT 15.0
#define GAINLOWERLIMIT  0.1

unsigned int spectrumValue[7];
float spectrumDecay[7] = {0};
float spectrumPeaks[7] = {0};
float audioAvg = 270.0;
float  gainAGC =   0.0;
uint8_t spectrumByte[7];
uint8_t peakAvg;

uint8_t spectrumAvg;
uint8_t trebleAvg;

unsigned long currentMillis;
unsigned long audioMillis;

void initializeAudio() {
  pinMode(MSGEQ7_AUDIO_PIN, INPUT);
  pinMode(MSGEQ7_RESET_PIN, OUTPUT);
  pinMode(MSGEQ7_STROBE_PIN, OUTPUT);
  digitalWrite(MSGEQ7_RESET_PIN, LOW);
  digitalWrite(MSGEQ7_STROBE_PIN, HIGH);
}

void readAudio(){
  static PROGMEM const byte spectrumFactors[7] = {9, 11, 13, 13, 12, 12, 13};
  digitalWrite(MSGEQ7_RESET_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(MSGEQ7_RESET_PIN, LOW);
  int analogsum = 0;
  int treblesum = 0;
  for(int i=0; i<7; i++){
    digitalWrite(MSGEQ7_STROBE_PIN, LOW);
    delayMicroseconds(36);
    spectrumValue[i] = analogRead(MSGEQ7_AUDIO_PIN);
    digitalWrite(MSGEQ7_STROBE_PIN, HIGH);
    spectrumValue[i]<NOISEFLOOR? spectrumValue[i]=0: spectrumValue[i]-=NOISEFLOOR;
    spectrumValue[i] = (spectrumValue[i] * pgm_read_byte_near(spectrumFactors + i)) / 10;
    analogsum += spectrumValue[i];
    if(i!=0) treblesum += spectrumValue[i];
    spectrumValue[i] *= gainAGC;
    spectrumDecay[i] = (1.0 - SPECTRUMSMOOTH) * spectrumDecay[i] + SPECTRUMSMOOTH * spectrumValue[i];
    if(spectrumPeaks[i] < spectrumDecay[i])
      spectrumPeaks[i] = spectrumDecay[i];
    spectrumPeaks[i] = spectrumPeaks[i] * (1.0 - PEAKDECAY);
    spectrumByte[i] = spectrumValue[i] / 4;
  }
  trebleAvg = (treblesum / 6.0) / 4;
  spectrumAvg = (analogsum / 7.0) / 4;
  audioAvg = (1.0 - AGCSMOOTH) * audioAvg + AGCSMOOTH * spectrumAvg;
  gainAGC = 270.0 / audioAvg;
  if (gainAGC > GAINUPPERLIMIT) gainAGC = GAINUPPERLIMIT;
  if (gainAGC < GAINLOWERLIMIT) gainAGC = GAINLOWERLIMIT;
}

#define      beatLvl  100.0
#define     beatDZn  100.0
#define    beatDelay  200
float       lastBeat = 0;
byte         beatHit = 0;
byte beatDetect() {
  static float beatAvg = 0;
  static unsigned long lastBeatMillis;
  uint8_t specCombo = spectrumByte[0];
  beatAvg = (1.0-AGCSMOOTH)*beatAvg + AGCSMOOTH*specCombo;
  if(lastBeat<beatAvg)
    lastBeat = beatAvg;
  if((specCombo-beatAvg)>beatLvl && beatHit==0 && (currentMillis-lastBeatMillis)>beatDelay){
    beatHit = 1;
    lastBeat = specCombo;
    lastBeatMillis = currentMillis;
    return 1;
  } else if((lastBeat-specCombo)>beatDZn){
    beatHit = 0;
    return 0;
  } else
    return 0;
}

byte   trebTriggered = 0;
#define    trebLevel  66
#define trebDeadzone  66
#define    trebDelay  66
float       lastSpec = 0;
byte trebDetect(){
  static float trebAvg = 0;
  static unsigned long lastTrebMillis;
  uint8_t specCombo = spectrumByte[6];
  trebAvg = (1.0-AGCSMOOTH)*trebAvg + AGCSMOOTH*specCombo;
  if(lastSpec<trebAvg)
    lastSpec = trebAvg;
  if((specCombo-trebAvg)>trebLevel && trebTriggered==0 && (currentMillis-lastTrebMillis)>trebDelay){
    trebTriggered = 1;
    lastSpec = specCombo;
    lastTrebMillis = currentMillis;
    return 1;
  } else if((lastSpec-specCombo)>trebDeadzone){
    trebTriggered = 0;
    return 0;
  } else
    return 0;
}
