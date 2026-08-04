# I.MX6ULL-Apps

Qt 5 applications for the ATK-IMX6U development board (NXP i.MX6ULL),
built on the board's custom embedded Linux BSP.

## Hardware

| Component | Specification |
|-----------|---------------|
| **SoC** | NXP i.MX6ULL, Cortex-A7 @ 792MHz (ARMv7, no GPU — software rendering) |
| **RAM** | 512MB DDR3L |
| **Storage** | 8GB eMMC (mmcblk1) |
| **Display** | ATK-MD0700R 7" 1024x600 RGB888 (DRM mxsfb + PWM backlight) |
| **Touch** | GOODIX GT911 capacitive touchscreen (I2C2, addr 0x14) |
| **Ethernet** | SR8201F PHY via RMII (ENET1 + ENET2) |
| **WiFi** | Realtek RTL8188EUS USB dongle (2.4GHz 802.11b/g/n) |
| **Audio** | WM8960 codec (SAI2 + I2C1) |
| **Serial** | UART1 (ttymxc0) console, 115200 baud |

## Software Stack

| Component | Source | Version |
|-----------|--------|---------|
| **Kernel** | NXP linux-imx (lf-6.6.y) | 6.6.52 |
| **Rootfs** | Buildroot | 2024.02 |
| **Bootloader** | NXP uboot-imx (lf_v2023.04) | 2023.04 |
| **Toolchain** | Buildroot-built | GCC 12.3.0, glibc |
| **Qt** | Buildroot qt5 packages | 5.15.11 |

### Kernel & SoC Details

- Kernel: `6.6.52-g5a0a5e71d2bd-dirty` (ARMv7, PREEMPT, custom `imx6ull-atk.dts`).
- Display: DRM mxsfb with `panel-dpi` graph binding;
  `pixelclk-active = <1>` + LCDIF pinctrl `0x1b0b0` (noise/jitter fix).
- Touch: GT911 on I2C2; `/dev/input/event1` → `touchscreen0`.
- WiFi: `CONFIG_RTL8XXXU` (rtl8xxxu driver); regulatory.db embedded in kernel
  (`CONFIG_EXTRA_FIRMWARE`); auto-connect via `S50wifi` (wpa_supplicant + udhcpc).

## Development Environment

- **Host:** WSL2 (Hermes, Ubuntu) — cross-compilation via Buildroot toolchain
  (`output/host/bin/qmake`, `arm-buildroot-linux-gnueabihf-g++`).
- **Board access:** serial console (115200) + SSH (dropbear, 192.168.1.100).
- **Deployment:** scp binary to `/opt/ui/src/apps/`, update launcher cfg,
  restart the systemui launcher.

## Directory Structure

```
I.MX6ULL-Apps/
├── DeepSeek/          ChatGPT-style client (DeepSeek V4 Flash) — see DeepSeek/README.md
├── client/            SystemUICommonApiClient (shared dependency)
├── Repcs/             Qt Remote Objects .rep definitions (shared dependency)
└── README.md
```

## Build

```bash
export PATH=<buildroot>/output/host/bin:$PATH
cd <AppFolder>
qmake <App>.pro
make -j8
```

Qt5 packages required: `qt5base` (+linuxfb, +png, +jpeg), `qt5declarative`,
`qt5quickcontrols2`, `qt5remoteobjects`, `qt5virtualkeyboard`, plus OpenSSL.

## Deploy

```bash
scp <App> root@192.168.1.100:/opt/ui/src/apps/<App>
ssh root@192.168.1.100 'chmod 755 /opt/ui/src/apps/<App>'
```

Register the app in the launcher cfg (`/opt/ui/src/ATK/apk1.cfg` or
`apk3.cfg`), then restart the systemui launcher.

## Notes

Each app folder contains its own `README.md` with app-specific details
(features, configuration, API keys, etc.).
