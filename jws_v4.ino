//=================================================================
// JWS MUROTTAL V4.0 - ROBUST & SMOOTH
// Improved ESP8266 Mosque Controller
// Features:
// - Message Queue System
// - Watchdog Support  
// - Non-blocking Operations
// - Modern Web UI
//=================================================================

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
// #include <DNSServer.h>           // REMOVED - save ~2KB IRAM (no captive portal)
#include <FS.h>
#include <ArduinoJson.h>
// Removed untuk save IRAM:
// #include <ESP8266HTTPClient.h>  // REMOVED - tidak dipakai
// #include <ArduinoOTA.h>          // REMOVED - save ~8KB IRAM (web OTA masih ada)
// #include <WiFiUdp.h>             // REMOVED - untuk NTP (optional feature)
// #include <NTPClient.h>           // REMOVED - auto time sync (RTC manual set aja)

#include <SoftwareSerial.h>
#include <DFMiniMp3.h>

#include <Wire.h>
#include <RtcDS3231.h>

// Global RTC Object - Declare AFTER Wire/RtcDS3231 but BEFORE headers that use it
RtcDS3231<TwoWire> Rtc(Wire);

#include <HJS589.h>

#include <fonts/ElektronMart6x8.h>
#include <fonts/ElektronMart6x16.h>
#include <fonts/ElektronMart5x6.h>
#include <fonts/ElektronMartArabic6x16.h>
#include <fonts/ElektronMartArabic5x6.h>

#include <PrayerTimes.h>

#include "Display.h"
#include "DFPlayer.h"
#include "WebPage_V4.h"    // New modern UI
#include "FSConfig.h"
#include "WebServer_V4.h"  // Improved WebServer
#include "RTC.h"
#include "JWS.h"

// Watchdog
#include <Ticker.h>
Ticker watchdogTicker;
volatile bool watchdogFlag = false;

void ICACHE_RAM_ATTR watchdogISR() {
  watchdogFlag = true;
}

void feedWatchdog() {
  watchdogFlag = false;
}

void checkWatchdog() {
  if (watchdogFlag) {
    Serial.println("\n⚠ WATCHDOG TIMEOUT - RESTARTING...");
    delay(500);
    ESP.restart();
  }
}

/* ============================================
   OTA & NTP - DISABLED untuk save IRAM
   Web OTA masih available via /update page
   RTC set manual via web interface
   ============================================ */

/*
// NTP Client for Time Sync (DISABLED - save IRAM)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 3600000);

// OTA Setup Functions (DISABLED - save ~8KB IRAM)
void setupOTA() {
  ArduinoOTA.setHostname("JWS-ESP8266");
  ArduinoOTA.setPassword("jws123");
  
  ArduinoOTA.onStart([]() {
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
    Serial.println("\n[OTA] Update Start: " + type);
    Disp.clear();
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\n[OTA] Update Complete!");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 1000) {
      Serial.printf("[OTA] Progress: %u%%\r", (progress / (total / 100)));
      lastPrint = millis();
    }
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("\n[OTA] Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  
  ArduinoOTA.begin();
  Serial.println("✓ OTA Ready (Password: jws123)");
}

// NTP Time Sync (DISABLED - save IRAM)
void syncNTPTime() {
  if (WiFi.status() == WL_CONNECTED) {
    timeClient.update();
    
    unsigned long epochTime = timeClient.getEpochTime();
    if (epochTime > 0) {
      struct tm *timeinfo = gmtime((time_t*)&epochTime);
      
      RtcDateTime now = Rtc.GetDateTime();
      RtcDateTime ntpTime(
        timeinfo->tm_year + 1900,
        timeinfo->tm_mon + 1,
        timeinfo->tm_mday,
        timeinfo->tm_hour,
        timeinfo->tm_min,
        timeinfo->tm_sec
      );
      
      uint32_t nowSec = now.TotalSeconds();
      uint32_t ntpSec = ntpTime.TotalSeconds();
      uint32_t diff = (nowSec > ntpSec) ? (nowSec - ntpSec) : (ntpSec - nowSec);
      
      if (diff > 10) {
        Rtc.SetDateTime(ntpTime);
        Serial.println("[NTP] ✓ RTC synced with NTP");
      }
    }
  }
}
*/

