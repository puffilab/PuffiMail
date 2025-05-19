# PuffiMail Smart Mailbox Sender


A smart mailbox sender device based on Seeed Studio XIAO ESP32-C3 and VL53L0X Time-of-Flight distance sensor.  
Features ultra-low power design, deep sleep, multiple wakeup methods (magnetic switch & button), web-based Wi-Fi/MQTT configuration & OTA, MQTT integration, and seamless compatibility with Home Assistant and other smart home platforms.

## Features

- ✉️ **Mail Detection:** Automatically measures the distance to detect new mail in your mailbox.
- 🛌 **Ultra Low Power:** Deep sleep design enables months of battery life.
- 🔋 **Battery Monitoring:** Low battery warning and reporting.
- 🧲 **Magnetic Switch:** Wake up automatically when mailbox is opened/closed.
- 🛠️ **Web-based AP Mode Configuration:** Configure Wi-Fi, MQTT, firmware OTA, and baseline distance in browser.
- 🌏 **Multi-language Web UI:** English, Chinese, and Japanese supported.
- 📦 **MQTT Data Reporting:** Compatible with Home Assistant, Node-RED, and more.
- 🔧 **Logic Reversal via DIP Switch:** Supports different installation scenarios.



## AP Mode Web Configuration

The device enters AP configuration mode when first powered on, if unconfigured, by triple-pressing the button, or by long-press for factory reset.

1. Connect to Wi-Fi: **Smartmailbox_Sender / 12345678**
2. Open browser and visit: `192.168.4.1`
3. Fill in Wi-Fi, MQTT settings, set baseline distance, and perform OTA firmware update if needed

Web UI supports English, Chinese, and Japanese.

---

## MQTT Topics

- `mailbox-sensor/distance`  — Current distance as JSON, e.g. `{"current": 355, "baseline": 362}`
- `mailbox-sensor/mail_status` — `"new_mail"`, `"empty"`, or `"unknown"`
- `mailbox-sensor/battery` — `"normal"` or `"low_battery"`
- `mailbox-sensor/availability` — `"online"` (retained, offline is Last Will)

**Home Assistant can integrate these topics as MQTT sensors automatically!**

---

## OTA Firmware Upgrade

- Use the web UI’s "Firmware Update" section to upload a `.bin` file for over-the-air updates (no USB required).

---

## FAQ

**Q:** How to restore factory settings?  
**A:** Long-press the button for 10 seconds to clear all settings and reboot.

**Q:** How to enter the configuration page?  
**A:** Triple-press the button to enter AP mode.

**Q:** Will I be notified when battery is low?  
**A:** The device sends a `low_battery` MQTT message; Home Assistant can display notifications.

**Q:** Which smart home platforms are supported?  
**A:** Any platform supporting MQTT, such as Home Assistant, openHAB, Node-RED, Domoticz, etc.

---


