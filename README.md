# Projects STC — SWAN LAB, IIT Kharagpur

A progressive series of embedded systems projects covering Arduino and ESP32, from a simple blinking LED to a live IoT sensor dashboard over MQTT.

---

## Project Index

| # | File | Board | Description |
|---|------|-------|-------------|
| 1 | `arduino-led-blink.ino` | Arduino | LED blink |
| 2 | `ldr-led-arduino.ino` | Arduino | LDR auto-light with LED |
| 3 | `test-relay-arduino.ino` | Arduino | Relay module test |
| 4 | `relay-ldr-arduino.ino` | Arduino | LDR auto-light with relay |
| 5 | `esp32-led-blink.ino` | ESP32 | LED blink |
| 6 | `led-ldr-esp32.ino` | ESP32 | LDR auto-light with LED |
| 7 | `ldr-led-esp32-mqtt.ino` + `actuate-ldr.py` | ESP32 + Python | LDR → MQTT → live graph |

---

## 1 · Arduino LED Blink

**File:** `arduino-led-blink.ino`

The classic "Hello, World!" for embedded systems. Blinks an LED on and off every 1.5 seconds using `digitalWrite` and `delay`.

### Hardware

| Component | Connection |
|-----------|------------|
| LED + 220Ω resistor | Pin 9 → GND |

### How it works

`setup()` configures Pin 9 as OUTPUT. `loop()` drives it HIGH (on) for 1500 ms then LOW (off) for 1500 ms, repeating forever.

---

## 2 · Arduino LDR Auto-Light

**File:** `ldr-led-arduino.ino`

Reads ambient light from an LDR on analog pin A0. When the reading falls below a threshold, the LED on Pin 12 switches ON automatically.

### Hardware

| Component | Connection |
|-----------|------------|
| LDR | One leg → 5V, other leg → A0 |
| 10 kΩ resistor | A0 → GND (pull-down) |
| LED + 220Ω resistor | Pin 12 → GND |

### Wiring note

The LDR and resistor form a voltage divider. As light increases, resistance drops and voltage at A0 rises (0 = dark, 1023 = bright).

### Calibration

Open Serial Monitor at 9600 baud. Cover and uncover the LDR and note the values. Set `DARK_THRESHOLD` between the dark and bright readings.

---

## 3 · Relay Module Test

**File:** `test-relay-arduino.ino`

A minimal sketch to verify that the 5V relay module is wired correctly and to confirm whether it is active HIGH or active LOW.

### Hardware

| Component | Connection |
|-----------|------------|
| 5V relay module IN | Pin 8 |
| Relay module VCC | 5V |
| Relay module GND | GND |

### How to use

Upload the sketch. Watch the onboard relay LED and listen for an audible click.

- If the relay clicks ON when the sketch writes `LOW` → the module is **active LOW** (most common).
- If it clicks ON when the sketch writes `HIGH` → the module is **active HIGH**.

Note the result and use the correct logic in Project 4.

---

## 4 · Arduino LDR + Relay Auto-Light

**File:** `relay-ldr-arduino.ino`

Extends Project 2 by replacing the indicator LED with a 10A relay, allowing a high-power load (bulb, fan, pump) to be switched automatically based on light level.

### Hardware

| Component | Connection |
|-----------|------------|
| LDR | One leg → 5V, other leg → A0 |
| 10 kΩ resistor | A0 → GND (pull-down) |
| LED + 220Ω resistor | Pin 12 → GND (indicator) |
| Relay module IN | Pin 8 |
| Relay module VCC | 5V |
| Relay module GND | GND |
| Load | Between relay COM and NO terminals |

### Safety

- The relay module's COM + NO terminals are on the **high-voltage side** — keep this completely isolated from the Arduino side.
- Always use the NO (Normally Open) terminal so the load is OFF by default if the Arduino loses power.
- The module used here is **active LOW**: `LOW` = relay ON, `HIGH` = relay OFF. This is handled with `#define RELAY_ON LOW` in the code.

---

