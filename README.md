# Meeple's Gambit - ESP32 Base

Shared controller hub for the board game. Connects via MQTT to the game server. Handles button inputs for 3 players and provides LCD/audio/LED feedback.

## Player Mapping
| Button | GPIO | Player ID |
|--------|------|-----------|
| 1 (Red) | 22 | `meeple_1` |
| 2 (Green) | 23 | `meeple_2` |
| 3 (Blue) | 13 | `meeple_3` |

## Wiring

### LCD (ADM1602K)
| LCD Pin | Function | ESP32 |
|---------|----------|-------|
| 1 | GND | GND |
| 2 | VDD | 5V |
| 3 | Contrast | GND (2K res) |
| 4 | RS | GPIO 19 |
| 5 | RW | GND |
| 6 | Enable | GPIO 18 |
| 11-14 | D4-D7 | GPIO 17, 16, 4, 21 |
| 15 | LED+ | 5V |
| 16 | LED- | GND |

### Buzzer
GPIO 33 → Buzzer+ / GND → Buzzer−

### Traffic Light LEDs
| Color | GPIO |
|-------|------|
| Red | 25 |
| Yellow | 26 |
| Green | 27 |

## MQTT Topics

**Subscribe:**
- `game/status` – Game state (`WAITING`, `MINIGAME`, etc.)
- `game/display` – LCD update: `{"line1":"...", "line2":"...", "buttons":[1,2,3]}`
- `game/sound` – Sound trigger: `WIN`, `LOSE`, `ROLL`, `MOVE`, `SIGNAL`, `MINIGAME_START`

**Publish:**
- `base/button` – Button press: `{"player":"meeple_1", "button":1, "timestamp":123}`
- `game/connection` – `CONNECTED` on startup
- `game/ack` – Display message acknowledgement

## Setup
1. Edit `include/wifi_manager.h` → Set `WIFI_SSID` and `WIFI_PASS`
2. Edit `include/mqtt_manager.h` → Set `MQTT_BROKER_URL`
3. Flash with PlatformIO: `pio run -t upload`
