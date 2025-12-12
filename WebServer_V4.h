//=================================================================
// WEBSERVER V4 - IMPROVED & ROBUST
// Features:
// - Message Queue System
// - Non-blocking operations
// - Better error handling
// - Watchdog support
//=================================================================

const char* netssid = configwifi.wifissid;
const char* netpassword = configwifi.wifipassword;
const char* ssid = configwifi.ssid;
const char* password = configwifi.password;

int pin_led = 2;

IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress netmask(255, 255, 255, 0);

// DNS Server untuk Captive Portal - REMOVED untuk save IRAM
// DNSServer dnsServer;
// const byte DNS_PORT = 53;

// Message Queue untuk smooth processing
#define MAX_QUEUE_SIZE 10
struct MessageQueue {
  String messages[MAX_QUEUE_SIZE];
  int head = 0;
  int tail = 0;
  int count = 0;
  
  bool enqueue(String msg) {
    if (count >= MAX_QUEUE_SIZE) {
      Serial.println("[QUEUE] Full! Dropping message.");
      return false;
    }
    messages[tail] = msg;
    tail = (tail + 1) % MAX_QUEUE_SIZE;
    count++;
    return true;
  }
  
  bool dequeue(String &msg) {
    if (count == 0) return false;
    msg = messages[head];
    head = (head + 1) % MAX_QUEUE_SIZE;
    count--;
    return true;
  }
  
  bool isEmpty() { return count == 0; }
  int size() { return count; }
};

MessageQueue msgQueue;

//----------------------------------------------------------------------
// WIFI CONNECTION dengan Auto-Retry & Dual Mode
void wifiConnect() {
  pinMode(pin_led, OUTPUT);
  
  // Clear previous connections
  WiFi.softAPdisconnect(true);
  WiFi.disconnect(true);
  delay(1000);
  
  Serial.println("\n╔════════════════════════════════╗");
  Serial.println("║    WIFI CONNECTION MANAGER    ║");
  Serial.println("╚════════════════════════════════╝");
  
  // Konfigurasi WiFi
  WiFi.persistent(false);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.setAutoReconnect(true);
  
  // Cek apakah ada WiFi credentials yang valid
  bool hasWiFiCredentials = (strlen(netssid) > 0 && strcmp(netssid, "WiFi Kamu") != 0);
  
  if (hasWiFiCredentials) {
    // Mode AP+STA: Coba connect ke WiFi sambil jalankan AP
    Serial.println("→ Mode: AP + Station (Dual Mode)");
    WiFi.mode(WIFI_AP_STA);
    
    // Setup AP dulu
    Serial.println("\n→ Starting Access Point...");
    WiFi.softAPConfig(local_ip, gateway, netmask);
    bool apStarted = WiFi.softAP(ssid, password, 1, 0, 4);
    
    if (apStarted) {
      Serial.println("✓ AP Started!");
      
      // Start DNS Server untuk Captive Portal - REMOVED
      // dnsServer.start(DNS_PORT, "*", local_ip);
      // Serial.println("✓ DNS Server started");
      
      Serial.println("┌────────────────────────────────┐");
      Serial.print("│ AP SSID   : ");
      Serial.print(ssid);
      for(int i = strlen(ssid); i < 20; i++) Serial.print(" ");
      Serial.println("│");
      Serial.print("│ Password  : ");
      Serial.print(password);
      for(int i = strlen(password); i < 20; i++) Serial.print(" ");
      Serial.println("│");
      Serial.print("│ AP IP     : ");
      Serial.print(WiFi.softAPIP());
      int lenAP = WiFi.softAPIP().toString().length();
      for(int i = lenAP; i < 20; i++) Serial.print(" ");
      Serial.println("│");
      Serial.println("└────────────────────────────────┘");
      Serial.println("→ Web UI (AP): http://192.168.4.1/");
    } else {
      Serial.println("✗ AP Failed to start!");
    }
    
    // Coba connect ke WiFi
    Serial.print("\n→ Connecting to WiFi: ");
    Serial.println(netssid);
    WiFi.begin(netssid, netpassword);
    
    unsigned long startTime = millis();
    int dots = 0;
    
    // Timeout lebih lama: 20 detik
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      digitalWrite(pin_led, !digitalRead(pin_led));
      
      if (++dots % 20 == 0) Serial.println();
      if (millis() - startTime > 20000) { // 20 detik
        Serial.println("\n✗ WiFi connection timeout!");
        break;
      }
      
      yield(); // Feed watchdog
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n\n✓ WIFI CONNECTED!");
      Serial.println("┌────────────────────────────────┐");
      Serial.print("│ SSID      : ");
      Serial.print(WiFi.SSID());
      int lenSSID = WiFi.SSID().length();
      for(int i = lenSSID; i < 20; i++) Serial.print(" ");
      Serial.println("│");
      Serial.print("│ IP Address: ");
      Serial.print(WiFi.localIP());
      int len1 = WiFi.localIP().toString().length();
      for(int i = len1; i < 20; i++) Serial.print(" ");
      Serial.println("│");
      Serial.print("│ Gateway   : ");
      Serial.print(WiFi.gatewayIP());
      int len2 = WiFi.gatewayIP().toString().length();
      for(int i = len2; i < 20; i++) Serial.print(" ");
      Serial.println("│");
      Serial.print("│ Subnet    : ");
      Serial.print(WiFi.subnetMask());
      int len3 = WiFi.subnetMask().toString().length();
      for(int i = len3; i < 20; i++) Serial.print(" ");
      Serial.println("│");
      Serial.print("│ RSSI      : ");
      Serial.print(WiFi.RSSI());
      Serial.println(" dBm              │");
      Serial.println("└────────────────────────────────┘");
      Serial.print("\n→ Web UI (WiFi): http://");
      Serial.println(WiFi.localIP());
      digitalWrite(pin_led, HIGH);
    } else {
      Serial.println("\n✗ WiFi connection failed");
      Serial.println("→ AP Mode tetap aktif");
      digitalWrite(pin_led, HIGH);
    }
    
  } else {
    // Mode AP only: Tidak ada WiFi credentials
    Serial.println("→ Mode: Access Point Only");
    WiFi.mode(WIFI_AP);
    
    WiFi.softAPConfig(local_ip, gateway, netmask);
    bool apStarted = WiFi.softAP(ssid, password, 1, 0, 4);
    
    if (apStarted) {
      Serial.println("✓ AP Mode Started!");
      
      // Start DNS Server untuk Captive Portal - REMOVED
      // dnsServer.start(DNS_PORT, "*", local_ip);
      // Serial.println("✓ DNS Server started");
      
      Serial.println("┌────────────────────────────────┐");
      Serial.print("│ AP SSID   : ");
      Serial.print(ssid);
      for(int i = strlen(ssid); i < 20; i++) Serial.print(" ");
      Serial.println("│");
      Serial.print("│ Password  : ");
      Serial.print(password);
      for(int i = strlen(password); i < 20; i++) Serial.print(" ");
      Serial.println("│");
      Serial.print("│ AP IP     : ");
      Serial.print(WiFi.softAPIP());
      int lenAP = WiFi.softAPIP().toString().length();
      for(int i = lenAP; i < 20; i++) Serial.print(" ");
      Serial.println("│");
      Serial.println("└────────────────────────────────┘");
      Serial.println("\n→ Web UI: http://192.168.4.1/");
    } else {
      Serial.println("✗ AP Failed!");
    }
    
    digitalWrite(pin_led, HIGH);
  }
  
  Serial.print("\n→ MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.println("════════════════════════════════\n");
}


ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// Helper untuk compare payload
inline bool payloadEquals(uint8_t *payload, size_t length, const char* str) {
  if (length != strlen(str)) return false;
  return memcmp(payload, str, length) == 0;
}

