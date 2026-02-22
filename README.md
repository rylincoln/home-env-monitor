# Home Environment Monitor

A CC3220SF-based indoor environment monitoring system that tracks temperature, humidity, air quality, CO levels, and ambient light — with a local Raspberry Pi dashboard and CO safety alarms.

## Sensors

| Sensor | Measures | Interface |
|--------|----------|-----------|
| BME280 | Temperature, humidity, barometric pressure | I2C `0x76` |
| CCS811 | eCO2, TVOC (indoor air quality) | I2C `0x5A` |
| MQ-7 | Carbon monoxide (20-2000 ppm) | ADC (via voltage divider) |
| BH1750 | Ambient light (1-65535 lux) | I2C `0x23` |
| Piezo Buzzer | CO alarm output | GPIO (via 2N2222 transistor) |

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

~$60 total. See `project.html` for the full BOM with part descriptions and estimated costs. Key components:

- CC3220SF-LAUNCHXL (~$30)
- BME280, CCS811, BH1750, MQ-7 sensor breakouts
- Piezo buzzer, 2N2222 transistor, resistors (10k/20k voltage divider)
- Logic level shifter (3.3V/5V for MQ-7)
- Raspberry Pi 4 (2GB+) for the dashboard

## Dashboard Stack (Raspberry Pi)

All services run via Docker Compose:

- **Mosquitto** -- MQTT broker
- **Telegraf** -- MQTT-to-InfluxDB bridge
- **InfluxDB 2.x** -- Time-series storage
- **Grafana** -- Live dashboard with gauges and alerts

Setup instructions and config files are in `project.html`.

## License

See [LICENSE](LICENSE).
