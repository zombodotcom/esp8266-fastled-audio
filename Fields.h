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

String getPatterns(){
  String json = "";
  for(uint8_t i=0; i<patternCount; i++){
    json += "\""+patterns[i].name+"\"";
    if(i<patternCount-1)
      json += ",";
  }
  return json;
}
String getPalettes(){
  String json = "";
  for(uint8_t i=0; i<paletteCount; i++){
    json += "\""+paletteNames[i]+"\"";
    if(i<paletteCount-1)
      json += ",";
  }
  return json;
}
String          getPower(){ return String(power);      }
String     getBrightness(){ return String(brightness); }
String        getPattern(){ return String(currPatIdx); }
String    getPaletteMode(){ return String(pmd);        }
String        getPalette(){ return String(currPalIdx); }
String        getAutoPat(){ return String(apt);        }
String     getAutoPatDur(){ return String(autoPatDur); }
String        getAutoPal(){ return String(apl);        }
String     getAutoPalDur(){ return String(autoPalDur); }
String          getSpeed(){ return String(speed);      }
String       getHueSpeed(){ return String(hueSpd);     }
String        getGlitter(){ return String(glit);       }
String      getDirection(){ return String(dir);        }
String      getInversion(){ return String(inv);        }
String       getBassFade(){ return String(bfd);        }
String       getTrebFade(){ return String(tfd);        }
String      getHueUpdate(){ return String(hup);        }
String        getHueMode(){ return String(hmd);        }
String       getBassMode(){ return String(bmd);        }
String      getFlashMode(){ return String(fmd);        }
String      getMorphMode(){ return String(mor);        }
String        getCooling(){ return String(cooling);    }
String       getSparking(){ return String(sparking);   }
String   getTwinkleSpeed(){ return String(twnkSpd);    }
String getTwinkleDensity(){ return String(twnkDns);    }
String     getSolidColor(){ return String(solidColor.r)
                           + "," + String(solidColor.g)
                           + "," + String(solidColor.b); }

FieldList fields = {
  { "power", "Power", BooleanFieldType, 0, 1, getPower },
  { "brightness", "Brightness", NumberFieldType, 0, 255, getBrightness },
  //{ "patterns", "Patterns", SectionFieldType },
  { "pattern", "Pattern",  SelectFieldType, 0, patternCount, getPattern, getPatterns },
  { "pmd", "Palette Mode", BooleanFieldType, 0, 1, getPaletteMode },
  { "palette", "Palette", SelectFieldType, 0, paletteCount, getPalette, getPalettes },
  { "apt", "Auto Pattern", BooleanFieldType, 0, 1, getAutoPat },
  { "apl", "Auto Palette", BooleanFieldType, 0, 1, getAutoPal },
  { "autoPatDur", "Auto Pattern Duration", NumberFieldType, 0, 255, getAutoPatDur },
  //{ "palettes", "Palettes", SectionFieldType },
  { "autoPalDur", "Auto Palette Duration", NumberFieldType, 0, 255, getAutoPalDur },
  { "controls", "Controls", SectionFieldType },
  { "speed", "Speed", NumberFieldType, 0, 25, getSpeed },
  { "hueSpd", "Hue Speed", NumberFieldType, 0, 25, getHueSpeed },
  { "glit", "Glit Amount", NumberFieldType, 0,255, getGlitter },
  { "dir", "Direction"   , BooleanFieldType, 0, 1, getDirection },
  { "inv", "Inversion"   , BooleanFieldType, 0, 1, getInversion },
  { "bfd", "Bass  Fade"  , BooleanFieldType, 0, 1, getBassFade },
  { "tfd", "Treb  Fade"  , BooleanFieldType, 0, 1, getTrebFade },
  { "hup", "T-Hue|B-Hue" , BooleanFieldType, 0, 1, getHueUpdate },
  { "hmd", "Hue   Mode"  , BooleanFieldType, 0, 1, getHueMode },
  { "bmd", "Bass  Mode"  , BooleanFieldType, 0, 1, getBassMode },
  { "fmd", "Flash Mode"  , BooleanFieldType, 0, 1, getFlashMode },
  { "mor", "Morph Mode"  , BooleanFieldType, 0, 1, getMorphMode },
  //{ "colors", "Color Picker", SectionFieldType },
  //{ "misc", "Misc", SectionFieldType },
  { "cooling", "Cooling", NumberFieldType, 0, 255, getCooling },
  { "sparking", "Sparking", NumberFieldType, 0, 255, getSparking },
  { "twnkSpd", "Twinkle Speed", NumberFieldType, 0, 8, getTwinkleSpeed },
  { "twnkDns", "Twinkle Density", NumberFieldType, 0, 8, getTwinkleDensity },
  { "solidColor", "Color", ColorFieldType, 0, 255, getSolidColor },
};

uint8_t fieldCount = ARRAY_SIZE(fields);
