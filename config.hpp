#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <algorithm>
#include <arpa/inet.h>
#include <nlohmann/json.hpp> // nlohmann/json kütüphanesi kullanıldığı varsayılmıştır

using json = nlohmann::json;

// Konfigürasyon Yapısı
struct OuchConfig {
    bool enabled = false;
    std::string desc;
    std::string client_account;
    std::string account_number;
    std::string username;
    std::string pass;
    std::string ip;
    uint16_t port = 0;
    uint8_t client_category = 0;
};

// BISTECH OUCH Enter Order Paketi (1-byte alignment şarttır)
#pragma pack(push, 1)
struct BistechOuchEnterOrder {
    char messageType;           // 'O' (Enter Order)
    char orderToken[14];        // Benzersiz Sipariş Kodu
    uint32_t orderBookID;       // Hisse ID (Big-Endian)
    char side;                  // 'B' (Alış) veya 'S' (Satış)
    uint64_t quantity;          // Lot Miktarı (Big-Endian)
    uint32_t price;             // Fiyat * 100 (Big-Endian)
    uint8_t clientCategory;     // Müşteri Kategorisi
    char clientAccount[10];     // Hesap Numarası
};
#pragma pack(pop)

// Hata Kodları Sözlüğü
inline std::string get_ouch_error_message(int32_t error_code) {
    switch (error_code) {
        case -800014: return "Gecersiz Fiyat Adimi / Price Tick Invalid";
        case -800015: return "Bakiye Yersiz / Insufficient Funds";
        case -800016: return "Sembol Bulunamadi / Symbol Not Found";
        case -800017: return "Pazar Kapali / Market Closed";
        default: return "Bilinmeyen Hata Kodu (" + std::to_string(error_code) + ")";
    }
}

// JSON Okuma Fonksiyonu (Dizi Mantığı Eklendi)
inline bool load_config(const std::string& filename, OuchConfig& config) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    try {
        json j;
        file >> j;

        if (j.contains("ouch_pay") && j["ouch_pay"].is_array() && !j["ouch_pay"].empty()) {
            for (const auto& item : j["ouch_pay"]) {
                if (item.value("enabled", false)) {
                    config.enabled = item.value("enabled", false);
                    config.desc = item.value("desc", "");
                    config.client_account = item.value("client_account", "");
                    config.account_number = item.value("account_number", "");
                    config.username = item.value("username", "");
                    config.pass = item.value("pass", "");
                    config.ip = item.value("ip", "");
                    config.port = item.value("port", 0);
                    config.client_category = item.value("client_category", 0);
                    return true;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[JSON PARSE HATA] " << e.what() << std::endl;
        return false;
    }
    return false;
}

#endif
