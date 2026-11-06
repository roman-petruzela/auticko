// Hlavni soubor: jen setup a loop. Kod je v zalozkach motory.ino, api.ino, senzory.ino.

// Deklarace funkci ze zalozek (automaticke prototypy Arduina tu nefunguji)
void motoryInit();
void motoryBezpecnost();
void zastav();
void dopredu();
void dozadu();
void otocDoleva();
void otocDoprava();
void celeDoleva();
void celeDoprava();
void apiInit();
void apiLoop();
extern unsigned long lastCommandTime;

void setup() {

    Serial.begin(115200);

    delay(500);

    Serial.println();
    Serial.println("========================================");
    Serial.println("          ROBOT ESP32 SYSTEM");
    Serial.println("========================================");

    motoryInit();

    apiInit();

    Serial.println();
    Serial.println("========================================");
    Serial.println("             SYSTEM READY");
    Serial.println("========================================");
    Serial.println("Wi-Fi: Robot-Auto");
    Serial.println("Adresa: http://192.168.4.1");
    Serial.println("Serial: 115200 baud");
    Serial.println("========================================");

}

void loop() {

    apiLoop();

    motoryBezpecnost();

}
