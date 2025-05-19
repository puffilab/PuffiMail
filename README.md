[English](#english) | [日本語](#日本語)


# English
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
# 日本語
# PuffiMail スマート郵便受けセンダー

Seeed Studio XIAO ESP32-C3とVL53L0X ToF距離センサーをベースにした、スマート郵便受け検知デバイスです。  
超低消費電力設計、ディープスリープ、マグネットスイッチ・ボタンによる多様なウェイクアップ方式、WebベースのWi-Fi/MQTT設定とOTA、MQTT連携、Home Assistantなどのスマートホームプラットフォームとシームレスに連携できます。

## 特徴

- ✉️ **郵便物検知:** 距離を自動測定し、郵便受けに新しい郵便物があるかどうかを検出します。
- 🛌 **超低消費電力:** ディープスリープ設計により、数ヶ月間バッテリーで動作可能。
- 🔋 **バッテリー監視:** バッテリー低下時に警告・通知を行います。
- 🧲 **マグネットスイッチ:** 郵便受けの開閉で自動的に起動します。
- 🛠️ **WebベースAPモード設定:** ブラウザでWi-Fi、MQTT、ファームウェアOTA、基準距離などを簡単設定。
- 🌏 **多言語Web UI:** 英語・中国語・日本語に対応。
- 📦 **MQTTデータ送信:** Home AssistantやNode-REDなどに対応。
- 🔧 **DIPスイッチによるロジック反転:** 設置環境に応じてロジックを切り替え可能。

---

## APモードWeb設定

初回起動時、未設定時、ボタン三回押し、または長押し（工場出荷時リセット）でAP設定モードに入ります。

1. Wi-Fiに接続：**Smartmailbox_Sender / 12345678**
2. ブラウザで `192.168.4.1` にアクセス
3. Wi-Fi、MQTT情報を入力し、基準距離の設定や必要に応じてOTAファームウェアアップデートを実施

Web UIは英語・中国語・日本語に対応しています。

---

## MQTTトピック

- `mailbox-sensor/distance`  — 現在の距離(JSON形式)、例: `{"current": 355, "baseline": 362}`
- `mailbox-sensor/mail_status` — `"new_mail"`、`"empty"`、または `"unknown"`
- `mailbox-sensor/battery` — `"normal"` または `"low_battery"`
- `mailbox-sensor/availability` — `"online"`（リテイン。切断時はラストウィル）

**Home Assistantではこれらのトピックを自動的にMQTTセンサーとして連携できます！**

---

## OTAファームウェアアップデート

- Web UIの「Firmware Update」セクションから `.bin` ファイルをアップロードすることで、USB不要でOTAアップデートが可能です。

---

## FAQ（よくあるご質問）

**Q:** 工場出荷時の設定にリセットする方法は？  
**A:** ボタンを10秒間長押しすると、すべての設定が消去され再起動します。

**Q:** 設定ページ（APモード）に入るには？  
**A:** ボタンを3回連続で押すと、APモードに入ります。

**Q:** バッテリー残量が少なくなると通知されますか？  
**A:** デバイスが `low_battery` のMQTTメッセージを送信し、Home Assistantで通知できます。

**Q:** 対応するスマートホームプラットフォームは？  
**A:** Home Assistant、openHAB、Node-RED、Domoticzなど、MQTTに対応した全てのプラットフォームで利用可能です。

---

