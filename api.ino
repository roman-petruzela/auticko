// API: Wi-Fi hotspot, HTTP server, WebSocket a webove rozhrani. Protokol viz docs/api.md.

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

// =====================================================
// 2. WIFI
// =====================================================

const char* ssid = "Robot-Auto";
const char* password = "password123";

IPAddress localIP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

// =====================================================
// 3. SERVERY
// =====================================================

WebServer server(80);
WebSocketsServer webSocket(81);


int previousClients = 0;

const char* posledniStav = "";

// =====================================================
// 5. LOGOVANI
// =====================================================

void logInfo(const char* category, const String& message) {

    Serial.printf(
        "[%10lu ms] [%-10s] %s\n",
        millis(),
        category,
        message.c_str()
    );

}

void logInfo(const char* category, const char* message) {

    Serial.printf(
        "[%10lu ms] [%-10s] %s\n",
        millis(),
        category,
        message
    );

}

void logStav(const char* novyStav) {

    if (strcmp(posledniStav, novyStav) != 0) {

        posledniStav = novyStav;

        logInfo("MOTORY", novyStav);

    }

}

// =====================================================
// 6. HTML ROZHRANI
// =====================================================

const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="cs">

<head>

<meta charset="UTF-8">

<meta name="viewport"
content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">

<title>Robot Control</title>

<style>

* {
    box-sizing: border-box;
    -webkit-tap-highlight-color: transparent;
}

body {
    margin: 0;
    padding: 20px;
    min-height: 100vh;
    background: #10131a;
    color: white;
    font-family: Arial, sans-serif;
    display: flex;
    justify-content: center;
    align-items: center;
    user-select: none;
}

.container {
    width: 100%;
    max-width: 420px;
    text-align: center;
}

.header {
    margin-bottom: 30px;
}

.header h1 {
    font-size: 28px;
    margin-bottom: 8px;
}

.header p {
    color: #858b9a;
    font-size: 13px;
}

.status {
    display: inline-flex;
    align-items: center;
    gap: 8px;
    background: #202632;
    padding: 10px 18px;
    border-radius: 30px;
    font-size: 13px;
    margin-bottom: 35px;
}

.dot {
    width: 9px;
    height: 9px;
    border-radius: 50%;
    background: #ff5252;
}

.dot.connected {
    background: #00e69a;
    box-shadow: 0 0 10px #00e69a;
}

.controls {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: 12px;
    max-width: 330px;
    margin: auto;
}

button {
    border: 1px solid #343c4d;
    background: #202632;
    color: white;
    height: 90px;
    border-radius: 20px;
    font-size: 30px;
    font-weight: bold;
    cursor: pointer;
    touch-action: none;
    transition: background 0.1s, transform 0.1s;
}

button:active,
button.active {
    background: #00c995;
    color: #10131a;
    transform: scale(0.95);
}

.empty {
    visibility: hidden;
}

.stop {
    background: #e63e4b;
    border-color: #e63e4b;
    font-size: 17px;
}

.stop:active {
    background: #ff6670;
}

.instructions {
    margin-top: 35px;
    color: #777f90;
    font-size: 13px;
    line-height: 1.8;
}

.footer {
    margin-top: 30px;
    color: #444c5b;
    font-size: 11px;
}

@media(max-width: 360px) {
    button {
        height: 75px;
    }
}

</style>

</head>

<body>

<div class="container">

    <div class="header">
        <h1>ROBOT CONTROL</h1>
        <p>Bezdratove ovladani ESP32</p>
    </div>

    <div class="status">
        <div id="dot" class="dot"></div>
        <span id="status">Pripojovani...</span>
    </div>

    <div class="controls">

        <button data-command="ROTATE_LEFT">↺</button>
        <button data-command="FORWARD">▲</button>
        <button data-command="ROTATE_RIGHT">↻</button>

        <button data-command="LEFT">◀</button>
        <button class="stop" id="stopButton">STOP</button>
        <button data-command="RIGHT">▶</button>

        <div class="empty"></div>
        <button data-command="BACKWARD">▼</button>
        <div class="empty"></div>

    </div>

    <div class="instructions">
        <b>KLAVESNICE</b><br>
        W / A / S / D - Pohyb<br>
        Q / E - Otaceni<br>
        SPACE - Zastaveni
    </div>

    <div class="footer">
        ESP32 ROBOT SYSTEM
    </div>

