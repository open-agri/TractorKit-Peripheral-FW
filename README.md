# TractorKit-Peripheral-FW
Generic peripheral firmware.

## Supported boards
- EngineSense (RPM with pulse counter and MLX90614 IR thermometer)
- Navis (any NMEA-compliant GPS)
- Custom (a selection of available modules)

## Supported sensors and data
- Pulse counter RPM meter
- MLX90614 IR and environment thermometer
- NMEA GPS modules
- (Coming soon) I2C compass modules

## OTA updates
OTA support is coming soon

## BLE GATT services
Here is a complete list of the custom BLE GATT services with their UUIDs.
The "Device information" service is automatically generated based on the IDF and FW build, the device's MAC address and the board name selected in kconfig.

Every service and characteristic UUID for OpenAgri-compatible devices has the following structure: `5AAA-2412-111F-2400-XXXX-GGHHJJJJKKKK`.

The `XXXX` part should be selected based on the owner of the device/service. Official services have `XXXX = 0AA1`.
I will add an entry to a table whenever somebody creates its first service, in order to keep track of the owner codes.

The `GGHHJJJJKKKK` part is split in four sections.

- `GG` is an 8-bit identifier for the device group. `0x13` is for TractorKit.
- `HH` is the device identifier. A characteristic should be related to a device kind.

| `HH` | Usage |
|------|-------|
| `0x00` | Common services (OTA, ...) |
| `0x01` | Specific for Commander interaction with portable devices (smartphone, PC, ...) |
| `0x02` | Engine data and settings |
| `0x03` | Navigation data and settings (temporary, migrate to GATT "Location and Navigation Service" instead |
| `0xFF` | Diagnosis |

- `JJJJ` is the service identifier. Has an offset of `0x5D24`.
- `KKKK` is the characteristic identifier. Services should have `KKKK` set to zero. Characteristic descriptions should have an offset of `0x1000`.

## UUID table
| UUID                                   | Type           | Description                         | Data type      | Access      |
|----------------------------------------|----------------|-------------------------------------|----------------|-------------|
| `5AAA2412-111F-2400-0AA1-13005D240000` | **Service**    | **OTA Service**                     |                |             |
| `5AAA2412-111F-2400-0AA1-13005D240001` | Characteristic | Enable access point for .bin upload | Boolean (byte) | read write  |
| `5AAA2412-111F-2400-0AA1-13005D240002` | Characteristic | Access point SSID                   | String         | read        |
| `5AAA2412-111F-2400-0AA1-13005D240003` | Characteristic | Access point password               | String         | read        |
| `5AAA2412-111F-2400-0AA1-13005D240004` | Characteristic | Update URL                          | String         | read        |
| `5AAA2412-111F-2400-0AA1-13005D240005` | Characteristic | Update progress                     | Integer        | read notify |
| `5AAA2412-111F-2400-0AA1-13025D240000` | **Service**    | **Engine RPM service**              |                |             |
| `5AAA2412-111F-2400-0AA1-13025D240001` | Characteristic | Engine RPM                          | Double         | read notify |
| `5AAA2412-111F-2400-0AA1-13025D240002` | Characteristic | Engine RPM availability             | Boolean (byte) | read notify |
| `5AAA2412-111F-2400-0AA1-13025D240003` | Characteristic | Engine RPM coefficient              | Double         | read write  |
| `5AAA2412-111F-2400-0AA1-13025D250000` | **Service**    | **Engine temperature service**      |                |             |
| `5AAA2412-111F-2400-0AA1-13025D250001` | Characteristic | Engine temperature                  | Double         | read notify |
| `5AAA2412-111F-2400-0AA1-13025D250002` | Characteristic | Intake air temperature              | Double         | read notify |
| `5AAA2412-111F-2400-0AA1-13025D250003` | Characteristic | Engine temperature availability     | Boolean (byte) | read notify |
| `5AAA2412-111F-2400-0AA1-13025D250004` | Characteristic | Intake air temperature availability | Boolean (byte) | read notify |