// Forward declaration
void processMessage(String data);

// Process Message Queue
void processMessageQueue() {
  if (msgQueue.isEmpty()) return;
  
  String msg;
  if (msgQueue.dequeue(msg)) {
    processMessage(msg);
  }
}

// Message Processor (non-blocking)
void processMessage(String data) {
  const char* payload = data.c_str();
  size_t length = data.length();
  
  if (length == 0) return;
  
  // Volume Setting
  if (payload[0] == '#') {
    Disp.drawRect(16, 8, 46, 14, 0, 0);
    String data2 = data.substring(1);
    Serial.print("[WEB] Volume → "); Serial.println(data2);
    handleSettingDfUpdate(data2);
    return;
  }
  
  // Media Controls
  if (payloadEquals((uint8_t*)payload, length, "dplay")) {
    Serial.println("[WEB] ▶ Play");
    mp3.start();
    return;
  }
  if (payloadEquals((uint8_t*)payload, length, "dpause")) {
    Serial.println("[WEB] ⏸ Pause");
    mp3.pause();
    return;
  }
  if (payloadEquals((uint8_t*)payload, length, "dstop")) {
    Serial.println("[WEB] ⏹ Stop");
    mp3.stop();
    return;
  }
  if (payloadEquals((uint8_t*)payload, length, "dnext")) {
    Serial.println("[WEB] ⏭ Next");
    mp3.nextTrack();
    return;
  }
  if (payloadEquals((uint8_t*)payload, length, "dprev")) {
    Serial.println("[WEB] ⏮ Previous");
    mp3.prevTrack();
    return;
  }
  if (payloadEquals((uint8_t*)payload, length, "drandom")) {
    Serial.println("[WEB] 🔀 Random");
    mp3.playRandomTrackFromAll();
    currentTrack = 0;
    return;
  }
  
  // Display Modes
  if (payloadEquals((uint8_t*)payload, length, "dmodemp3")) {
    Serial.println("[WEB] 📻 Mode MP3");
    Disp.clear();
    tmputama = 2;
    return;
  }
  if (payloadEquals((uint8_t*)payload, length, "dmodejam")) {
    Serial.println("[WEB] 🕒 Mode Jam");
    Disp.clear();
    tmputama = 0;
    return;
  }
  if (payloadEquals((uint8_t*)payload, length, "dtesled")) {
    Serial.println("[WEB] 💡 Test LED");
    Disp.clear();
    tmputama = 3;
    return;
  }
  
  // Playlist Selection
  if (payload[0] == 'p') {
    Serial.print("[WEB] Playlist → "); Serial.println(payload);
    int folder = 0;
    
    if (payload[1] == '1') folder = -1; // Random all
    else if (payload[1] >= '2' && payload[1] <= '9') folder = payload[1] - '0';
    else if (payload[1] == 'l' && length >= 3) {
      if (payload[2] >= '1' && payload[2] <= '5') folder = 9 + (payload[2] - '0');
    }
    
    if (folder == -1) {
      mp3.playRandomTrackFromAll();
    } else if (folder > 0) {
      mp3.loopFolder(folder);
    }
    currentTrack = 0;
    return;
  }
  
  // Equalizer
  if (payload[0] == 'e' && length >= 2) {
    Serial.print("[WEB] Equalizer → "); Serial.println(payload);
    switch(payload[1]) {
      case '1': mp3.setEq(DfMp3_Eq_Normal); break;
      case '2': mp3.setEq(DfMp3_Eq_Pop); break;
      case '3': mp3.setEq(DfMp3_Eq_Rock); break;
      case '4': mp3.setEq(DfMp3_Eq_Jazz); break;
      case '5': mp3.setEq(DfMp3_Eq_Classic); break;
      case '6': mp3.setEq(DfMp3_Eq_Bass); break;
    }
    return;
  }
  
  // Date Setting
  if (payload[0] == 't' && length >= 11) {
    uint16_t tahun = ((payload[1]-'0')*1000) + ((payload[2]-'0')*100) + 
                     ((payload[3]-'0')*10) + (payload[4]-'0');
    uint8_t bulan = ((payload[6]-'0')*10) + (payload[7]-'0');
    uint8_t tanggal = ((payload[9]-'0')*10) + (payload[10]-'0');
    
    RtcDateTime now = Rtc.GetDateTime();
    Rtc.SetDateTime(RtcDateTime(tahun, bulan, tanggal, now.Hour(), now.Minute(), 0));
    Serial.println("[WEB] 📅 Date Updated");
    return;
  }
  
  // Time Setting
  if (payload[0] == 'j' && length >= 6) {
    uint8_t jam = ((payload[1]-'0')*10) + (payload[2]-'0');
    uint8_t menit = ((payload[4]-'0')*10) + (payload[5]-'0');
    
    RtcDateTime now = Rtc.GetDateTime();
    Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), jam, menit, 0));
    Serial.println("[WEB] 🕐 Time Updated");
    return;
  }
  
  // Info Update
  if (payload[0] == 'i') {
    String data2 = data.substring(1);
    Serial.println("[WEB] 🕌 Info Updated");
    handleSettingInfoUpdate(data2);
    return;
  }
  
  // Display Settings
  if (payload[0] == 'c') {
    String data2 = data.substring(1);
    Serial.println("[WEB] 🖥 Display Updated");
    handleSettingDispUpdate(data2);
    return;
  }
  
  // JWS Settings
  if (payload[0] == 's') {
    String data2 = data.substring(1);
    Serial.println("[WEB] 🕋 JWS Settings Updated");
    handleSettingJwsUpdate(data2);
    return;
  }
  
  // WiFi Settings
  if (payload[0] == 'w') {
    String data2 = data.substring(1);
    Serial.println("[WEB] 📡 WiFi Updated");
    handleSettingWifiUpdate(data2);
    return;
  }
  
  // Reset Device
  if (payload[0] == 'z') {
    Serial.println("[WEB] ⚠ RESETTING DEVICE...");
    delay(500);
    ESP.restart();
    return;
  }
}

