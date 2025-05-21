[English](#english) | [日本語](#日本語) | [中文](#中文)


# English
# PuffiMail Smart Mailbox Sender
![](https://github.com/puffilab/PuffiMail/blob/main/img/backview3d.png)

A smart mailbox sender device based on ESP32-C3 and VL53L0X Time-of-Flight distance sensor.  
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
![](https://github.com/puffilab/PuffiMail/blob/main/img/frontview2d.png)
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
![](https://github.com/puffilab/PuffiMail/blob/main/img/backview3d.png)
ESP32-C3とVL53L0X ToF距離センサーをベースにした、スマート郵便受け検知デバイスです。  
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
![](https://github.com/puffilab/PuffiMail/blob/main/img/frontview2d.png)
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

# 中文
# PuffiMail 智能郵箱發射器
![](https://github.com/puffilab/PuffiMail/blob/main/img/backview3d.png)

這是一款基於 ESP32-C3 與 VL53L0X ToF（飛行時間）距離感測器的智慧型郵箱發射裝置。  
擁有超低功耗設計、深度休眠、多種喚醒方式（磁吸開關與按鈕）、網頁式 Wi-Fi/MQTT 設定與 OTA 韌體升級、MQTT 整合，並能無縫對接 Home Assistant 及多種智慧家庭平台。

## 特色功能

- ✉️ **郵件偵測：** 自動測量郵箱內部距離，準確判斷是否有新郵件投遞。
- 🛌 **超低功耗：** 深度休眠設計，一顆電池可用數個月以上。
- 🔋 **電池監控：** 低電量自動警告與 MQTT 狀態上報。
- 🧲 **磁吸開關：** 郵箱開啟/關閉時自動喚醒裝置。
- 🛠️ **網頁 AP 模式設定：** 支援瀏覽器設定 Wi-Fi、MQTT、OTA 韌體更新與基準距離。
- 🌏 **多語網頁介面：** 支援英文、中文、日文介面切換。
- 📦 **MQTT 數據上報：** 完美相容 Home Assistant、Node-RED 等平台。
- 🔧 **撥片開關邏輯反轉：** 一鍵切換安裝方向，真正即插即用，適應各種安裝場景。

## AP 模式網頁設定

設備於首次啟動、未設定狀態、按鈕連按三下或長按（恢復出廠）時，會自動進入 AP 設定模式。

1. 連接 Wi-Fi：**Smartmailbox_Sender / 12345678**
2. 瀏覽器開啟網址：`192.168.4.1`
3. 填寫 Wi-Fi、MQTT 設定，調整基準距離，或進行 OTA 韌體升級

網頁介面支援繁體中文、英文、日文。

---
![](https://github.com/puffilab/PuffiMail/blob/main/img/frontview2d.png)
## MQTT 主題

- `mailbox-sensor/distance` — 當前距離（JSON 格式），例：`{"current": 355, "baseline": 362}`
- `mailbox-sensor/mail_status` — `"new_mail"`、`"empty"` 或 `"unknown"`
- `mailbox-sensor/battery` — `"normal"` 或 `"low_battery"`
- `mailbox-sensor/availability` — `"online"`（保留訊息，離線時為 Last Will）

**Home Assistant 可自動將這些主題作為 MQTT 感測器整合！**

---

## OTA 韌體升級

- 於網頁 UI「Firmware Update」區塊上傳 `.bin` 韌體，即可進行 OTA 升級（無需 USB 連接）。

---

## 常見問題

**Q：如何恢復出廠設定？**  
A：長按按鈕 10 秒即可清除所有設定並重啟。

**Q：如何進入設定頁面？**  
A：連續按下按鈕三次，即可進入 AP 模式。

**Q：電池沒電會通知嗎？**  
A：裝置會傳送 `low_battery` MQTT 訊息，Home Assistant 可自動提醒。

**Q：支援哪些智慧家庭平台？**  
A：支援所有支援 MQTT 的平台，如 Home Assistant、openHAB、Node-RED、Domoticz 等。

---

