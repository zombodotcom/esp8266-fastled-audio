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

void setPower(uint8_t value){
  power = value==0 ? 0:1;
  EEPROM.write(0, power);
  EEPROM.commit();
  broadcastInt("power", power);
}

void setBrightness(uint8_t value){
       if(value>255) value = 255;
  else if(value<  0) value =   0;
        brightness = value;
  FastLED.setBrightness(brightness);
  EEPROM.write(1, brightness);
  EEPROM.commit();
  broadcastInt("brightness", brightness);
}

void setPattern(uint8_t value){
                     if(value >= patternCount)
                        value =  patternCount-1;
  currPatIdx = value;
  EEPROM.write(2, currPatIdx);
  EEPROM.commit();
  broadcastInt("pattern", currPatIdx);
}
void setPatternName(String name){
  for(uint8_t i=0; i<patternCount; i++)
    if(patterns[i].name==name){
      setPattern(i);
      break;
    }
}
void adjustPat(){ currPatIdx++;
  if(currPatIdx >= patternCount)
    currPatIdx = 0;
  if(!apt){
    EEPROM.write(2, currPatIdx);
    EEPROM.commit();
  }
  broadcastInt("pattern", currPatIdx);
}
void setPaletteMode(uint8_t value){
  pmd = value==0 ? 0:1;
  EEPROM.write(11, pmd);
  EEPROM.commit();
  broadcastInt("pmd", pmd);
}

void setPalette(uint8_t value){
                     if(value>=paletteCount)
                        value =paletteCount-1;
  currPalIdx = value;
  EEPROM.write(5, currPalIdx);
  EEPROM.commit();
  broadcastInt("palette", currPalIdx);
}
void setPaletteName(String name){
  for(uint8_t i=0; i<paletteCount; i++)
    if(paletteNames[i]==name){
      setPalette(i);
      break;
    }
}
void adjustPal(){ currPalIdx++;
  if(currPalIdx >= paletteCount)
    currPalIdx = 0;
  if(!apt){
    EEPROM.write(5, currPalIdx);
    EEPROM.commit();
  }
  broadcastInt("palette", currPalIdx);
}
void setAutoPat(uint8_t value){
  apt = value==0 ? 0:1;
  EEPROM.write(3, apt);
  EEPROM.commit();
  broadcastInt("apt", apt);
}
void setAutoPal(uint8_t value){
  apl = value==0 ? 0:1;
  EEPROM.write(6, apl);
  EEPROM.commit();
  broadcastInt("apl", apl);
}
void setAutoPatDur(uint8_t value){
  autoPatDur = value;
  EEPROM.write(4, autoPatDur);
  EEPROM.commit();
  autoPatTimeout = millis()+(autoPatDur*1000);
  broadcastInt("autoPatDur", autoPatDur);
}
void setAutoPalDur(uint8_t value){
  autoPalDur = value;
  EEPROM.write(7, autoPalDur);
  EEPROM.commit();
  autoPalTimeout = millis()+(autoPalDur*1000);
  broadcastInt("autoPalDur", autoPalDur);
}


void setSpeed(uint8_t value){
         if(value>25) value = 25;
    else if(value< 0) value =  0;
              speed = value;
  EEPROM.write(8, speed);
  EEPROM.commit();
  broadcastInt("speed", speed);
}
void setHueSpd(uint8_t value){
          if(value>25) value = 25;
     else if(value< 0) value =  0;
              hueSpd = value;
  EEPROM.write(9, hueSpd);
  EEPROM.commit();
  broadcastInt("hueSpd", hueSpd);
}
void setGlit(uint8_t value){
       if(value>255) value = 255;
  else if(value<  0) value =   0;
              glit = value;
  EEPROM.write(10, glit);
  EEPROM.commit();
  broadcastInt("glit", glit);
}
void setDirection(uint8_t value){
  dir = value==0 ? 0:1;
  EEPROM.write(12, dir);
  EEPROM.commit();
  broadcastInt("dir", dir);
}
void setInversion(uint8_t value){
  inv = value==0 ? 0:1;
  EEPROM.write(13, inv);
  EEPROM.commit();
  broadcastInt("inv", inv);
}
void setBassFade(uint8_t value){
  bfd = value==0 ? 0:1;
  EEPROM.write(14, bfd);
  EEPROM.commit();
  broadcastInt("bfd", bfd);
}
void setTrebFade(uint8_t value){
  tfd = value==0 ? 0:1;
  EEPROM.write(15, tfd);
  EEPROM.commit();
  broadcastInt("tfd", tfd);
}
void setHueUpdate(uint8_t value){
  hup = value==0 ? 0:1;
  EEPROM.write(16, hup);
  EEPROM.commit();
  broadcastInt("hup", hup);
}
void setHueMode(uint8_t value){
  hmd = value==0 ? 0:1;
  EEPROM.write(17, hmd);
  EEPROM.commit();
  broadcastInt("hmd", hmd);
}
void setBassMode(uint8_t value){
  bmd = value==0 ? 0:1;
  EEPROM.write(18, bmd);
  EEPROM.commit();
  broadcastInt("bmd", bmd);
}
void setFlashMode(uint8_t value){
  fmd = value==0 ? 0:1;
  EEPROM.write(19, fmd);
  EEPROM.commit();
  broadcastInt("fmd", fmd);
}
void setMorphMode(uint8_t value){
  mor = value==0 ? 0:1;
  EEPROM.write(20, mor);
  EEPROM.commit();
  broadcastInt("mor", mor);
}

void setCooling(uint8_t value){
       if(value>255) value = 255;
  else if(value<  0) value =   0;
        cooling = value;
  EEPROM.write(24, cooling);
  EEPROM.commit();
  broadcastInt("cooling", cooling);
}
void setSparking(uint8_t value){
       if(value>255) value = 255;
  else if(value<  0) value =   0;
        sparking = value;
  EEPROM.write(25, sparking);
  EEPROM.commit();
  broadcastInt("sparking", sparking);
}

void setTwinkleSpeed(uint8_t value){
       if(value>8) value = 8;
  else if(value<0) value = 0;
        twnkSpd = value;
  EEPROM.write(26, twnkSpd);
  EEPROM.commit();
  broadcastInt("twnkSpd", twnkSpd);
}
void setTwinkleDensity(uint8_t value){
       if(value>8) value = 8;
  else if(value<0) value = 0;
        twnkSpd = value;
  EEPROM.write(26, twnkSpd);
  EEPROM.commit();
  broadcastInt("twnkSpd", twnkSpd);
}

void setSolidColor(uint8_t r, uint8_t g, uint8_t b){
  solidColor = CRGB(r, g, b);
  EEPROM.write(21, r);
  EEPROM.write(22, g);
  EEPROM.write(23, b);
  EEPROM.commit();
  setPattern(patternCount - 1);
  broadcastString("color", String(solidColor.r) + "," + String(solidColor.g) + "," + String(solidColor.b));
}
void setSolidColor(CRGB color){ setSolidColor(color.r, color.g, color.b); }
