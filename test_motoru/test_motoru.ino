/*
  TEST MOTORŮ – mecanum autíčko
  ESP32 DevKit V1 (30 pinů) + 2× L298N

  Zapojení:
    Levý driver:  přední levý  = OUT3/OUT4 (IN3=D4,  IN4=D16, ENB=D17)
                  zadní levý   = OUT1/OUT2 (IN1=D21, IN2=D22, ENA=D23)
    Pravý driver: přední pravý = OUT3/OUT4 (IN3=D13, IN4=D14, ENB=D12)
                  zadní pravý  = OUT1/OUT2 (IN1=D26, IN2=D25, ENA=D33)
    Červený drát motoru je vždy v lichém OUT.
    Jumpery ENA/ENB na driverech MUSÍ být sundané.

  Postup:
    1. Vyndej baterky z 4× AA (ESP32 jede z USB), 8× AA zapni.
    2. Zvedni auto, aby kola visela ve vzduchu.
    3. Otevři Serial Monitor na 115200 baud a posílej příkazy (viz menu).
*/

#include <Arduino.h>

struct Motor {
  const char* name;
  uint8_t inOdd;   // IN1 nebo IN3 – HIGH = "+" na červený drát
  uint8_t inEven;  // IN2 nebo IN4
  uint8_t en;      // ENA nebo ENB – PWM rychlost
};

Motor motors[4] = {
  {"predni levy",  4,  16, 17},
  {"zadni levy",   21, 22, 23},
  {"predni pravy", 13, 14, 12},
  {"zadni pravy",  26, 25, 33},
};

const uint32_t PWM_FREQ = 1000;   // Hz
const uint8_t  PWM_RES  = 8;      // 0–255
const unsigned long RUN_MS = 2000; // jak dlouho motor běží v jednom směru

int speedPct = 80;                // rychlost v procentech

// ---------- PWM (funguje s ESP32 Arduino core 2.x i 3.x) ----------
void pwmSetup(uint8_t index) {
  uint8_t pin = motors[index].en;
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(pin, PWM_FREQ, PWM_RES);
  ledcWrite(pin, 0);
#else
  ledcSetup(index, PWM_FREQ, PWM_RES);
  ledcAttachPin(pin, index);
  ledcWrite(index, 0);
#endif
}

void pwmWrite(uint8_t index, uint8_t duty) {
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(motors[index].en, duty);
#else
  ledcWrite(index, duty);
#endif
}

// ---------- Ovládání motoru ----------
// dir: 1 = lichý IN HIGH (+ na červený), -1 = opačně, 0 = stop
void setMotor(uint8_t i, int dir) {
  if (dir == 0) {
    pwmWrite(i, 0);
    digitalWrite(motors[i].inOdd, LOW);
    digitalWrite(motors[i].inEven, LOW);
    return;
  }
  digitalWrite(motors[i].inOdd,  dir > 0 ? HIGH : LOW);
  digitalWrite(motors[i].inEven, dir > 0 ? LOW  : HIGH);
  pwmWrite(i, map(speedPct, 0, 100, 0, 255));
}

void stopAll() {
  for (uint8_t i = 0; i < 4; i++) setMotor(i, 0);
}

void testMotor(uint8_t i) {
  Serial.printf("\n>> Motor %d: %s (rychlost %d %%)\n", i + 1, motors[i].name, speedPct);

  Serial.println("   Smer A (+ na cerveny drat) ... sleduj, kam se kolo toci");
  setMotor(i, 1);
  delay(RUN_MS);
  setMotor(i, 0);
  delay(700);

  Serial.println("   Smer B (opacne)");
  setMotor(i, -1);
  delay(RUN_MS);
  setMotor(i, 0);

  Serial.println("   Hotovo.");
}

void testAll(int dir) {
  Serial.printf("\n>> Vsechny motory soucasne, smer %s (rychlost %d %%)\n",
                dir > 0 ? "A" : "B", speedPct);
  for (uint8_t i = 0; i < 4; i++) setMotor(i, dir);
  delay(RUN_MS);
  stopAll();
  Serial.println("   Hotovo.");
}

void printMenu() {
  Serial.println("\n===== TEST MOTORU =====");
  Serial.println(" 1 = predni levy");
  Serial.println(" 2 = zadni levy");
  Serial.println(" 3 = predni pravy");
  Serial.println(" 4 = zadni pravy");
  Serial.println(" a = vsechny smer A");
  Serial.println(" b = vsechny smer B");
  Serial.println(" + / - = rychlost o 10 %");
  Serial.println(" s = stop");
  Serial.println(" m = toto menu");
  Serial.printf(" Aktualni rychlost: %d %%\n", speedPct);
}

void setup() {
  // Nejdřív bezpečně vypnout EN piny, aby motory při startu necukly
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(motors[i].en, OUTPUT);
    digitalWrite(motors[i].en, LOW);
    pinMode(motors[i].inOdd, OUTPUT);
    pinMode(motors[i].inEven, OUTPUT);
    digitalWrite(motors[i].inOdd, LOW);
    digitalWrite(motors[i].inEven, LOW);
    pwmSetup(i);
  }

  Serial.begin(115200);
  delay(500);
  printMenu();
}

void loop() {
  if (!Serial.available()) return;

  char c = Serial.read();
  switch (c) {
    case '1': case '2': case '3': case '4':
      testMotor(c - '1');
      break;
    case 'a': testAll(1);  break;
    case 'b': testAll(-1); break;
    case '+':
      speedPct = min(100, speedPct + 10);
      Serial.printf("Rychlost: %d %%\n", speedPct);
      break;
    case '-':
      speedPct = max(0, speedPct - 10);
      Serial.printf("Rychlost: %d %%\n", speedPct);
      break;
    case 's':
      stopAll();
      Serial.println("STOP");
      break;
    case 'm': printMenu(); break;
    default: break; // ignoruje Enter a jiné znaky
  }
}