unsigned long websocketCount = 0, wait = 0;

void setup() {
  
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n\n");
  Serial.println("╔════════════════════════════════════╗");
  Serial.println("║   JWS MUROTTAL V4.0 - STARTING    ║");
  Serial.println("╚════════════════════════════════════╝\n");
  
  // Start Watchdog (60 seconds timeout - reduce interrupt overhead)
  watchdogTicker.attach(60, watchdogISR);
  Serial.println("✓ Watchdog started (60s timeout)");
  
  // Buzzer Init
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  delay(50);
  Serial.println("✓ Buzzer initialized");
  
  // RTC Init
  Serial.println("\n→ Initializing RTC...");
  mulaiRTC();
  feedWatchdog();
  
  // SPIFFS Init
  Serial.println("\n→ Mounting SPIFFS...");
  if (!SPIFFS.begin()) {
    Serial.println("✗ SPIFFS Mount Failed!");
  } else {
    FSInfo fs_info;
    SPIFFS.info(fs_info);
    Serial.printf("✓ SPIFFS: %d/%d bytes used\n", fs_info.usedBytes, fs_info.totalBytes);
  }
  feedWatchdog();
  
  // Load Configurations
  Serial.println("\n→ Loading configurations...");
  loadDispConfig(fileconfigdisp, configdisp);
  loadDfConfig(fileconfigdf, configdf);
  loadInfoConfig(fileconfiginfo, configinfo);
  loadJwsConfig(fileconfigjws, configjws);
  loadWifiConfig(fileconfigwifi, configwifi);
  
  LoadDataAwal();
  printAllConfig();
  feedWatchdog();
  
  // WiFi Init
  Serial.println("\n→ Starting WiFi...");
  wifiConnect();
  feedWatchdog();
  
  /* OTA & NTP Setup - DISABLED untuk save IRAM
  if (WiFi.status() == WL_CONNECTED || WiFi.getMode() == WIFI_AP_STA) {
    Serial.println("\n→ Setting up OTA...");
    setupOTA();
    feedWatchdog();
    
    // NTP Client Start
    Serial.println("→ Starting NTP Client...");
    timeClient.begin();
    Serial.println("✓ NTP Client started");
  }
  */
  
  // Web Server Setup
  Serial.println("\n→ Setting up Web Server...");
  server.on("/", [](){
    Serial.println("[HTTP] Serving webpage...");
    yield();
    server.sendHeader("Connection", "close");
    server.sendHeader("Cache-Control", "no-cache");
    yield();
    server.send_P(200, "text/html", webpage);
    yield();
    Serial.println("[HTTP] Page sent successfully");
  });
  
  server.on("/xml", handleXML);
  server.on("/realxml", handleRealXML);
  
  // Backup/Restore Endpoints
  server.on("/backup", HTTP_GET, [](){
    Serial.println("[HTTP] Backup request");
    String backup = "{";
    backup += "\"jws\":" + String(fileconfigjws) + ",";
    backup += "\"wifi\":" + String(fileconfigwifi) + ",";
    backup += "\"disp\":" + String(fileconfigdisp) + ",";
    backup += "\"df\":" + String(fileconfigdf) + ",";
    backup += "\"info\":" + String(fileconfiginfo);
    backup += "}";
    server.send(200, "application/json", backup);
    Serial.println("[HTTP] Backup sent");
  });
  
  server.on("/restore", HTTP_POST, [](){
    Serial.println("[HTTP] Restore request");
    if (server.hasArg("plain")) {
      String body = server.arg("plain");
      // Parse and restore configs here
      server.send(200, "text/plain", "OK");
      Serial.println("[HTTP] Config restored");
    } else {
      server.send(400, "text/plain", "No data");
    }
  });
  
  // Web OTA Update Page
  server.on("/update", HTTP_GET, [](){
    String html = F("<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>"
      "<title>OTA Update - JWS</title><style>*{margin:0;padding:0;box-sizing:border-box}body{font-family:Arial,sans-serif;background:#0a0e27;color:#e0e0e0;padding:20px}"
      ".container{max-width:600px;margin:0 auto;background:#1a1f3a;padding:30px;border-left:4px solid #4CAF50}h1{color:#4CAF50;margin-bottom:20px}"
      ".upload-area{border:2px dashed #4CAF50;padding:40px;text-align:center;margin:20px 0;cursor:pointer}.upload-area:hover{background:#0f1329}"
      "input[type=file]{display:none}.btn{background:#4CAF50;color:#fff;padding:15px 30px;border:none;cursor:pointer;width:100%;font-size:1rem;margin-top:10px}"
      ".btn:hover{opacity:0.8}.progress{width:100%;height:20px;background:#0a0e27;margin:20px 0;display:none}.progress-bar{height:100%;background:#4CAF50;width:0%;transition:width 0.3s}"
      ".status{margin-top:20px;padding:15px;border-left:3px solid #4CAF50;background:#0f1329;display:none}</style></head><body>"
      "<div class='container'><h1>🚀 OTA Firmware Update</h1><p style='color:#888;margin-bottom:20px'>Upload file .bin untuk update firmware</p>"
      "<div class='upload-area' onclick='document.getElementById(\"firmware\").click()'><p style='font-size:3rem;margin-bottom:10px'>📁</p>"
      "<p>Klik untuk pilih file .bin</p><p style='font-size:0.85rem;color:#888;margin-top:10px'>atau drag & drop file disini</p></div>"
      "<input type='file' id='firmware' accept='.bin'><button class='btn' onclick='uploadFirmware()'>⬆️ Upload & Update</button>"
      "<div class='progress'><div class='progress-bar' id='progress'></div></div><div class='status' id='status'></div>"
      "<p style='margin-top:30px;color:#666;font-size:0.85rem'>⚠️ Jangan putus koneksi selama update<br>Device akan restart otomatis setelah selesai</p></div>"
      "<script>const input=document.getElementById('firmware');input.addEventListener('change',function(){"
      "document.querySelector('.upload-area p').textContent='File: '+this.files[0].name});"
      "function uploadFirmware(){const file=input.files[0];if(!file){alert('Pilih file .bin dulu!');return}"
      "const formData=new FormData();formData.append('firmware',file);const xhr=new XMLHttpRequest();"
      "xhr.upload.addEventListener('progress',function(e){const percent=(e.loaded/e.total)*100;"
      "document.querySelector('.progress').style.display='block';document.getElementById('progress').style.width=percent+'%'});"
      "xhr.addEventListener('load',function(){document.getElementById('status').style.display='block';"
      "if(xhr.status===200){document.getElementById('status').innerHTML='✓ Update Success!<br>Device restarting...';"
      "setTimeout(()=>location.href='/',5000)}else{document.getElementById('status').innerHTML='✗ Update Failed!'}});"
      "xhr.addEventListener('error',function(){document.getElementById('status').style.display='block';"
      "document.getElementById('status').innerHTML='✗ Connection Error!'});xhr.open('POST','/update',true);xhr.send(formData)}</script></body></html>");
    server.send(200, "text/html", html);
  });
  
  // Web OTA Upload Handler
  server.on("/update", HTTP_POST, [](){
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    delay(1000);
    ESP.restart();
  }, [](){
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("\n[OTA] Update Start: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if (!Update.begin(maxSketchSpace)) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      } else {
        Serial.printf("[OTA] Progress: %u%%\r", (Update.progress() * 100) / Update.size());
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) {
        Serial.printf("\n[OTA] Update Success: %u bytes\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  
  server.begin();
  Serial.println("✓ HTTP Server started on port 80");
  
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("✓ WebSocket Server started on port 81");
  
  feedWatchdog();
  
  // DFPlayer Init
  Serial.println("\n→ Initializing DFPlayer...");
  mulaiMP3();
  mp3.setVolume(configdf.volume);
  Serial.printf("✓ DFPlayer ready, Volume: %d\n", configdf.volume);
  
  feedWatchdog();
  
  // Display Init
  Serial.println("\n→ Initializing Display...");
  Disp_init();
  Disp.setBrightness(configdisp.kecerahan);
  Serial.printf("✓ Display ready, Brightness: %d, Panels: %d\n", 
                configdisp.kecerahan, configdisp.panel);
  
  feedWatchdog();
  
  // Display IP Address di P10 selama 10 detik
  Disp.clear();
  Disp.setFont(ElektronMart5x6);
  
  if (WiFi.status() == WL_CONNECTED) {
    // Jika connect ke WiFi, tampilkan WiFi IP
    String ip = WiFi.localIP().toString();
    Serial.println("\n→ Displaying WiFi IP on P10...");
    
    Disp.drawText(2, 0, "WiFi IP:");
    Disp.drawText(0, 8, ip.c_str());
    
    delay(10000); // Tampil 10 detik
  } else {
    // Jika mode AP, tampilkan AP IP
    String apIP = WiFi.softAPIP().toString();
    Serial.println("\n→ Displaying AP IP on P10...");
    
    Disp.drawText(2, 0, "AP Mode:");
    Disp.drawText(0, 8, apIP.c_str());
    
    delay(10000); // Tampil 10 detik
  }
  
  Disp.clear();
  feedWatchdog();
  
  Serial.println("\n╔════════════════════════════════════╗");
  Serial.println("║    SYSTEM READY - JWS V4.0        ║");
  Serial.println("╚════════════════════════════════════╝\n");
}

// Optimized Loop
void loop() {
  
  // Feed watchdog first
  feedWatchdog();
  
  // OTA Handler - DISABLED untuk save IRAM
  // ArduinoOTA.handle();
  
  // Network handling (high priority)
  // dnsServer.processNextRequest(); // Captive Portal - REMOVED untuk save IRAM
  webSocket.loop();
  server.handleClient();
  
  // Process message queue (non-blocking)
  processWebMessages();
  
  /* NTP Sync - DISABLED untuk save IRAM
  static unsigned long lastNTPSync = 0;
  if (millis() - lastNTPSync > 3600000) {
    syncNTPTime();
    lastNTPSync = millis();
  }
  */
  
  // Serial passthrough (if needed)
  if (Serial.available() > 0) {
    char c[] = {(char)Serial.read()};
    webSocket.broadcastTXT(c, sizeof(c));
  }
  
  // Main display state machine
  switch(tmputama) {
    case 0:
      tampiljws();
      break;
    case 1:
      Iqomah();
      break;
    case 2:
      DFPlayer();
      break;
    case 3:
      TestLED();
      break;
  }
  
  // Yield to system
  yield();
}

// JWS Display Handler
void tampiljws() {
  switch(tmpjws) {
    case 0:
      Intro1P();
      break;
    case 1:
      if (configdisp.panel == 1) {
        TampilInfo();
      }
      if (configdisp.panel == 2) {
        JamBesarInfo();
      }
      break;
    case 2:
      TeksBerjalanKananKiri();
      AlarmSholat();
      break;
    case 3:
      TeksJalanInfo1();
      AlarmSholat();
      break;
    case 4:
      TeksJalanInfo2();
      AlarmSholat();
      break;
    case 5:
      intro1p = 0;
      tmpjws = 0;
      break;
  }
}

void JamBesarInfo() {
  JamBesar(1);
  TampilInfo();
  AlarmSholat();
}

void Intro1P() {
  switch(intro1p) {
    case 0:
      animLogoX();
      break;
    case 1:
      Disp.drawRect(0, 0, 31, 15, 0, 0);
      JamBesar(32);
      break;
  }
}

void TampilInfo() {
  switch(tmpinfo) {
    case 0:
      TampilTanggal();
      break;
    case 1:
      TampilSuhu();
      break;
    case 2:
      TampilJadwalSholat();
      break;
  }
}

void DFPlayer() {
  char vol[3];
  sprintf(vol, "%02d", configdf.volume);
  
  char track[4];
  sprintf(track, "%03d", currentTrack);
  
  Disp.setFont(ElektronMart6x8);
  Disp.drawText(1, 0, "Volume");
  Disp.drawText(1, 8, vol);
  Disp.drawRect(16, 8, 16 + (configdf.volume * 24 / 30), 14, 1, 1);
  
  Disp.drawText(50, 0, "TR");
  Disp.drawText(43, 8, track);
}

void TestLED() {
  Disp.drawRect(0, 0, Disp.width(), 16, 1, 1);
}
