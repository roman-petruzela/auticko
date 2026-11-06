// Motory: konfigurace kol, smerovani a pohybove funkce.

// =====================================================
// 1. KONFIGURACE MOTORU
// =====================================================

// in1/in2 = smer, en = PWM rychlost (jumpery ENA/ENB na driverech musi byt sundane).
// invert: kolo se pri in1=HIGH toci dozadu, tak se smer otoci v kodu.
struct Kolo {
    uint8_t in1, in2, en;
    bool invert;
};

const Kolo LZ = {21, 22, 23, true}; // Leve zadni   (levy driver, OUT1/OUT2)
const Kolo LP = {4, 16, 17, true};  // Leve predni  (levy driver, OUT3/OUT4)
const Kolo PZ = {26, 25, 33, true}; // Prave zadni  (pravy driver, OUT1/OUT2)
const Kolo PP = {13, 14, 12, true}; // Prave predni (pravy driver, OUT3/OUT4)

const uint32_t PWM_FREQ = 1000; // Hz
const uint8_t PWM_RES = 8;      // 0-255
const uint8_t RYCHLOST = 255;   // plny vykon; nejslabsi motor (leve zadni) potrebuje rozjezd


// Bezpecnostni zastaveni, kdyz dlouho neprijde prikaz
const unsigned long SAFETY_TIMEOUT = 500;
unsigned long lastCommandTime = 0;
bool motorRunning = false;

void motoryInit() {

    for (const Kolo* k : {&LZ, &LP, &PZ, &PP}) {
        pinMode(k->in1, OUTPUT);
        pinMode(k->in2, OUTPUT);
        ledcAttach(k->en, PWM_FREQ, PWM_RES); // ESP32 core 3.x
    }

    zastav();

}

void motoryBezpecnost() {

    if (motorRunning && millis() - lastCommandTime > SAFETY_TIMEOUT) {

        logInfo("SAFETY", "Timeout! Nebyl prijat prikaz 500 ms. Zastavuji robota.");

        zastav();

    }

}

// =====================================================
// 7. OVLADANI MOTORU
// =====================================================

// in1/in2 = logicky smer (HIGH, LOW = dopredu); invert ho otoci podle zapojeni kola
void ridiKolo(const Kolo& kolo, int in1, int in2) {

    if (kolo.invert) {
        int tmp = in1;
        in1 = in2;
        in2 = tmp;
    }

    digitalWrite(kolo.in1, in1);
    digitalWrite(kolo.in2, in2);
    ledcWrite(kolo.en, (in1 || in2) ? RYCHLOST : 0);

}

// =====================================================
// 8. POHYBOVE FUNKCE
// =====================================================

void zastav() {

    ridiKolo(LZ, LOW, LOW);
    ridiKolo(LP, LOW, LOW);
    ridiKolo(PZ, LOW, LOW);
    ridiKolo(PP, LOW, LOW);

    motorRunning = false;

    logStav("Zastaveno");

}

void dopredu() {

    ridiKolo(LZ, HIGH, LOW);
    ridiKolo(LP, HIGH, LOW);
    ridiKolo(PZ, HIGH, LOW);
    ridiKolo(PP, HIGH, LOW);

    motorRunning = true;

    logStav("Jizda dopredu");

}

void dozadu() {

    ridiKolo(LZ, LOW, HIGH);
    ridiKolo(LP, LOW, HIGH);
    ridiKolo(PZ, LOW, HIGH);
    ridiKolo(PP, LOW, HIGH);

    motorRunning = true;

    logStav("Jizda dozadu");

}

void otocDoprava() {

    ridiKolo(LZ, HIGH, LOW);
    ridiKolo(LP, HIGH, LOW);
    ridiKolo(PZ, LOW, HIGH);
    ridiKolo(PP, LOW, HIGH);

    motorRunning = true;

    logStav("Otaceni doprava");

}

void otocDoleva() {

    ridiKolo(LZ, LOW, HIGH);
    ridiKolo(LP, LOW, HIGH);
    ridiKolo(PZ, HIGH, LOW);
    ridiKolo(PP, HIGH, LOW);

    motorRunning = true;

    logStav("Otaceni doleva");

}

void celeDoprava() {

    ridiKolo(LZ, LOW, HIGH);
    ridiKolo(LP, HIGH, LOW);
    ridiKolo(PZ, HIGH, LOW);
    ridiKolo(PP, LOW, HIGH);

    motorRunning = true;

    logStav("Pohyb doprava");

}

void celeDoleva() {

    ridiKolo(LZ, HIGH, LOW);
    ridiKolo(LP, LOW, HIGH);
    ridiKolo(PZ, LOW, HIGH);
    ridiKolo(PP, HIGH, LOW);

    motorRunning = true;

    logStav("Pohyb doleva");

}

