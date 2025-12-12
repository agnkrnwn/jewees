const char webpage[] PROGMEM = 
R"=====(
<!DOCTYPE HTML>
<html lang="id">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>JWS Murottal V4</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Arial, sans-serif;
            background: #0a0e27;
            color: #e0e0e0;
            line-height: 1.6;
        }
        
        .container {
            max-width: 900px;
            margin: 0 auto;
            padding: 15px;
        }
        
        /* Header */
        .header {
            background: #1a1f3a;
            border-left: 4px solid #4CAF50;
            padding: 20px;
            margin-bottom: 20px;
        }
        
        .header h1 {
            font-size: 1.8rem;
            color: #4CAF50;
            margin-bottom: 5px;
        }
        
        .header p {
            color: #888;
            font-size: 0.9rem;
        }
        
        .status {
            display: inline-block;
            padding: 6px 12px;
            border-radius: 4px;
            font-size: 0.85rem;
            margin-top: 10px;
            font-weight: 600;
        }
        
        .status.online { background: #2e7d32; color: #fff; }
        .status.offline { background: #c62828; color: #fff; }
        
        /* Dashboard */
        .stats {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
            gap: 15px;
            margin-bottom: 20px;
        }
        
        .stat {
            background: #1a1f3a;
            padding: 15px;
            text-align: center;
            border-left: 3px solid #2196F3;
        }
        
        .stat-value {
            font-size: 1.5rem;
            font-weight: bold;
            color: #4CAF50;
            font-family: 'Courier New', monospace;
        }
        
        .stat-label {
            color: #888;
            font-size: 0.85rem;
            text-transform: uppercase;
            margin-top: 5px;
        }
        
        /* Section */
        .section {
            background: #1a1f3a;
            padding: 20px;
            margin-bottom: 15px;
        }
        
        .section-title {
            font-size: 1.2rem;
            color: #4CAF50;
            margin-bottom: 15px;
            padding-bottom: 10px;
            border-bottom: 2px solid #263238;
        }
        
        /* Form */
        .form-group {
            margin-bottom: 15px;
        }
        
        label {
            display: block;
            color: #aaa;
            font-size: 0.85rem;
            margin-bottom: 5px;
            text-transform: uppercase;
            font-weight: 600;
        }
        
        input, select, textarea {
            width: 100%;
            padding: 10px;
            background: #0a0e27;
            border: 1px solid #333;
            color: #e0e0e0;
            font-size: 0.95rem;
        }
        
        input:focus, select:focus, textarea:focus {
            outline: none;
            border-color: #4CAF50;
        }
        
        /* Buttons */
        .btn {
            padding: 12px 20px;
            border: none;
            cursor: pointer;
            font-size: 0.9rem;
            font-weight: 600;
            margin: 5px 5px 5px 0;
            transition: opacity 0.2s;
        }
        
        .btn:hover { opacity: 0.8; }
        .btn-primary { background: #4CAF50; color: #fff; }
        .btn-secondary { background: #2196F3; color: #fff; }
        .btn-danger { background: #f44336; color: #fff; }
        .btn-warning { background: #ff9800; color: #fff; }
        .btn-full { width: 100%; margin-top: 10px; }
        
        /* Media Controls */
        .media-controls {
            display: flex;
            justify-content: center;
            gap: 10px;
            margin: 20px 0;
        }
        
        .media-btn {
            width: 50px;
            height: 50px;
            border: 2px solid #333;
            background: #0a0e27;
            color: #4CAF50;
            font-size: 1.2rem;
            cursor: pointer;
            transition: all 0.2s;
        }
        
        .media-btn:hover {
            background: #4CAF50;
            color: #fff;
            border-color: #4CAF50;
        }
        
        .media-btn.main {
            width: 60px;
            height: 60px;
            font-size: 1.5rem;
            background: #4CAF50;
            color: #fff;
            border-color: #4CAF50;
        }
        
        /* Prayer Table */
        .prayer-table {
            width: 100%;
            border-collapse: collapse;
        }
        
        .prayer-table td {
            padding: 12px;
            border-bottom: 1px solid #263238;
        }
        
        .prayer-table td:first-child {
            color: #aaa;
            font-weight: 600;
        }
        
        .time-badge {
            background: #4CAF50;
            color: #fff;
            padding: 6px 15px;
            border-radius: 3px;
            font-family: 'Courier New', monospace;
            font-weight: bold;
        }
        
        /* Grid */
        .grid-2 { display: grid; grid-template-columns: 1fr 1fr; gap: 15px; }
        .grid-3 { display: grid; grid-template-columns: 1fr 1fr 1fr; gap: 15px; }
        
        /* Toast */
        #toast {
            position: fixed;
            bottom: -100px;
            left: 50%;
            transform: translateX(-50%);
            background: #4CAF50;
            color: #fff;
            padding: 15px 30px;
            border-radius: 4px;
            font-weight: 600;
            transition: bottom 0.3s;
            z-index: 1000;
        }
        
        #toast.show { bottom: 30px; }
        
        /* Responsive */
        @media (max-width: 600px) {
            .grid-2, .grid-3 { grid-template-columns: 1fr; }
            .header h1 { font-size: 1.5rem; }
            .media-btn { width: 45px; height: 45px; font-size: 1rem; }
            .media-btn.main { width: 55px; height: 55px; font-size: 1.3rem; }
        }
        
        .divider {
            height: 1px;
            background: #263238;
            margin: 20px 0;
        }
    </style>
</head>
<body onload="init()">
    
    <div class="container">
        
        <!-- Header -->
        <div class="header">
            <h1>🕌 JWS Murottal</h1>
            <p>Smart Mosque Controller V4.0</p>
            <div id="status" class="status offline">Connecting...</div>
        </div>
        
        <!-- Dashboard -->
        <div class="stats">
            <div class="stat">
                <div class="stat-value" id="clock">00:00:00</div>
                <div class="stat-label">Waktu</div>
            </div>
            <div class="stat">
                <div class="stat-value" id="date">-- ---</div>
                <div class="stat-label">Tanggal</div>
            </div>
            <div class="stat">
                <div class="stat-value"><span id="temp">--</span>°C</div>
                <div class="stat-label">Suhu</div>
            </div>
        </div>
        
        <!-- Audio Player -->
        <div class="section">
            <div class="section-title">🎵 Audio Player</div>
            
            <div class="form-group">
                <label>Volume (0-30)</label>
                <input type="number" min="0" max="30" value="15" id="volume">
            </div>
            <button class="btn btn-primary btn-full" onclick="sendVolume()">Set Volume</button>
            
            <div class="media-controls">
                <button class="media-btn" onclick="sendPrev()">⏮</button>
                <button class="media-btn" onclick="sendStop()">⏹</button>
                <button class="media-btn main" onclick="sendPlay()">▶</button>
                <button class="media-btn" onclick="sendPause()">⏸</button>
                <button class="media-btn" onclick="sendNext()">⏭</button>
            </div>
            
            <button class="btn btn-secondary btn-full" onclick="sendRandom()">🔀 Random</button>
            
            <div class="grid-2" style="margin-top: 15px;">
                <div class="form-group">
                    <label>Playlist</label>
                    <select id="playlist">
                        <option value="p1">Al Quran - All</option>
                        <option value="p2">Saad Alghamdi</option>
                        <option value="p3">Juz Amma - Saad</option>
                        <option value="p4">Mishary Rasyid</option>
                        <option value="p5">Juz Amma - Mishary</option>
                        <option value="p6">As Sudais</option>
                        <option value="p7">Juz Amma - Sudais</option>
                        <option value="p8">Al Budair</option>
                        <option value="p9">Juz Amma - Budair</option>
                        <option value="pl1">Al Muaiqly</option>
                        <option value="pl2">Juz Amma - Muaiqly</option>
                        <option value="pl3">Al Hudhaifi</option>
                        <option value="pl4">Juz Amma - Hudhaifi</option>
                        <option value="pl5">Lain-lain</option>
                    </select>
                </div>
                <div class="form-group">
                    <label>Equalizer</label>
                    <select id="equalizer">
                        <option value="e1">Normal</option>
                        <option value="e2">Pop</option>
                        <option value="e3">Rock</option>
                        <option value="e4">Jazz</option>
                        <option value="e5">Classic</option>
                        <option value="e6">Bass</option>
                    </select>
                </div>
            </div>
            <button class="btn btn-primary btn-full" onclick="sendAudioSettings()">Apply Settings</button>
        </div>
        
        <!-- Display Mode -->
        <div class="section">
            <div class="section-title">🖥️ Display</div>
            
            <button class="btn btn-primary" onclick="sendModeJam()">🕒 Jam</button>
            <button class="btn btn-secondary" onclick="sendModeMP3()">📻 Audio</button>
            <button class="btn btn-warning" onclick="sendTesLED()">💡 Test</button>
            
            <div class="grid-2" style="margin-top: 15px;">
                <div class="form-group">
                    <label>Kecerahan (15-254)</label>
                    <input type="number" min="15" max="254" value="100" id="kecerahan">
                </div>
                <div class="form-group">
                    <label>Panel P10</label>
                    <select id="panel">
                        <option value="1">1 Panel</option>
                        <option value="2">2 Panel</option>
                    </select>
                </div>
            </div>
            <button class="btn btn-primary btn-full" onclick="sendKecerahan()">Apply Display</button>
        </div>
        
        <!-- Info Masjid -->
        <div class="section">
            <div class="section-title">🕌 Info Masjid</div>
            
            <div class="form-group">
                <label>Nama Masjid</label>
                <input type="text" id="nama" placeholder="Nama Masjid">
            </div>
            <div class="form-group">
                <label>Running Text 1</label>
                <textarea id="info1" rows="2"></textarea>
            </div>
            <div class="form-group">
                <label>Running Text 2</label>
                <textarea id="info2" rows="2"></textarea>
            </div>
            <button class="btn btn-primary btn-full" onclick="sendNama()">Simpan Info</button>
        </div>
        
        <!-- Jadwal Sholat -->
        <div class="section">
            <div class="section-title">🕋 Jadwal Sholat</div>
            
            <table class="prayer-table">
                <tr><td>Tanbih</td><td><span id="jtanbih" class="time-badge">--:--</span></td></tr>
                <tr><td>Subuh</td><td><span id="jsubuh" class="time-badge">--:--</span></td></tr>
                <tr><td>Dzuhur</td><td><span id="jdzuhur" class="time-badge">--:--</span></td></tr>
                <tr><td>Ashar</td><td><span id="jashar" class="time-badge">--:--</span></td></tr>
                <tr><td>Maghrib</td><td><span id="jmaghrib" class="time-badge">--:--</span></td></tr>
                <tr><td>Isya</td><td><span id="jisya" class="time-badge">--:--</span></td></tr>
            </table>
            
            <div style="margin-top: 20px;">
                <label>Waktu Iqomah (Menit)</label>
                <div class="grid-3" style="margin-top: 10px;">
                    <div class="form-group"><label>Subuh</label><input type="number" id="iqmhs" placeholder="12"></div>
                    <div class="form-group"><label>Dzuhur</label><input type="number" id="iqmhd" placeholder="8"></div>
                    <div class="form-group"><label>Ashar</label><input type="number" id="iqmha" placeholder="6"></div>
                    <div class="form-group"><label>Maghrib</label><input type="number" id="iqmhm" placeholder="5"></div>
                    <div class="form-group"><label>Isya</label><input type="number" id="iqmhi" placeholder="5"></div>
                    <div class="form-group"><label>Durasi Adzan</label><input type="number" id="durasiadzan" placeholder="1"></div>
                </div>
            </div>
            <button class="btn btn-primary btn-full" onclick="setJws()">Simpan Jadwal</button>
        </div>
        
        <!-- Lokasi & Waktu -->
        <div class="section">
            <div class="section-title">📍 Lokasi & Waktu</div>
            
            <div class="grid-2">
                <div class="form-group">
                    <label>Set Tanggal</label>
                    <input type="date" id="tanggal">
                </div>
                <div class="form-group">
                    <label>Set Jam</label>
                    <input type="time" id="jam">
                </div>
            </div>
            <button class="btn btn-primary btn-full" onclick="sendTanggalJam()">Set Waktu</button>
            
            <div class="grid-2" style="margin-top: 15px;">
                <div class="form-group"><label>Latitude</label><input type="number" step="any" id="latitude" placeholder="-6.16"></div>
                <div class="form-group"><label>Longitude</label><input type="number" step="any" id="longitude" placeholder="106.61"></div>
                <div class="form-group"><label>Zona Waktu</label><input type="number" id="zonawaktu" placeholder="7"></div>
                <div class="form-group"><label>Koreksi (menit)</label><input type="number" id="ihti" placeholder="2"></div>
            </div>
            <button class="btn btn-primary btn-full" onclick="setJws()">Simpan Lokasi</button>
        </div>
        
        <!-- WiFi Settings -->
        <div class="section">
            <div class="section-title">📡 WiFi</div>
            
            <label style="color: #4CAF50; font-size: 1rem;">Station Mode</label>
            <div class="form-group">
                <label>SSID WiFi</label>
                <input type="text" id="wifissid" placeholder="Nama WiFi">
            </div>
            <div class="form-group">
                <label>Password</label>
                <input type="password" id="wifipassword" placeholder="Password">
            </div>
            
            <div class="divider"></div>
            
            <label style="color: #4CAF50; font-size: 1rem;">Access Point Mode</label>
            <div class="form-group">
                <label>Hotspot Name</label>
                <input type="text" id="ssid" placeholder="JWS">
            </div>
            <div class="form-group">
                <label>Hotspot Password</label>
                <input type="password" id="password" placeholder="Min. 8 karakter">
            </div>
            
            <button class="btn btn-primary btn-full" onclick="setWifi()">Simpan WiFi</button>
            <button class="btn btn-danger btn-full" onclick="resetDevice()">⚠️ Restart</button>
        </div>
        
        <!-- System -->
        <div class="section">
            <div class="section-title">⚙️ System</div>
            <button class="btn btn-warning" onclick="location.href='/update'">🚀 OTA Update Firmware</button>
            <button class="btn btn-secondary" onclick="downloadBackup()">💾 Backup Config</button>
            <button class="btn btn-warning" onclick="document.getElementById('restore').click()">📤 Restore Config</button>
            <input type="file" id="restore" style="display:none" accept=".json" onchange="restoreConfig(this)">
            <p style="margin-top: 15px; color: #888; font-size: 0.85rem;">
                💡 Tips: Backup config sebelum update firmware<br>
                📶 Free Heap: <span id="heap">--</span> KB
            </p>
        </div>
        
        <div style="text-align: center; padding: 20px; color: #666;">
            <p>JWS Murottal V4.0 - ESP8266</p>
        </div>
        
    </div>
    
    <!-- Toast -->
    <div id="toast">✓ Saved!</div>
    
    <script>
        let ws;
        let wsConnected = false;
        let reconnectAttempts = 0;
        let maxReconnectAttempts = 10;
        let xmlHttp = new XMLHttpRequest();
        
        function init() {
            console.log('🚀 JWS V4 Init');
            initWebSocket();
            startDataSync();
        }
        
        let messageQueue = [];
        let isSending = false;
        
        function initWebSocket() {
            try {
                ws = new WebSocket('ws://' + window.location.hostname + ':81/');
                
                ws.onopen = () => {
                    console.log('✓ WS Connected');
                    wsConnected = true;
                    reconnectAttempts = 0;
                    updateStatus(true);
                    toast('Connected');
                    processQueue();
                };
                
                ws.onclose = () => {
                    console.log('✗ WS Disconnected');
                    wsConnected = false;
                    updateStatus(false);
                    
                    if (reconnectAttempts < maxReconnectAttempts) {
                        reconnectAttempts++;
                        let delay = Math.min(1000 * Math.pow(2, reconnectAttempts - 1), 30000);
                        setTimeout(initWebSocket, delay);
                    }
                };
                
                ws.onerror = (e) => {
                    console.error('WS Error:', e);
                    updateStatus(false);
                };
                
                ws.onmessage = (e) => console.log('📩', e.data);
                
            } catch(e) {
                console.error('WS Init Error:', e);
                reconnectAttempts++;
                if (reconnectAttempts < maxReconnectAttempts) {
                    setTimeout(initWebSocket, 2000);
                }
            }
        }
        
        function sendWS(data) {
            messageQueue.push(data);
            processQueue();
        }
        
        function processQueue() {
            if (!wsConnected || isSending || messageQueue.length === 0) return;
            
            isSending = true;
            const message = messageQueue.shift();
            
            try {
                ws.send(message);
                console.log('📤', message);
                setTimeout(() => {
                    isSending = false;
                    processQueue();
                }, 100);
            } catch(e) {
                console.error('Send error:', e);
                messageQueue.unshift(message);
                isSending = false;
            }
        }
        
        function updateStatus(online) {
            const el = document.getElementById('status');
            if (online) {
                el.className = 'status online';
                el.textContent = 'Online';
            } else {
                el.className = 'status offline';
                el.textContent = 'Offline';
            }
        }
        
        function toast(msg) {
            const t = document.getElementById('toast');
            t.textContent = msg;
            t.className = 'show';
            setTimeout(() => t.className = '', 3000);
        }
        
        function startDataSync() {
            syncConfig();
            syncRealtime();
            setInterval(syncRealtime, 1000);
        }
        
        function syncConfig() {
            if (xmlHttp.readyState == 0 || xmlHttp.readyState == 4) {
                xmlHttp.open('GET', 'xml', true);
                xmlHttp.onreadystatechange = handleConfig;
                xmlHttp.send(null);
            }
        }
        
        function syncRealtime() {
            if (xmlHttp.readyState == 0 || xmlHttp.readyState == 4) {
                xmlHttp.open('GET', 'realxml', true);
                xmlHttp.onreadystatechange = handleRealtime;
                xmlHttp.send(null);
            }
        }
        
        function handleConfig() {
            if (xmlHttp.readyState == 4 && xmlHttp.status == 200) {
                const xml = xmlHttp.responseXML;
                const getVal = (tag) => {
                    const el = xml.getElementsByTagName(tag);
                    return el.length > 0 ? el[0].firstChild.nodeValue : '';
                };
                
                const fields = ['nama', 'info1', 'info2', 'iqmhs', 'iqmhd', 'iqmha', 'iqmhm', 'iqmhi',
                               'durasiadzan', 'ihti', 'latitude', 'longitude', 'zonawaktu', 'panel',
                               'ssid', 'password', 'wifissid', 'wifipassword'];
                
                fields.forEach(id => {
                    const el = document.getElementById(id);
                    if (el && document.activeElement !== el) {
                        const val = getVal('r' + id.charAt(0).toUpperCase() + id.slice(1));
                        if (val) el.value = val;
                    }
                });
                
                ['jtanbih', 'jsubuh', 'jdzuhur', 'jashar', 'jmaghrib', 'jisya'].forEach(id => {
                    const val = getVal('r' + id.charAt(1).toUpperCase() + id.slice(2));
                    if (val) document.getElementById(id).textContent = val;
                });
            }
        }
        
        function handleRealtime() {
            if (xmlHttp.readyState == 4 && xmlHttp.status == 200) {
                const xml = xmlHttp.responseXML;
                const getVal = (tag) => {
                    const el = xml.getElementsByTagName(tag);
                    return el.length > 0 ? el[0].firstChild.nodeValue : '';
                };
                
                const h = getVal('rHour');
                const m = getVal('rMinute');
                const s = getVal('rSecond');
                document.getElementById('clock').textContent = `${h}:${m}:${s}`;
                
                const d = getVal('rDay');
                const mo = getVal('rMonth');
                const y = getVal('rYear');
                const months = ['Jan','Feb','Mar','Apr','Mei','Jun','Jul','Agt','Sep','Okt','Nov','Des'];
                document.getElementById('date').textContent = `${d} ${months[parseInt(mo)-1] || ''} ${y}`;
                
                document.getElementById('temp').textContent = getVal('rTemp');
                
                const volEl = document.getElementById('volume');
                if (document.activeElement !== volEl) {
                    const vol = getVal('rVolume');
                    if (vol) volEl.value = vol;
                }
                
                const kecEl = document.getElementById('kecerahan');
                if (document.activeElement !== kecEl) {
                    const kec = getVal('rKecerahan');
                    if (kec) kecEl.value = kec;
                }
                
                // Update heap
                const heap = getVal('rHeap');
                if (heap) document.getElementById('heap').textContent = (parseInt(heap)/1024).toFixed(1);
            }
        }
        
        // Control Functions
        function sendVolume() {
            const vol = Math.min(30, Math.max(0, parseInt(document.getElementById('volume').value)));
            document.getElementById('volume').value = vol;
            sendWS('#{"volume":' + vol + '}');
            toast('Volume: ' + vol);
        }
        
        function sendPlay() { sendWS('dplay'); toast('Play'); }
        function sendPause() { sendWS('dpause'); toast('Pause'); }
        function sendStop() { sendWS('dstop'); toast('Stop'); }
        function sendNext() { sendWS('dnext'); toast('Next'); }
        function sendPrev() { sendWS('dprev'); toast('Previous'); }
        function sendRandom() { sendWS('drandom'); toast('Random'); }
        
        function sendAudioSettings() {
            const playlist = document.getElementById('playlist').value;
            const eq = document.getElementById('equalizer').value;
            sendWS(playlist);
            setTimeout(() => sendWS(eq), 150);
            toast('Settings Applied');
        }
        
        function sendModeJam() { sendWS('dmodejam'); toast('Mode Jam'); }
        function sendModeMP3() { sendWS('dmodemp3'); toast('Mode Audio'); }
        function sendTesLED() { sendWS('dtesled'); toast('Test LED'); }
        
        function sendKecerahan() {
            const kec = Math.min(254, Math.max(15, parseInt(document.getElementById('kecerahan').value)));
            const panel = document.getElementById('panel').value;
            document.getElementById('kecerahan').value = kec;
            sendWS('c{"kecerahan":' + kec + ',"panel":' + panel + '}');
            toast('Display Updated');
        }
        
        function sendNama() {
            const data = {
                nama: document.getElementById('nama').value,
                info1: document.getElementById('info1').value,
                info2: document.getElementById('info2').value
            };
            sendWS('i' + JSON.stringify(data));
            toast('Info Saved');
        }
        
        function setJws() {
            const data = {
                iqmhs: parseInt(document.getElementById('iqmhs').value) || 12,
                iqmhd: parseInt(document.getElementById('iqmhd').value) || 8,
                iqmha: parseInt(document.getElementById('iqmha').value) || 6,
                iqmhm: parseInt(document.getElementById('iqmhm').value) || 5,
                iqmhi: parseInt(document.getElementById('iqmhi').value) || 5,
                durasiadzan: parseInt(document.getElementById('durasiadzan').value) || 1,
                ihti: parseInt(document.getElementById('ihti').value) || 2,
                latitude: parseFloat(document.getElementById('latitude').value) || -6.16,
                longitude: parseFloat(document.getElementById('longitude').value) || 106.61,
                zonawaktu: parseInt(document.getElementById('zonawaktu').value) || 7
            };
            sendWS('s' + JSON.stringify(data));
            toast('JWS Config Saved');
        }
        
        function sendTanggalJam() {
            const tgl = document.getElementById('tanggal').value;
            const jam = document.getElementById('jam').value;
            if (tgl) {
                sendWS('t' + tgl.replace(/-/g, ''));
                setTimeout(() => {
                    if (jam) sendWS('j' + jam.replace(':', ''));
                }, 150);
                toast('Time Updated');
            }
        }
        
        function setWifi() {
            const data = {
                wifissid: document.getElementById('wifissid').value,
                wifipassword: document.getElementById('wifipassword').value,
                ssid: document.getElementById('ssid').value,
                password: document.getElementById('password').value
            };
            sendWS('w' + JSON.stringify(data));
            toast('WiFi Saved. Restart to apply.');
        }
        
        function resetDevice() {
            if (confirm('Restart device?')) {
                sendWS('z');
                toast('Restarting...');
                setTimeout(() => location.reload(), 5000);
            }
        }
        
        function downloadBackup() {
            fetch('/backup')
                .then(r => r.blob())
                .then(blob => {
                    const url = URL.createObjectURL(blob);
                    const a = document.createElement('a');
                    a.href = url;
                    a.download = 'jws_backup_' + Date.now() + '.json';
                    a.click();
                    toast('Backup Downloaded');
                })
                .catch(e => toast('Backup Failed'));
        }
        
        function restoreConfig(input) {
            if (input.files.length > 0) {
                const file = input.files[0];
                const reader = new FileReader();
                reader.onload = (e) => {
                    try {
                        const config = JSON.parse(e.target.result);
                        fetch('/restore', {
                            method: 'POST',
                            headers: {'Content-Type': 'application/json'},
                            body: JSON.stringify(config)
                        })
                        .then(r => r.text())
                        .then(() => {
                            toast('Restored! Restarting...');
                            setTimeout(() => location.reload(), 3000);
                        })
                        .catch(() => toast('Restore Failed'));
                    } catch(err) {
                        toast('Invalid File');
                    }
                };
                reader.readAsText(file);
            }
        }
    </script>
</body>
</html>

)=====";
