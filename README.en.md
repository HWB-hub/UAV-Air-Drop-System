[简体中文](README.md) | **English**

# Two-Wheeled Rover Deployment & Recovery System

A complete **drop → parachute descent → recovery → continue working on the ground** demonstration
system: a self-built fixed-wing UAV carries a 3D-printed **two-wheeled rover (Rover)** up to about
50 m and releases it; the Rover lands safely on a **servo-triggered parachute**, and after landing it
can still be **driven remotely over Bluetooth** while continuously streaming temperature, pressure and
attitude data.

| | |
|---|---|
| **Scope** | Simulates the full real-satellite workflow — launch → deployment → descent → recovery → continued operation — on an amateur budget |
| **What we did** | ① Two-wheeled Rover: mechanical design + 3D printing + electronics + firmware + parachute-bay release mechanism ② Fixed-wing carrier aircraft: airframe, power system and flight controller ③ All ground / airborne testing and data analysis |
| **Key figures** | Parachute-bay release costs about 1/10 of an electromagnetic-lock design (≈ $2 vs $20) · payload ratio 1:4 · ±1.5 m lateral drop accuracy · zero damage over repeated drops |
| **Team** | Huang Wenbo · Kong Xiaosong · Alexander He Wu · Ma Tianchen |

---

## Table of contents

