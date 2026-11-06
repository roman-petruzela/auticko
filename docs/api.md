# API autíčka (návrh, do nečista)

Přenos: WebSocket `ws://192.168.4.1:81`, textové JSON zprávy. Každá zpráva má pole `t` (typ).
Neznámé typy a pole se ignorují.

## Aplikace → robot
| `t` | Pole | Popis |
|---|---|---|
| `drive` | `x`, `y`, `r` (-1..1) | `x` do boku, `y` dopředu, `r` otáčení. Posílat opakovaně (max. 200 ms mezi zprávami), po 500 ms ticha robot zastaví. |

## Robot → aplikace
| `t` | Pole | Popis |
|---|---|---|
| `hello` | `fw`, `caps[]`, `sensors[]` | Pošle se po připojení, říká co robot umí. |
| `sensors` | `front`, `right`, `rear`, `left` (cm, `null` = nemá hodnotu) | Pravidelně, cca 10× za sekundu. |
| `event` | `msg`, ... | Jednorázové události (např. překážka). |