// WebSocket Event Handler
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WS] Client #%u disconnected\n", num);
      break;
      
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[WS] Client #%u connected from %d.%d.%d.%d\n", 
                      num, ip[0], ip[1], ip[2], ip[3]);
      }
      break;
      
    case WStype_TEXT:
      {
        String msg = "";
        for(size_t i = 0; i < length; i++) {
          msg += (char)payload[i];
        }
        
        // Add to queue for processing
        if (!msgQueue.enqueue(msg)) {
          Serial.println("[WS] Queue full! Message dropped.");
        } else {
          Serial.printf("[WS] Queued (%d/10): %s\n", msgQueue.size(), msg.c_str());
        }
      }
      break;
      
    case WStype_ERROR:
      Serial.printf("[WS] Error on client #%u\n", num);
      break;
      
    default:
      break;
  }
}

// XML Handlers
void handleXML() {
  buildXML();
  server.send(200, "text/xml", XML);
}

void handleRealXML() {
  realXML();
  server.send(200, "text/xml", XML);
}

// Process queue in main loop
void processWebMessages() {
  static unsigned long lastProcess = 0;
  unsigned long now = millis();
  
  // Process one message every 50ms to avoid blocking
  if (now - lastProcess >= 50) {
    lastProcess = now;
    processMessageQueue();
  }
}
