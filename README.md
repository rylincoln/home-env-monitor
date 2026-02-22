# Home Environment Monitor

A CC3220SF-based indoor environment monitoring system that tracks temperature, humidity, air quality, CO levels, and ambient light — with a local Raspberry Pi dashboard and CO safety alarms.

## Sensors

| Sensor       | Measures                                   | Interface                    |
| ------------ | ------------------------------------------ | ---------------------------- |
| BME280       | Temperature, humidity, barometric pressure | I2C `0x76`                   |
| CCS811       | eCO2, TVOC (indoor air quality)            | I2C `0x5A`                   |
| MQ-7         | Carbon monoxide (20-2000 ppm)              | ADC (via voltage divider)    |
| BH1750       | Ambient light (1-65535 lux)                | I2C `0x23`                   |
| Piezo Buzzer | CO alarm output                            | GPIO (via 2N2222 transistor) |

## Architecture

```
CC3220SF LaunchPad          Raspberry Pi (LAN)
┌─────────────────┐         ┌──────────────────────┐
│  BME280 ─┐      │  MQTT   │  Mosquitto (broker)  │
│  CCS811 ─┤ I2C  │────────>│  Telegraf → InfluxDB │
│  BH1750 ─┘      │  JSON   │  Grafana (dashboard) │
│  MQ-7 ── ADC    │         └──────────────────────┘
│  Buzzer ─ GPIO  │
└─────────────────┘
```

Sensor data is sampled, converted to engineering units, and published as JSON to a local Mosquitto MQTT broker every 30 seconds. Telegraf ingests the MQTT stream into InfluxDB, and Grafana renders live charts on the Pi's display.

If CO exceeds 50 ppm, the buzzer activates and a `CO_ALERT` flag is added to the MQTT payload. The alarm clears when CO drops below 25 ppm (hysteresis).

## Repository Contents

- **`project.html`** -- Full project design document (open in a browser): system architecture, bill of materials, wiring diagrams, firmware code, Raspberry Pi dashboard setup (Docker Compose), and CO safety logic.
- **`env_monitor_enclosure.scad`** -- Parametric OpenSCAD 3D-printable enclosure with snap-fit lid, ventilation grille, sensor mounts, and wall-mount keyholes.

## Enclosure

The enclosure is a two-part snap-fit design (base + lid) sized at 140 x 100 x 38 mm. It includes:

- M2.5 standoffs for the LaunchPad and all sensor breakout boards
- Tall standoffs for the MQ-7 (heater clearance)
- Ventilation grille over the sensor bay
- Light sensor window, buzzer sound port, USB cutout
- Wall-mount keyhole slots

**Print settings:** PETG, 0.2 mm layers, 20% gyroid infill, supports enabled.

Render parts in OpenSCAD by setting `RENDER_PART` to `"base"`, `"lid"`, or `"both"` (exploded view).

## Bill of Materials

See `project.html` for full part descriptions, wiring details, and estimated costs.