1. [What this system is](#1-what-this-system-is)
2. [What we did](#2-what-we-did)
   - [2.1 Rover · Mechanical design](#21-rover--mechanical-design)
   - [2.2 Rover · Parachute-bay release mechanism (key innovation)](#22-rover--parachute-bay-release-mechanism-key-innovation)
   - [2.3 Rover · Electronics & hardware](#23-rover--electronics--hardware)
   - [2.4 Rover · Firmware & communication protocol](#24-rover--firmware--communication-protocol)
   - [2.5 Wireless iteration: why Bluetooth instead of XBee](#25-wireless-iteration-why-bluetooth-instead-of-xbee)
   - [2.6 Fixed-wing carrier aircraft](#26-fixed-wing-carrier-aircraft)
   - [2.7 Test results & measured data](#27-test-results--measured-data)
   - [2.8 Problems & solutions](#28-problems--solutions)
   - [2.9 Budget](#29-budget)
3. [Repository layout](#3-repository-layout)
4. [Getting started (running the Rover)](#4-getting-started-running-the-rover)
5. [License & team](#5-license--team)

---

## 1. What this system is

Think of it as a **downscaled satellite mission**: a real satellite is carried to orbit by a rocket,
separates in orbit, re-enters and is recovered; we replaced the rocket with a fixed-wing UAV and the
satellite payload with a two-wheeled vehicle, and ran the whole chain end to end — including the fact
that **the payload keeps working after landing**.

The mission chain has four steps:

```text
  ┌──────────────┐ servo bay door opens                                                     ┌────────────────────────┐
  │  FIXED-WING  │ (transmitter Ch.6)                                                       │   ROVER EJECTED FROM   │
  │   AIRCRAFT   │ ─────────────────────────►                                               │   THE BAY, ~50 m AGL   │
  └──────────────┘                                                                          └────────────┬───────────┘
                                                           │  keeps flying forward
                                                           ▼
                                  ┌──────────────────────────────────────────────────┐
                                  │    PARACHUTE RELEASE (servo + steel-rod pin)     │
                                  │  rod withdrawn -> cord loop free -> chute opens  │
                                  └────────────────────────┬─────────────────────────┘
                                                           ▼
                                        ┌────────────────────────────────────┐
                                        │   STABLE DESCENT -> SAFE LANDING   │
                                        │     (30 m / 1.5 m drop tests)      │
                                        └──────────────────┬─────────────────┘
                                                           ▼
                            ┌────────────────────────────────────────────────────────────┐
                            │ AFTER LANDING: Bluetooth HC-05 drive commands (F/B/L/R/S), │
                            │  MPU6050 / DPS310 / LM35 telemetry streaming continuously  │
                            └────────────────────────────────────────────────────────────┘
```

1. **Carry** — the Rover is fixed inside the aircraft's payload bay and climbs to the release altitude with it.
2. **Release** — the pilot flips transmitter channel 6; the servo opens the bay door and the Rover is ejected.
3. **Descent** — the parachute-bay release mechanism fires (see [2.2](#22-rover--parachute-bay-release-mechanism-key-innovation)); the parachute opens and the Rover comes down safely.
4. **Recovery & operation** — after touchdown the Rover measures environmental parameters and can be driven over Bluetooth, completing the "keep working after recovery" stage.

## 2. What we did

### 2.1 Rover · Mechanical design

The body was modelled in **Autodesk Fusion 360** and **3D-printed**. The final material is **PETG** at
**30 %** infill (early prototypes used ceramic resin with a **0.3 mm** layer height; after cracks
appeared in early drop tests we switched to PETG and raised the infill to 30 % to absorb impacts).

Key points of the printed shell:

* **Two wheel bays** sized to the N20 motor axle spacing, with **ventilation holes** for the motor drivers and the battery.
* **Sensor recesses** cut to a **±0.2 mm** tolerance to hold the MPU6050 and the DPS310, keeping the measurement axes identical between builds.
* **Servo bracket** mounted vertically on top of the shell and linked by a welded steel rod to the latch slot; in static testing the pin held **50 g** before releasing.
* **Two 40 mm rubber-tread wheels** with **differential (skid) steering**: climbs **15°** slopes and can pivot on the spot — no steering servo needed.
* **Impact resistance**: the structure survived **10 × 1 m free-fall drops** with no shell cracks and no sensor displacement; the 30 m / 1.5 m parachute drops caused **zero damage**.
* A **fibreglass base plate** stiffens the chassis and serves as the mounting surface for the PCB and the battery.

> 🖼️ The Fusion 360 reference models of the individual parts (Nano, MPU6050, wheels, N20 motors,
> battery, perfboard, servo, XBee, motor-driver module) are in
> [`rover/cad/fusion360_components/`](rover/cad/fusion360_components/).

### 2.2 Rover · Parachute-bay release mechanism (key innovation)

This is the most distinctive part of the Rover: **a single micro servo plus a steel-rod pin replaces
the usual electromagnetic lock**, removing almost all of the cost and the circuit complexity.

```text
   Parachute nylon cord (loop)
           │
           ▼
  ┌─────────────────┐
  │    servo arm    │   the steel rod passes through the loop,
  │    (SG90, D3)   │──► held in place by a self-made latch
  └────────┬────────┘
           │  servo rotates 90°  ("U" command over Bluetooth)
           ▼
  ┌─────────────────┐
  │  rod withdrawn  │──► cord loop free, the parachute is released
  └─────────────────┘
           │  relative airflow / rover inertia pulls the chute out
           ▼
        parachute deploys
```

How it works:

1. A **captive steel rod** is fixed to the SG90 servo arm and locked by a self-made latch; the rod
   **passes through a loop in the parachute's nylon cord**, **holding the folded parachute captive**.
2. To release, the Arduino sweeps the servo through **90°** (send `U` over Bluetooth); the arm rotates
   and **withdraws the rod from the cord loop**.
3. The parachute is now free. The Rover keeps flying and the chute is pulled out and inflated by
   **relative airflow / inertia** — no pyrotechnics, no springs, no extra actuator.

Why this design was chosen:

| Metric | Electromagnetic lock | Servo + steel rod (this project) |
|---|---|---|
| Part cost | ≈ $20 | **≈ $2 (≈ 90 % cheaper)** |
| Extra electronics | Driver circuit + latch coil | None — one ordinary servo, wired straight to the Arduino |
| Reliability (tests) | — | ≈ 98 % successful deployments in the 30 m / 1.5 m drop tests |
| Failure mode | Latch jams / coil overheats | Servo stalls → reset it; the chute stays held (fail-safe) |

### 2.3 Rover · Electronics & hardware

The main controller is an **Arduino Nano (ATmega328P)**. The two **N20 gear motors** are driven by a
**TB6612FNG dual H-bridge**; its two channels are wired to **four digital pins** (IN1/IN2 + IN3/IN4),
so each wheel can turn forwards or backwards on its own, giving differential steering. Motor power is
taken straight from the **7.4 V (2S) LiPo** to keep the high current off the logic rail.

Wiring of the final firmware (`rover_hc05_bluetooth_FINAL`):

| Function | Pin | Notes |
|---|---|---|
| Motor A (left) IN1 / IN2 | **D4 / D5** | to TB6612FNG Motor A |
| Motor B (right) IN3 / IN4 | **D6 / D8** | to TB6612FNG Motor B |
| Parachute SG90 servo (signal) | **D3** | PWM, 0° → 90° releases the chute |
| LM35DZ (analog temperature) | **A2** | 10 mV/°C, `V = reading×5/1023`, `T = V×100` |
| MPU6050 (I²C) | **A4 (SDA) / A5 (SCL)** | address `0x68`, raw values /16384 (accel), /131 (gyro) |
| DPS310 barometer | **SPI, CS = D10** | `dps.begin_SPI(10)` (Adafruit_DPS310) |
| HC-05 Bluetooth | **D0 (RX) / D1 (TX)** | hardware UART, 9600 baud |
| Serial debug | USB | same 9600 baud stream |

**Power feasibility estimate (worst-case current budget of the electronics):**

| Part | Current |
|---|---|
| MPU6050 | 5 mA |
| BMP280 | 5 mA |
| LM35DZ | 5 mA |
| XBee-PRO S1 | 50 mA |
| TB6612FNG | 10 mA |
| Arduino Nano | 50 mA |
| N20 gear motor ×2 | 150 mA each |
| **Total** | **≈ 475 mA** |

This estimate confirmed that the 7.4 V (2S) LiPo plus a 5 V regulator can supply the whole board
reliably.

### 2.4 Rover · Firmware & communication protocol

All Arduino code lives in [`rover/firmware/`](rover/firmware/). Every folder is a **standalone Arduino
sketch**: open the `.ino` in the Arduino IDE, select **Arduino Nano / ATmega328P** and upload over USB.

| Sketch | Status | Link | Notes |
|---|---|---|---|
| `rover_hc05_bluetooth_FINAL` | ✅ **final version — the one used in the field deployment** | HC-05 Bluetooth, hardware UART D0/D1 | Sensor acquisition + telemetry output + motor control + parachute servo commands |
| `xbee_rover_enddevice_swserial` | ⚙️ earlier (XBee) | XBee on software serial D2/D3 | Rover as an XBee end device; data forwarded over the radio |
| `xbee_rover_enddevice_hwserial` | ⚙️ earlier (XBee) | XBee on hardware UART | End-device variant (adds protection when printing NaN) |
| `xbee_ground_coordinator` | ⚙️ earlier (XBee) | XBee on software serial D2/D3 | Ground-station coordinator: receives telemetry, forwards typed commands to the Rover |
| `xbee_ground_coordinator_dev` | ⚙️ development iteration | same | Coordinator variant (strips CR/LF before forwarding) |
| `component_tests/*` | 🧪 component tests | USB serial | Single-part test sketches for BMP280 / DPS310 / LM35DZ / MPU6050 |

**Telemetry frame** (about 1 frame per second on the HC-05 UART / USB, 9600 baud):

```text
T:<°C>,P:<hPa>,AX:<g>,AY:<g>,AZ:<g>,GX:<°/s>,GY:<°/s>,GZ:<°/s>,MPU Temp:<°C>
```

**Drive / release commands** — the Rover reads a **single ASCII character** in `handleCommand()`,
so any Bluetooth serial app paired to the HC-05 can send them:

| Char | Effect (final firmware) |
|---|---|
| `F` | forward — both motors driven in the same direction |
| `B` | backward — both motors driven in the opposite direction |
| `L` | turn left on the spot — left wheel reversed, right wheel forward |
| `R` | turn right on the spot — left wheel forward, right wheel reversed |
| `S` | both wheels stop and the parachute servo returns to 0° |
| `U` | servo sweeps to 90° → **releases the parachute** |

### 2.5 Wireless iteration: why Bluetooth instead of XBee

The first wireless design used **XBee-PRO S1** radio modems (with a ground coordinator board), but in
real tests the XBee node was **unstable during sleep/wake-up** and **dropped incoming RF data** in the
field, so the link was replaced with an **HC-05 Bluetooth** module (pairs straight with a phone, no
separate ground coordinator hardware needed).

| | XBee design (early) | Bluetooth design (final) |
|---|---|---|
| Radio | **XBee-PRO S1** ↔ ground coordinator | **HC-05** ↔ phone serial app |
| Barometer | **BMP280** | **DPS310** (higher precision) |
| Field behaviour | ❌ unstable during sleep/wake-up, dropped RF data in tests | ✅ stable, command latency < 0.5 s |
| Ground station | needs a separate coordinator board + a PC | none — a phone pairs directly |

### 2.6 Fixed-wing carrier aircraft

The carrier is a self-built fixed-wing aircraft: the airframe is **laser-cut PP (magic) board** bonded
with electronic adhesive, with **carbon-fibre rods** reinforcing the wings and fuselage and steel-wire
pushrods as control linkages. Power and avionics:

| Part | Spec / role |
|---|---|
| **A2212 KV1400** brushless motor | 2–3S, ≈ 47 g, 3.17 mm shaft |
| **40 A ESC ×2** | twin powerplants, differential thrust |
| **4S 2200 mAh 45C LiPo** | main power |
| **RF209S** 9-channel receiver | PWM / PPM / S.BUS output |
| **Servos ×7** | ailerons (2), elevator, rudder, bay door, etc. |
| **PID flight controller** | pre-tuned on a PC, provides stability assist |

Receiver channel map used for the drop mission:

| Channel | Use |
|---|---|
| Ch1 | aileron servos ×2 (paralleled) |
| Ch2 | elevator |
| Ch3 & Ch5 | twin ESCs (differential thrust) |
| Ch4 | rudder |
| **Ch6** | **bay door — releases the Rover** |
| **Ch9** | PID flight controller on/off |

The pilot flips Ch6 to open the bay door; the Rover is released at about 50 m and then follows the
parachute recovery sequence.

### 2.7 Test results & measured data

* **Drop / recovery**: two kinds of drop tests, 30 m and 1.5 m — reliable deployments (≈ 98 %), no PETG shell damage, **zero damage** over repeated drops.
* **Drop accuracy**: released from about 50 m, **±1.5 m** lateral landing accuracy; aircraft payload ratio **1:4**.
* **Sensors**:
  * MPU6050: accel ±2 g (±0.05 g), gyro ±250 °/s (±3 %); in the field data the Z-axis acceleration changes by about 1.5 g at the moment the chute opens.
  * DPS310: 300–1200 hPa range (±1 hPa), calibrated altitude error **±1.5 m** (≈ ±1.2 m when fused); on the ground it outputs a stable ≈ 1011 hPa.
  * LM35DZ: measured stable over −10 … +40 °C in the field.
* **Comms**: HC-05 command latency **< 0.5 s** (measured response within about 0.3 s); the Rover ↔ ground station link was verified end to end.

### 2.8 Problems & solutions

| Problem | Root cause | Solution |
|---|---|---|
| Bluetooth signal loss | EMI from the motor wiring | Shielded motor wiring + a ferrite bead on the HC-05 |
| Sensor drift at low temperature | LM35 noise | 0.1 µF capacitor across the LM35 output |
| Shell cracking | Weak layer bonding in the 3D print | Infill raised to 30 % + switched to PETG |
| Battery overheating | Continuous high current | Heat sinks + ventilation holes in the shell |
| DPS310 measurement error | Barometric calibration drift | Real-time (Kalman-style) filtering of the pressure |

### 2.9 Budget

**Budget (≈ ¥798 in total):**

| Rover electronics & structure | Price (¥) | Carrier aircraft | Price (¥) |
|---|---|---|---|
| XBee-PRO (early design) | 150 | flight-control board | 100 |
| MPU6050 | 15 | brushless motor | 80 |
| BMP280 | 5 | ESC ×2 | 100 |
| LM35DZ | 3 | servos ×7 | 35 |
| N20 gear motor ×2 | 15 | 2600 mAh battery | 100 |
| 328PB circuit board | 25 | shell | 50 |
| Fibreglass base plate | 25 | — | — |
| Battery | 15 | — | — |
| 3D-printing material | 40 | — | — |
| Parachute material | 40 | — | — |
| Servos (purchase) | 5 | — | — |

Equipment rental and the test site were free. (The XBee was later swapped for the HC-05, see
[2.5](#25-wireless-iteration-why-bluetooth-instead-of-xbee).)

## 3. Repository layout

```text
.
├── README.md                  ← Chinese version
├── README.en.md               ← English version (this file)
├── LICENSE                    MIT licence
├── data/
│   └── launch_day/            Full telemetry log recorded on launch day (.xlsx, 1517 samples / ≈ 120 s)
├── rover/                     ← the two-wheeled Rover
│   ├── firmware/              Arduino code: final Bluetooth firmware + XBee-era rover / ground-station code
│   │   ├── rover_hc05_bluetooth_FINAL/     ✅ final firmware used in the field deployment
│   │   ├── xbee_rover_enddevice_*/         ⚙️ early XBee rover end devices
│   │   ├── xbee_ground_coordinator*/       ⚙️ early XBee ground stations
│   │   └── component_tests/                🧪 single-part tests for BMP280 / DPS310 / LM35DZ / MPU6050
│   ├── cad/fusion360_components/           Fusion 360 reference models of the individual parts
│   └── media/                 Photos and videos: assembly / bench tests / field deployment
└── fixed_wing_aircraft/       ← the fixed-wing carrier aircraft
    ├── cad_models/            Airframe CAD / structural drawings
    ├── docs/                  Wiring, channel maps and other documents
    ├── firmware_flight_controller/   Flight-controller firmware and tuning configuration
    └── media/                 Photos and videos
```

## 4. Getting started (running the Rover)

**Download and run the final firmware:**

1. Open `rover/firmware/rover_hc05_bluetooth_FINAL/rover_hc05_bluetooth_FINAL.ino` in the Arduino IDE.
2. Install the libraries: **Adafruit DPS310**, **Adafruit Unified Sensor**; `Servo`, `Wire` and `SPI` are built in.
   (If you want to go back to the earlier BMP280 test, also install `Adafruit BMP280`.)
3. Select the board **Arduino Nano**, processor ATmega328P, programmer AVRISP mkII / USB.
4. Wire it up as in the table in [2.3](#23-rover--electronics--hardware), upload, open the Serial Monitor
   (**9600 baud**) — telemetry should stream at about 1 Hz.
5. Power the motors from the **7.4 V (2S) LiPo** and the logic from USB / a 5 V regulator.
6. Pair a Bluetooth serial app on your phone with the HC-05 and send `F` / `B` / `L` / `R` / `S` / `U`
   characters to drive the Rover and to release the parachute.

> ⚠️ **Safety**: `U` releases the parachute — bench-test the servo **with the parachute detached and no
> load** first, and stay clear of the spinning wheels and the ejected rod while powered.

## 5. License & team

MIT licence, see [`LICENSE`](LICENSE).

**Team**: Huang Wenbo · Kong Xiaosong · Alexander He Wu · Ma Tianchen

*“Deploy. Recover. Innovate.”*