## 5 · ESP32 LED Blink

**File:** `esp32-led-blink.ino`

The same blink concept as Project 1, rebuilt for ESP32. Uses GPIO2 which is also the onboard LED on most ESP32 DevKit boards, so no external components are strictly required.

### Hardware

| Component | Connection |
|-----------|------------|
| LED + 220Ω resistor | GPIO2 → GND (or use onboard LED) |

### Key differences from Arduino

- Serial baud rate is 115200 (not 9600).
- GPIO2 maps to the onboard LED on most DevKit variants.

---

## 6 · ESP32 LDR Auto-Light

**File:** `led-ldr-esp32.ino`

Reads an LDR on GPIO34 (a dedicated ADC-only input pin) and controls an LED on GPIO2. Both pins sit on the **same side** of the ESP32 DevKit board, keeping all wiring on one rail.

### Hardware

| Component | Connection |
|-----------|------------|
| LDR | One leg → 3.3V, other leg → GPIO34 |
| 10 kΩ resistor | GPIO34 → GND (pull-down) |
| LED + 220Ω resistor | GPIO2 → GND |

### Pin layout (right side of DevKit, USB at bottom)

```
... GPIO26
    GPIO25
    GPIO34  ← LDR signal
    GPIO35
    GPIO2   ← LED
    GPIO4
...
```

GPIO34 is input-only with no internal pull resistor — the external 10kΩ is mandatory.

### Calibration

The ESP32 ADC is 12-bit (0–4095). Open Serial Monitor at 115200, cover and expose the LDR, then set `DARK_THRESHOLD` between your observed dark and bright values.

---

## 7 · ESP32 LDR + MQTT + Live Graph

**Files:** `ldr-led-esp32-mqtt.ino` · `actuate-ldr.py`

Extends Project 6 with WiFi and MQTT publishing. The ESP32 reads the LDR every 500 ms, controls the LED locally, and publishes a JSON payload to an MQTT broker. The Python script subscribes to that topic and plots the data as a live scrolling graph.

### Hardware

Same as Project 6 — no additional components needed. WiFi runs on-chip.

### MQTT topics

| Topic | Direction | Payload |
|-------|-----------|---------|
| `swan/ldr/data` | ESP32 → broker | `{"ldr": 312, "state": "DARK", "threshold": 150, "uptime_ms": 12345}` |
| `swan/ldr/status` | ESP32 → broker | `"online"` / `"offline"` (last-will) |

### ESP32 setup

1. Install libraries via Arduino Library Manager:
   - **PubSubClient** by Nick O'Leary
   - **ArduinoJson** by Benoit Blanchon
2. Fill in WiFi credentials in the sketch:
   ```cpp
   const char* WIFI_SSID     = "YOUR_WIFI_SSID";
   const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
   ```
3. The default broker is `broker.hivemq.com` port 1883 (free, no account needed). To run a local broker: `pip install mosquitto`.
4. Upload and open Serial Monitor at 115200 to verify WiFi + MQTT connection.

### Python setup

```bash
pip install paho-mqtt matplotlib
python actuate-ldr.py
```

The live graph:
- Y-axis: 0–400 (LDR value range for this environment).
- X-axis: scrolling time in seconds.
- Red dashed line: dark threshold reference.
- Background turns dark red when state is DARK, dark green when LIGHT.

### Graph configuration

To change the display range or threshold, edit the top of `actuate-ldr.py`:

```python
DARK_THRESHOLD = 150    # Must match value in ESP32 sketch
MAX_POINTS     = 100    # Number of data points visible at once
UPDATE_MS      = 200    # Graph refresh rate in milliseconds
```

---

## Dependencies summary

### Arduino IDE

| Library | Used in | Install via |
|---------|---------|-------------|
| PubSubClient | Project 7 | Arduino Library Manager |
| ArduinoJson | Project 7 | Arduino Library Manager |

### Python

```bash
pip install paho-mqtt matplotlib
```

---

## Credits

Coded by **SWAN LAB, IIT Kharagpur**
