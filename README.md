# UAS_Sistem-Mikroprosessor_Smart-Lamp-ESP32_23552011203_Farhandhika-Nurrohman

Proyek ini merupakan implementasi Smart Lamp berbasis ESP32 yang dirancang untuk mendemonstrasikan penerapan konsep Sistem Mikroprosesor secara komprehensif. Sistem mendukung pengendalian lampu melalui tombol fisik (local control) dan perintah jarak jauh (remote control) menggunakan koneksi WiFi dan protokol MQTT.

Arsitektur sistem memanfaatkan ESP32 dual-core dengan FreeRTOS untuk menjalankan proses secara paralel. Tugas pengolahan perangkat keras dan komunikasi jaringan dipisahkan ke dalam task yang berjalan pada core berbeda, sehingga sistem tetap responsif dan stabil.

Input tombol ditangani menggunakan external interrupt untuk efisiensi CPU, sementara pengaturan intensitas cahaya LED dilakukan melalui PWM (Pulse Width Modulation) dengan algoritma soft dimming agar transisi cahaya berlangsung halus. Sistem juga menerapkan Non-Volatile Storage (NVS) menggunakan library Preferences untuk menyimpan nilai kecerahan terakhir, sehingga data tetap tersedia meskipun perangkat dimatikan.

Proyek ini menampilkan integrasi antara manajemen proses, input/output, memori, dan komunikasi jaringan, sehingga cocok digunakan sebagai referensi atau dokumentasi pembelajaran pada mata kuliah Sistem Mikroprosesor.