</div>

<script>

// =====================================================
// WEBSOCKET
// =====================================================

let ws = null;
let activeCommand = null;
let reconnectTimer = null;

let pressedKeys = new Set();
let keyboardCommand = null;

const statusText = document.getElementById("status");
const statusDot = document.getElementById("dot");

function updateStatus(connected) {

    statusText.textContent =
        connected ? "Pripojeno k robotovi" : "Odpojeno";

    statusDot.classList.toggle("connected", connected);

}

function connectWebSocket() {

    if (ws && (
        ws.readyState === WebSocket.OPEN ||
        ws.readyState === WebSocket.CONNECTING
    )) {
        return;
    }

    clearTimeout(reconnectTimer);

    ws = new WebSocket(
        "ws://" + window.location.hostname + ":81"
    );

    ws.onopen = () => {

        updateStatus(true);

        console.log("[WS] Pripojeno");

        if (activeCommand) {
            send(activeCommand);
        }

    };

    ws.onclose = () => {

        updateStatus(false);

        console.log("[WS] Odpojeno");

        reconnectTimer = setTimeout(
            connectWebSocket,
            1500
        );

    };

    ws.onerror = () => {

        updateStatus(false);

        console.log("[WS] Chyba spojeni");

    };

}

function send(command) {

    if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send(command);
    }

}

function startCommand(command) {

    activeCommand = command;

    send(command);

}

function stopRobot() {

    activeCommand = null;

    send("STOP");

    document.querySelectorAll("button.active")
        .forEach(button => {
            button.classList.remove("active");
        });

}

// =====================================================
// TLAČÍTKA
// =====================================================

document.querySelectorAll("[data-command]").forEach(button => {

    button.addEventListener("pointerdown", event => {

        event.preventDefault();

        button.classList.add("active");

        try {
            button.setPointerCapture(event.pointerId);
        } catch (e) {}

        startCommand(button.dataset.command);

    });

    const release = () => {

        if (!button.classList.contains("active")) return;

        button.classList.remove("active");

        stopRobot();

    };

    button.addEventListener("pointerup", release);
    button.addEventListener("pointercancel", release);
    button.addEventListener("lostpointercapture", release);

});

document.getElementById("stopButton")
    .addEventListener("click", stopRobot);

// =====================================================
// KLAVESNICE
// =====================================================

const keyMap = {

    "w": "FORWARD",
    "ArrowUp": "FORWARD",

    "s": "BACKWARD",
    "ArrowDown": "BACKWARD",

    "a": "LEFT",
    "ArrowLeft": "LEFT",

    "d": "RIGHT",
    "ArrowRight": "RIGHT",

    "q": "ROTATE_LEFT",
    "e": "ROTATE_RIGHT"

};

window.addEventListener("keydown", event => {

    const key = event.key;

    if (key === " ") {

        event.preventDefault();
        stopRobot();
        return;

    }

    const command = keyMap[key];

    if (!command) return;

    event.preventDefault();

    if (pressedKeys.has(key)) return;

    pressedKeys.add(key);
    keyboardCommand = command;

    startCommand(command);

});

window.addEventListener("keyup", event => {

    const key = event.key;

    if (!pressedKeys.has(key)) return;

    pressedKeys.delete(key);

    if (keyMap[key] === keyboardCommand) {

        keyboardCommand = null;
        stopRobot();

    }

});

window.addEventListener("blur", () => {

    pressedKeys.clear();
    keyboardCommand = null;

    stopRobot();

});

// =====================================================
// HEARTBEAT
// =====================================================

setInterval(() => {

    if (activeCommand) {
        send(activeCommand);
    }

}, 150);

// =====================================================
// START
// =====================================================

connectWebSocket();

