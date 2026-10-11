# 🧪 Component bench-test sketches

One Arduino sketch per sensor, used while validating each part before integration.
Each is a standalone, self‑contained sketch that prints to the USB serial monitor
(9600 baud).

| Sketch | Sensor under test | Interface | Output |
|---|---|---|---|
| `BMP280_test/` | BMP280 barometer/temp | I²C (0x76 / 0x77) | temperature (°C), pressure (hPa) |
| `DPS310_test/` | DPS310 barometer/temp | **SPI, CS=D10** | temperature (°C), pressure (hPa) |
| `LM35DZ_test/` | LM35DZ analog temp | analog (A0 in sketch) | temperature (°C), 10 mV/°C |
| `MPU6050_test/` | MPU6050 6‑axis IMU | I²C (0x68), register‑level | accel (g) ×3, gyro (°/s) ×3, die temp (°C) |

> Notes
> * The MPU6050 sketch reads registers directly (wake‑up via PWR_MGMT_1, raw
>   ACCEL/GYRO/TEMP registers) — no MPU6050 library needed.
> * The DPS310 sketch is exactly the barometer init used in the rover main
>   firmware (`begin_SPI(CS)` @ 64 Hz / 64 samples).
> * The BMP280 test is the *earlier* barometer (see main README electronics
>   history) — the launch‑day log still contains `BMP_Temp` data.
> * `LM35DZ_test` reads A0; the final rover uses A2. Change the pin if you wire it
>   that way.
