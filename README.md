# Vývoj softwaru a oprava robotického autíčka

Tento repozitář je přehledem a archivem mé maturitní práce, na které právě pracuji, a podkladů k její obhajobě.

Navazuji na starší maturitní práci, která se zaměřovala hlavně na hardware a stavbu auta. Já se soustředím především na vývoj softwaru a k tomu na opravu a vylepšení hardwaru.

Podklady původní práce jsou ve složce [`roboticke_auto/`](roboticke_auto/). Samotný text původní práce je z důvodu ochrany soukromí vynechán.

![Autíčko](images/auticko.jpg)

## O autíčku

Autíčko vzniklo v roce 2023 jako maturitní práce *Robotické auto* jiného studenta naší školy. Jezdí na čtyřech DC motorech s mecanum koly, takže umí jet dopředu, dozadu, do stran i šikmo a otáčet se na místě. Původně se ovládalo přes Bluetooth mobilní aplikací Dabble a na každé straně mělo ultrazvukový senzor, který ho zastavil před překážkou.

V původní práci jsou kola popsaná jako omni, ve skutečnosti jde ale o mecanum kola. Pro software je to podstatné, protože se podle toho jinak počítá, jak rychle a kterým směrem se má každé kolo točit.

## Co jsem změnil

- **Nová řídicí deska.** Původní deska ESP-WROOM-32 (38 pinů, micro USB) po zkratu přestala fungovat. Nahradil jsem ji deskou ESP32 DevKit V1 (30 pinů, USB-C, převodník CH340).
- **Nové zapojení pinů.** Piny jsou rozmístěné podle toho, kde je daná součástka na autě: přední součástky na přední části desky, zadní na zadní, levé vlevo a pravé vpravo.
- **Opravený pravý senzor.** Původně měl TRIG na GPIO 39, který je na ESP32 pouze vstupní, takže senzor nemohl nikdy fungovat.
- **Regulace rychlosti.** Každý motor má nově zapojený i EN pin s PWM, takže lze řídit rychlost. Dříve motory jely buď naplno, nebo vůbec.
- **Dva drivery.** Levé motory jsou na levém L298N, pravé na pravém.

## Hardware

| Součástka | Počet | Poznámka |
|---|---|---|
| ESP32 DevKit V1 (30 pinů, USB-C) | 1 | náhrada za původní desku |
| Driver L298N (HW-095) | 2 | levá a pravá strana |
| DC motor s mecanum kolem | 4 | |
| Ultrazvukový senzor HC-SR04 | 4 | vpředu, vzadu, vlevo, vpravo |
| Držák 8× AA s vypínačem | 1 | napájení motorů (12 V) |
| Držák 4× AA | 1 | napájení ESP32 (6 V) |

## Zapojení

Deska leží v autě součástkami nahoru a USB-C míří dopředu. Řada pinů VIN … EN je tedy na pravé straně auta a řada 3V3 … D23 na levé. Názvy pinů odpovídají popiskům na desce (D13 = GPIO 13, VP = GPIO 36, VN = GPIO 39).

### Motory

| Motor | Driver | Svorky | Směr | Rychlost |
|---|---|---|---|---|
| Přední levý | levý | OUT3 / OUT4 | IN3 ← D4, IN4 ← D16 | ENB ← D17 |
| Zadní levý | levý | OUT1 / OUT2 | IN1 ← D21, IN2 ← D22 | ENA ← D23 |
| Přední pravý | pravý | OUT3 / OUT4 | IN3 ← D13, IN4 ← D14 | ENB ← D12 |
| Zadní pravý | pravý | OUT1 / OUT2 | IN1 ← D26, IN2 ← D25 | ENA ← D33 |

### Senzory

| Senzor | TRIG | ECHO |
|---|---|---|
| Přední | D15 | D2 |
| Pravý | D27 | D35 |
| Zadní | D32 | VN |
| Levý | D5 | D18 |

VCC senzorů je napájené ze svorky +5V levého driveru.

> **Senzory zatím nefungují.** Senzor posílá na ECHO 5 V, ale vstupy ESP32 snesou jen 3,3 V, takže mezi ně patří dělič napětí (1 kΩ + 2 kΩ). Rezistory zatím nemám, proto ECHO vodiče končí v breadboardu a dál nevedou.

### Napájení

- 8× AA (+) → vypínač → +12V na obou driverech
- 8× AA (−) → GND na obou driverech
- 4× AA (+) → VIN na ESP32, (−) → GND na ESP32
- GND ESP32 je propojená s GND driverů (společná zem)

Před nahráváním programu přes USB je potřeba vyndat baterie z držáku 4× AA, aby se napájení z USB a z baterií navzájem neovlivňovalo.

## Plány

Zatím je hotový hardware a testovací program. Na softwaru chci dál vyvíjet:

- **Aplikaci pro ovládání**, která nahradí Dabble – buď webovou, nebo pro Android.
- **Práci se senzory** – hlídání překážek a zobrazení vzdáleností v aplikaci.
- **Mapu okolí** – možná zkusím z dat senzorů sestavit jednoduchou mapu.

Na hardwaru ještě plánuji:

- doplnit děliče napětí na ECHO piny, aby senzory fungovaly,
- přidat kondenzátor k ESP32 proti restartům při rozjezdu motorů,
- případně nahradit AA baterie článkem Li-ion 3S, protože AA baterie motory příliš neutáhnou.

## 3D modely

Moje vlastní držáky jsou ve složce [`modely/`](modely/) (`middlepart.stl`, `sw-holder-mk2.stl`, `uss-6V-holder.stl`). Modely původního auta zůstávají beze změny v [`roboticke_auto/modely/`](roboticke_auto/modely/).

## Struktura repozitáře

| Soubor / složka | Obsah |
|---|---|
| `test_motoru.ino` | test motorů jeden po druhém přes Serial Monitor |
| `images/` | fotky autíčka |
| `modely/` | moje 3D modely (STL) |
| `roboticke_auto/` | podklady původního auta, beze změn |

## Poznámka k AI

Všechny dosavadní `.ino` soubory v tomto repozitáři jsem vygeneroval pomocí AI. Slouží jen k otestování hardwaru a nejsou finálním softwarem autíčka.
