# ⚙️ Rover firmware

Every folder is a standalone Arduino sketch (open the `.ino` in the Arduino IDE →
**Arduino Nano / ATmega328P** → Upload).

> **Which one is the real one?** The code base went through two architectures.
> The sketch that matches the project report and was used in the final field
> deployment is:

> ```
> rover_hc05_bluetooth_FINAL/        ← ✅ FINAL (used in deployment)
> ```

> The `xbee_*` sketches are the earlier radio‑link generation, kept for history.

## Sketch map

| Sketch | Status | Comms link | Notes |
|---|---|---|---|
| **`rover_hc05_bluetooth_FINAL/`** | ✅ **FINAL** | HC‑05 @ HW UART D0/D1, 9600 | Full rover: sensor loop + telemetry + motor & parachute‑servo commands. Matches report §4.1 & appendix code. |
| `xbee_rover_enddevice_swserial/` | earlier | XBee @ SW serial D2/RX D3/TX | Rover as XBee end‑device; telemetry forwarded over radio. |
| `xbee_rover_enddevice_hwserial/` | earlier | XBee @ HW UART | Rover variant; NaN‑safe `sendSensorData()`. |
| `xbee_ground_coordinator/` | earlier | XBee @ SW serial D2/RX D3/TX | Ground‑station coordinator: parses telemetry frame, forwards typed commands. |
| `xbee_ground_coordinator_dev/` | dev iteration | same | Coordinator variant that strips CR/LF before forwarding. |
| `component_tests/` | 🧪 bench tests | USB serial | One sketch per sensor — see [`component_tests/README.md`](component_tests/README.md) |

## Required libraries (final firmware)

* `Adafruit DPS310` (Adafruit_DPS310) — barometer
* `Adafruit Unified Sensor` (dependency of the above)
* built‑in: `Servo`, `Wire`, `SPI`

The older XBee sketches need nothing extra beyond `SoftwareSerial` (built‑in).

## Wiring (final firmware — Arduino Nano)

| Function | Pin |
|---|---|
| Motor A IN1 / IN2 (left) | D4 / D5 |
| Motor B IN3 / IN4 (right) | D6 / D8 |
| Parachute SG90 servo | D3 |
| LM35DZ (analog) | A2 |
| MPU6050 I²C | A4 (SDA) / A5 (SCL), addr 0x68 |
| DPS310 SPI | CS=D10 (SCK/MOSI/MISO = 13/11/12) |
| HC‑05 Bluetooth | D0 (RX) / D1 (TX) |
| Motor power | 7.4 V 2S LiPo (direct to driver) |

> ⚠️ The final firmware reads **no PWM/STBY pin** of the driver — wheel state is
> set purely by the four IN pins (fixed full‑speed states). If you want variable
> speed, wire a PWM pin (e.g. D9) to TB6612 PWMA/PWMB and call `analogWrite`.

## Telemetry & command protocol

Streamed every ~1 s on the HC‑05 UART / USB at 9600 baud:

```text
T:26.8,P:1011.2,AX:-0.1,AY:0.1,AZ:1.0,GX:-60.5,GY:-80.0,GZ:50.0,MPU Temp:40.0
```

Commands (single ASCII character in `handleCommand()`):

| Char | Effect |
|---|---|
| `F` / `L` / `R` | drive states (differential) |
| `S` | motors stop + parachute servo → 0° |
| `U` | parachute servo → 90° (**release the parachute**) |

> ⚠️ See the “naming caveat” in the main README §A.5 — Serial labels in
> `handleCommand()` drifted across revisions; trust the code, not the labels.

## Reverting to a different sensor combo

* The `component_tests/` folder has standalone BMP280 / DPS310 / LM35DZ / MPU6050
  sketches if you are re‑building the sensor bench.
* The earlier design (XBee + BMP280) matches `data/launch_day` columns — the
  launch‑day log still records `BMP_Temp`/`Pressure`/`Altitude`.