</script>

</body>
</html>
)rawliteral";

// =====================================================
// 9. WEBSOCKET EVENT
// =====================================================

void webSocketEvent(
    uint8_t num,
    WStype_t type,
    uint8_t * payload,
    size_t length
) {

    if (type == WStype_CONNECTED) {

        IPAddress ip = webSocket.remoteIP(num);

        logInfo(
            "WEBSOCKET",
            "Klient pripojen: ID " + String(num) +
            " | IP: " + ip.toString()
        );

    }

    else if (type == WStype_DISCONNECTED) {

        logInfo(
            "WEBSOCKET",
            "Klient odpojen: ID " + String(num)
        );

        zastav();

    }

    else if (type == WStype_TEXT) {

        String command;
        command.reserve(length);

        for (size_t i = 0; i < length; i++) {
            command += (char)payload[i];
        }

        command.trim();

        logInfo(
            "OVLADANI",
            "Klient " + String(num) +
            " poslal prikaz: " + command
        );

        if (command == "FORWARD") {
            dopredu();
        }

        else if (command == "BACKWARD") {
            dozadu();
        }

        else if (command == "LEFT") {
            celeDoleva();
        }

        else if (command == "RIGHT") {
            celeDoprava();
        }

        else if (command == "ROTATE_LEFT") {
            otocDoleva();
        }

        else if (command == "ROTATE_RIGHT") {
            otocDoprava();
        }

        else if (command == "STOP") {
            zastav();
        }

        else {

            logInfo("WARNING", "Neznamy prikaz!");
            return;

        }

        lastCommandTime = millis();

    }

    else if (type == WStype_ERROR) {

        logInfo("WEBSOCKET", "Chyba WebSocket komunikace");

    }

}

// =====================================================
// 10. HTTP SERVER
// =====================================================

void handleRoot() {

    IPAddress ip = server.client().remoteIP();

    logInfo(
        "HTTP",
        "Webova stranka otevrena | IP: " + ip.toString()
    );

    server.send_P(
        200,
        "text/html; charset=utf-8",
        HTML_PAGE
    );

}

void handleNotFound() {

    IPAddress ip = server.client().remoteIP();

    logInfo(
        "HTTP",
        "Nenalezena stranka | IP: " + ip.toString()
    );

    server.send(
        404,
        "text/plain",
        "404 - Stranka nenalezena"
    );

}


void apiInit() {

    // WIFI

    logInfo("WIFI", "Spoustim Wi-Fi hotspot");

    WiFi.mode(WIFI_AP);

    WiFi.softAPConfig(
        localIP,
        gateway,
        subnet
    );

    bool wifiStarted = WiFi.softAP(
        ssid,
        password,
        11 // pevný kanál; na výchozím kanálu 1 AP nebylo vidět z PC
    );

    if (wifiStarted) {

        logInfo("WIFI", "Hotspot uspesne spusten");

        logInfo("WIFI", "Nazev: Robot-Auto");

        logInfo(
            "WIFI",
            "IP adresa: " + WiFi.softAPIP().toString()
        );

        logInfo(
            "WIFI",
            "Heslo: password123"
        );

    }

    else {

        logInfo("ERROR", "Nepodarilo se spustit Wi-Fi!");

    }

    // HTTP SERVER

    server.on("/", HTTP_GET, handleRoot);

    server.onNotFound(handleNotFound);

    server.begin();

    logInfo("HTTP", "HTTP server spusten na portu 80");

    // WEBSOCKET

    webSocket.begin();

    webSocket.onEvent(webSocketEvent);

    logInfo("WEBSOCKET", "WebSocket spusten na portu 81");


    lastCommandTime = millis();

    previousClients = 0;

}

void apiLoop() {

    server.handleClient();

    webSocket.loop();

    // Sledovani Wi-Fi klientu

    int currentClients = WiFi.softAPgetStationNum();

    if (currentClients != previousClients) {

        logInfo("WIFI", "Pocet pripojenych zarizeni: " + String(currentClients));

        previousClients = currentClients;

    }

}
