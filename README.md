# 💡 Smart Lamp ESP32  
**UAS Sistem Mikroprosesor**  
**Farhandhika Nurrohman | 23552011203**

---

## 📌 Deskripsi
Proyek ini merupakan implementasi **Smart Lamp berbasis ESP32** yang mendukung kontrol **lokal** melalui tombol fisik dan **remote** melalui **WiFi dan protokol MQTT**. Sistem dirancang untuk menerapkan konsep inti **Sistem Mikroprosesor** secara efisien, responsif, dan stabil.

Arsitektur sistem memanfaatkan **ESP32 dual-core** dengan **FreeRTOS** untuk memisahkan proses hardware dan komunikasi jaringan. Input tombol ditangani menggunakan **external interrupt**, sementara kecerahan LED dikontrol menggunakan **PWM** dengan algoritma *soft dimming*. Nilai kecerahan disimpan menggunakan **Non-Volatile Storage (NVS)** agar tetap tersimpan setelah perangkat dimatikan.

---

## 🛠️ Teknologi
- ESP32 Dual Core  
- FreeRTOS  
- External Interrupt  
- PWM (Pulse Width Modulation)  
- WiFi  
- MQTT  
- JSON  
- Non-Volatile Storage (Preferences)

---

## 🎥 Video Demo
👉 (https://youtu.be/bhtpUIJQhmc)