> **Note:** The CCS811 has been discontinued by AMS. The [Adafruit SGP30](https://www.adafruit.com/product/3709) ($17.50, in stock) is the recommended drop-in replacement for eCO2/TVOC sensing over I2C.

| Component                                                                                                   | Description                                          | Price  | Source                                                                                                                                                                                                                                                                                                |
| ----------------------------------------------------------------------------------------------------------- | ---------------------------------------------------- | ------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [CC3220SF-LAUNCHXL](https://www.digikey.com/en/products/detail/texas-instruments/CC3220SF-LAUNCHXL/6660400) | TI SimpleLink Wi-Fi LaunchPad (MCU)                  | ~$70   | [DigiKey](https://www.digikey.com/en/products/detail/texas-instruments/CC3220SF-LAUNCHXL/6660400), [Mouser](https://www.mouser.com/ProductDetail/Texas-Instruments/CC3220SF-LAUNCHXL?qs=KuuZdrM3jLxt7xOTmPb+qA%3D%3D), [Arrow](https://www.arrow.com/en/products/cc3220sf-launchxl/texas-instruments) |
| [BME280](https://www.adafruit.com/product/2652)                                                             | Temp/humidity/pressure sensor breakout (I2C)         | $14.95 | [Adafruit](https://www.adafruit.com/product/2652) (in stock), [SparkFun](https://www.sparkfun.com/sparkfun-atmospheric-sensor-breakout-bme280.html)                                                                                                                                                   |
| [SGP30](https://www.adafruit.com/product/3709)                                                              | eCO2/TVOC air quality sensor (I2C) — replaces CCS811 | $17.50 | [Adafruit](https://www.adafruit.com/product/3709) (in stock), [Amazon](https://www.amazon.com/Adafruit-SGP30-Quality-Sensor-Breakout/dp/B07L5YN11R)                                                                                                                                                   |
| [MQ-7](https://www.sparkfun.com/carbon-monoxide-sensor-mq-7.html)                                           | Carbon monoxide sensor module (analog)               | ~$7.50 | [SparkFun SEN-09403](https://www.sparkfun.com/carbon-monoxide-sensor-mq-7.html)                                                                                                                                                                                                                       |
| [BH1750](https://www.adafruit.com/product/4681)                                                             | Ambient light sensor breakout (I2C)                  | $4.50  | [Adafruit](https://www.adafruit.com/product/4681) (in stock), [Mouser](https://www.mouser.com/new/adafruit/adafruit-bh1750-ambient-light-sensor/)                                                                                                                                                     |
| [Piezo Buzzer](https://www.adafruit.com/product/1536)                                                       | Active buzzer, 5V, breadboard-friendly               | $0.95  | [Adafruit](https://www.adafruit.com/product/1536) (in stock)                                                                                                                                                                                                                                          |
| [Logic Level Shifter](https://www.sparkfun.com/sparkfun-logic-level-converter-bi-directional.html)          | 4-ch bi-directional 3.3V/5V (for MQ-7)               | ~$3.95 | [SparkFun BOB-12009](https://www.sparkfun.com/sparkfun-logic-level-converter-bi-directional.html), [Amazon](https://www.amazon.com/SparkFun-BOB-12009-Logic-Converter-Bi-Directional/dp/B00M7U5DV2)                                                                                                   |
| [2N2222A Transistor](https://www.digikey.com/en/products/detail/dcomponents/2N2222A/13575189)               | NPN TO-92 (buzzer driver)                            | ~$0.17 | [DigiKey](https://www.digikey.com/en/products/detail/dcomponents/2N2222A/13575189) (in stock)                                                                                                                                                                                                         |
| [10kΩ Resistor](https://www.digikey.com/en/products/detail/stackpole-electronics-inc/CF14JT10K0/1741265)    | 1/4W through-hole (voltage divider)                  | ~$0.10 | [DigiKey](https://www.digikey.com/en/products/detail/stackpole-electronics-inc/CF14JT10K0/1741265)                                                                                                                                                                                                    |
| [20kΩ Resistor](https://digikey.com/en/products/detail/yageo/CFR-25JB-52-20K/880)                           | 1/4W through-hole (voltage divider)                  | ~$0.10 | [DigiKey](https://digikey.com/en/products/detail/yageo/CFR-25JB-52-20K/880)                                                                                                                                                                                                                           |
| [Breadboard](https://www.adafruit.com/product/64)                                                           | Half-size, 400 tie points                            | ~$5.00 | [Adafruit](https://www.adafruit.com/product/64)                                                                                                                                                                                                                                                       |
| [Jumper Wires (M-M)](https://www.adafruit.com/product/758)                                                  | 40-pack, 150mm                                       | ~$4.00 | [Adafruit](https://www.adafruit.com/product/758)                                                                                                                                                                                                                                                      |
| [Jumper Wires (M-F)](https://www.adafruit.com/product/826)                                                  | 40-pack, 150mm                                       | ~$4.00 | [Adafruit](https://www.adafruit.com/product/826)                                                                                                                                                                                                                                                      |

_Prices checked Feb 2026. Raspberry Pi 4 (2GB+) also required for the dashboard but not listed above._

## Dashboard Stack (Raspberry Pi)

All services run via Docker Compose:

- **Mosquitto** -- MQTT broker
- **Telegraf** -- MQTT-to-InfluxDB bridge
- **InfluxDB 2.x** -- Time-series storage
- **Grafana** -- Live dashboard with gauges and alerts

Setup instructions and config files are in `project.html`.

## License

See [LICENSE](LICENSE).
