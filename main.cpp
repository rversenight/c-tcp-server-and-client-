#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <cstdint>
#include <arpa/inet.h>
#include "config.hpp"

// 64-bit Big-Endian Dönüştürücü (Cross-platform)
uint64_t hton64(uint64_t val) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return __builtin_bswap64(val);
#else
    return val;
#endif
}

// Sembol - OrderBookID Haritası
uint32_t get_order_book_id(const std::string& symbol) {
    if (symbol == "THYAO") return 1045;
    if (symbol == "GARAN") return 2088;
    if (symbol == "AKBNK") return 3012;
    if (symbol == "EREGL") return 4001;
    return 0; // Bulunamadı
}

int main() {
    OuchConfig config;
    if (!load_config("setting.json", config)) {
        std::cerr << "[HATA] setting.json okunamadi veya aktif konfigürasyon bulunamadi!" << std::endl;
        return -1;
    }

    std::cout << "=================================================" << std::endl;
    std::cout << " BISTECH OUCH Order Entry Client Simulation" << std::endl;
    std::cout << " Config IP/Port : " << config.ip << ":" << config.port << std::endl;
    std::cout << " User/Account   : " << config.username << " / " << config.account_number << std::endl;
    std::cout << "=================================================\n" << std::endl;

    std::string hisse;
    double fiyat;
    uint64_t lot;

    // Girdi Doğrulamaları
    std::cout << "Hisse Adi Giriniz : ";
    if (!(std::cin >> hisse)) return -1;

    uint32_t book_id = get_order_book_id(hisse);
    if (book_id == 0) {
        std::cerr << "[HATA] Tanimsiz hisse kodu girildi!" << std::endl;
        return -1;
    }

    std::cout << "Fiyat Giriniz     : ";
    if (!(std::cin >> fiyat) || fiyat <= 0) {
        std::cerr << "[HATA] Gecersiz fiyat girdisi!" << std::endl;
        return -1;
    }

    std::cout << "Lot Giriniz       : ";
    if (!(std::cin >> lot) || lot == 0) {
        std::cerr << "[HATA] Gecersiz lot miktari!" << std::endl;
        return -1;
    }

    BistechOuchEnterOrder emir;
    std::memset(&emir, 0, sizeof(emir));

    emir.messageType = 'O';

    // Safe Token Copy (Max 14 byte)
    std::string token = "ORD" + hisse + "001";
    std::memcpy(emir.orderToken, token.c_str(), std::min(sizeof(emir.orderToken), token.length()));

    // Network Byte Order (Big-Endian) Dönüşümleri
    emir.orderBookID = htonl(book_id);
    emir.side = 'B';
    emir.quantity = hton64(lot);
    emir.price = htonl(static_cast<uint32_t>(fiyat * 100));
    emir.clientCategory = config.client_category;

    // Safe Account Copy (Max 10 byte)
    std::memcpy(emir.clientAccount, config.account_number.c_str(), std::min(sizeof(emir.clientAccount), config.account_number.length()));

    std::cout << "\n[PAKET HAZIRLANDI (Big-Endian Format)]" << std::endl;
    std::cout << " -> Byte Boyutu  : " << sizeof(emir) << " Byte" << std::endl;
    std::cout << " -> Order Token  : " << token << std::endl;
    std::cout << " -> OrderBook ID : " << book_id << " (Network: " << emir.orderBookID << ")" << std::endl;
    std::cout << " -> Islem        : " << hisse << " | Lot: " << lot << " | Fiyat: " << fiyat << std::endl;
    std::cout << " -> Musteri Cat  : " << static_cast<int>(emir.clientCategory) << std::endl;

    std::cout << "\n[HATA METNI DOGRULAMA]" << std::endl;
    std::cout << " -> Kod -800014 : " << get_ouch_error_message(-800014) << std::endl;

    return 0;
}
