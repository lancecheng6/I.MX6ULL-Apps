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
├── PiAgent/           Pi Agent (earendil-works/pi coding agent) as a Qt chat app — see PiAgent/DevLog_20260805.md
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

### PiAgent — Pi coding agent on-board

> **Purpose:** deploy the AI as an **autonomous controller of the embedded
> system** — the agent senses the hardware (GPIO, sensors, UART/RS-485,
> system status), reasons & **makes decisions** with an LLM, then **acts**
> through tools to drive the hardware — i.e. **AI-driven automation**.
> `PiAgent/` is the on-board foundation for that: a Qt chat app that drives
> the open-source **pi coding agent**
> (`earendil-works/pi`, npm `@earendil-works/pi-coding-agent`) in headless RPC
> mode (`pi --mode rpc`, JSONL over stdin/stdout via `QProcess`), using the
> **OpenCode Zen** endpoint (`https://opencode.ai/zen/go/v1`,
> model `deepseek-v4-flash`) through a custom provider in
> `~/.pi/agent/models.json`.

Runtime requirements on the board:

- **Node.js 22** (`linux-armv7l` prebuilt, installed to `/opt/node`).
- **pi 0.83.x** installed globally via `npm install -g --ignore-scripts`.
- **Clock sync**: the board has no RTC; `S55ntp` (boot script) syncs time from
  an HTTP `Date:` header — mandatory for TLS (`CERT_NOT_YET_VALID` otherwise).
- **HOME**: the app forces `HOME=/root` when spawning pi (SystemUI inherits
  `HOME=/` which makes pi miss `~/.pi/agent/models.json`).

UI/UX highlights: one bubble per answer with throttled streaming
(100 ms coalescing), `working...`/`ready` status, Send locked while working,
`cacheBuffer` + user-drag-aware auto-scroll (see the DevLog's scroll-debugging
chapter — the hardest part).

**Autonomy & automation roadmap:**
- The agent already has `bash` — it can read `/sys`, run `gpioset`/`devmem`,
  write to serial ports, toggle relays, … so it can already **sense → decide
  → actuate** from the chat interface.
- Next step: register purpose-built hardware tools (`gpio_set`, `uart_send`,
  `sensor_read`, `relay_toggle`) via pi extensions/skills for clean, safe,
  fully autonomous control of the embedded system.
