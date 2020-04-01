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

EEPROM.begin(512);
  loadSettings();
  FastLED.setBrightness(brightness);
  SPIFFS.begin();{
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
    }
  }
  // SET HOSTNAME
  String hostname(HOSTNAME);
  hostname += String(ESP.getChipId(), HEX);
  WiFi.hostname(hostname);
  char hostnameChar[hostname.length() + 1];
  memset(hostnameChar, 0, hostname.length() + 1);
  for(uint8_t i=0; i<hostname.length(); i++)
    hostnameChar[i] = hostname.charAt(i);
  if(apMode){
    WiFi.mode(WIFI_AP);
    uint8_t mac[WL_MAC_ADDR_LENGTH];
    WiFi.softAPmacAddress(mac);
    String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                   String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
    macID.toUpperCase();
    String AP_NameString = "ESP8266-" + macID;
    char AP_NameChar[AP_NameString.length() + 1];
    memset(AP_NameChar, 0, AP_NameString.length() + 1);
    for(int i=0; i<AP_NameString.length(); i++)
      AP_NameChar[i] = AP_NameString.charAt(i);
    WiFi.softAP(AP_NameChar, WiFiAPPSK);
  } else{
    WiFi.mode(WIFI_STA);
    if(String(WiFi.SSID()) != String(ssid))
      WiFi.begin(ssid, password);
  }

  httpUpdateServer.setup(&webServer);

  webServer.on("/all", HTTP_GET, []() {
    String json = getFieldsJson(fields, fieldCount);
    webServer.send(200, "text/json", json);
  });
  webServer.on("/fieldValue", HTTP_GET, []() {
    String name = webServer.arg("name");
    String value = getFieldValue(name, fields, fieldCount);
    webServer.send(200, "text/json", value);
  });
  webServer.on("/fieldValue", HTTP_POST, []() {
    String name = webServer.arg("name");
    String value = webServer.arg("value");
    String newValue = setFieldValue(name, value, fields, fieldCount);
    webServer.send(200, "text/json", newValue);
  });
  webServer.on("/power", HTTP_POST, []() {
    String value = webServer.arg("value");
    setPower(value.toInt());
    sendInt(power);
  });
  webServer.on("/brightness", HTTP_POST, []() {
    String value = webServer.arg("value");
    setBrightness(value.toInt());
    sendInt(brightness);
  });
  webServer.on("/pattern", HTTP_POST, []() {
    String value = webServer.arg("value");
    setPattern(value.toInt());
    sendInt(currPatIdx);
  });
  webServer.on("/patternName", HTTP_POST, []() {
    String value = webServer.arg("value");
    setPatternName(value);
    sendInt(currPatIdx);
  });
  webServer.on("/pmd", HTTP_POST, []() {
    String value = webServer.arg("value");
    setPaletteMode(value.toInt());
    broadcastInt("pmd", pmd);
    sendInt(pmd);
  });
  webServer.on("/palette", HTTP_POST, []() {
    String value = webServer.arg("value");
    setPalette(value.toInt());
    sendInt(currPalIdx);
  });
  webServer.on("/paletteName", HTTP_POST, []() {
    String value = webServer.arg("value");
    setPaletteName(value);
    sendInt(currPalIdx);
  });
  webServer.on("/apt", HTTP_POST, []() {
    String value = webServer.arg("value");
    setAutoPat(value.toInt());
    sendInt(apt);
  });
  webServer.on("/apl", HTTP_POST, []() {
    String value = webServer.arg("value");
    setAutoPal(value.toInt());
    sendInt(apl);
  });
  webServer.on("/autoPatDur", HTTP_POST, []() {
    String value = webServer.arg("value");
    setAutoPatDur(value.toInt());
    sendInt(autoPatDur);
  });
  webServer.on("/autoPalDur", HTTP_POST, []() {
    String value = webServer.arg("value");
    setAutoPalDur(value.toInt());
    sendInt(autoPalDur);
  });
  webServer.on("/speed", HTTP_POST, []() {
    String value = webServer.arg("value");
    setSpeed(value.toInt());
    broadcastInt("speed", speed);
    sendInt(speed);
  });
  webServer.on("/hueSpd", HTTP_POST, []() {
    String value = webServer.arg("value");
    setHueSpd(value.toInt());
    //hueSpd = value.toInt();
    broadcastInt("hueSpd", hueSpd);
    sendInt(hueSpd);
  });
  webServer.on("/glit", HTTP_POST, []() {
    String value = webServer.arg("value");
    setGlit(value.toInt());
    //glit = value.toInt();
    broadcastInt("glit", glit);
    sendInt(glit);
  });
  webServer.on("/dir", HTTP_POST, []() {
    String value = webServer.arg("value");
    setDirection(value.toInt());
    broadcastInt("dir", dir);
    sendInt(dir);
  });
  webServer.on("/inv", HTTP_POST, []() {
    String value = webServer.arg("value");
    setInversion(value.toInt());
    broadcastInt("inv", inv);
    sendInt(inv);
  });
  webServer.on("/bfd", HTTP_POST, []() {
    String value = webServer.arg("value");
    setBassFade(value.toInt());
    broadcastInt("bfd", bfd);
    sendInt(bfd);
  });
  webServer.on("/tfd", HTTP_POST, []() {
    String value = webServer.arg("value");
    setTrebFade(value.toInt());
    broadcastInt("tfd", tfd);
    sendInt(tfd);
  });
  webServer.on("/hup", HTTP_POST, []() {
    String value = webServer.arg("value");
    setHueUpdate(value.toInt());
    broadcastInt("hup", hup);
    sendInt(hup);
  });
  webServer.on("/hmd", HTTP_POST, []() {
    String value = webServer.arg("value");
    setHueMode(value.toInt());
    broadcastInt("hmd", hmd);
    sendInt(hmd);
  });
  webServer.on("/bmd", HTTP_POST, []() {
    String value = webServer.arg("value");
    setBassMode(value.toInt());
    broadcastInt("bmd", bmd);
    sendInt(bmd);
  });
  webServer.on("/fmd", HTTP_POST, []() {
    String value = webServer.arg("value");
    setFlashMode(value.toInt());
    broadcastInt("fmd", fmd);
    sendInt(fmd);
  });
  webServer.on("/mor", HTTP_POST, []() {
    String value = webServer.arg("value");
    setMorphMode(value.toInt());
    broadcastInt("mor", mor);
    sendInt(mor);
  });
  webServer.on("/cooling", HTTP_POST, []() {
    String value = webServer.arg("value");
    cooling = value.toInt();
    broadcastInt("cooling", cooling);
    sendInt(cooling);
  });
  webServer.on("/sparking", HTTP_POST, []() {
    String value = webServer.arg("value");
    sparking = value.toInt();
    broadcastInt("sparking", sparking);
    sendInt(sparking);
  });
  webServer.on("/twnkSpd", HTTP_POST, []() {
    String value = webServer.arg("value");
    twnkSpd = value.toInt();
    if (twnkSpd < 0) twnkSpd = 0;
    else if (twnkSpd > 8) twnkSpd = 8;
    broadcastInt("twnkSpd", twnkSpd);
    sendInt(twnkSpd);
  });
  webServer.on("/twnkDns", HTTP_POST, []() {
    String value = webServer.arg("value");
    twnkDns = value.toInt();
    if (twnkDns < 0) twnkDns = 0;
    else if (twnkDns > 8) twnkDns = 8;
    broadcastInt("twnkDns", twnkDns);
    sendInt(twnkDns);
  });
  webServer.on("/solidColor", HTTP_POST, []() {
    String r = webServer.arg("r");
    String g = webServer.arg("g");
    String b = webServer.arg("b");
    setSolidColor(r.toInt(), g.toInt(), b.toInt());
    sendString(String(solidColor.r) + "," + String(solidColor.g) + "," + String(solidColor.b));
  });
  webServer.on("/list", HTTP_GET, handleFileList);
  webServer.on("/edit", HTTP_GET, []() {
    if (!handleFileRead("/edit.htm")) webServer.send(404, "text/plain", "FileNotFound");
  });
  webServer.on("/edit", HTTP_PUT, handleFileCreate);
  webServer.on("/edit", HTTP_DELETE, handleFileDelete);
  webServer.on("/edit", HTTP_POST, []() {
    webServer.send(200, "text/plain", "");
  }, handleFileUpload);

  webServer.serveStatic("/", SPIFFS, "/", "max-age=86400");

  webServer.begin();
  webSocketsServer.begin();
  webSocketsServer.onEvent(webSocketEvent);

  autoPatTimeout = millis() + (autoPatDur * 1000);
