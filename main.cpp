 GNU nano 7.2                                                                                          main.cpp *                                                                                                 int main() {
    OuchConfig config;
    if (!load_config("setting.json", config)) {
        std::cerr << "[HATA] setting.json okunamadi!" << std::endl;
        return -1;
    }

    std::cout << "=================================================" << std::endl;
    std::cout << " BISTECH OUCH Order Entry Client Simulation" << std::endl;
    std::cout << " Config IP/Port: " << config.ip << ":" << config.port << std::endl;
    std::cout << " User/Account  : " << config.username << " / " << config.account_number << std::endl;
    std::cout << "=================================================\n" << std::endl;

    std::string hisse;
    double fiyat;
    uint64_t lot;

    std::cout << "Hisse Adi Giriniz : ";
    std::cin >> hisse;
    std::cout << "Fiyat Giriniz : ";
    std::cin >> fiyat;
    std::cout << "Lot Giriniz: ";
    std::cin >> lot;

    BistechOuchEnterOrder emir;
    std::memset(&emir, 0, sizeof(emir));

    emir.messageType = 'O';
    std::string token = "ORD" + hisse + "001";
    std::memcpy(emir.orderToken, token.c_str(), std::min((size_t)14, token.length()));

    emir.orderBookID = (hisse == "THYAO") ? 1045 : 2088;
    emir.side = 'B';
    emir.quantity = lot;
    emir.price = static_cast<uint32_t>(fiyat * 100);
    emir.clientCategory = config.client_category;
    std::memcpy(emir.clientAccount, config.account_number.c_str(), config.account_number.length());

    std::cout << "\n[PAKET HAZIRLANDI]" << std::endl;
    std::cout << " -> Byte Boyutu  : " << sizeof(emir) << " Byte" << std::endl;
    std::cout << " -> Order Token  : " << token << std::endl;
    std::cout << " -> OrderBook ID : " << emir.orderBookID << std::endl;
    std::cout << " -> Islem        : " << hisse << " | Lot: " << emir.quantity << " | Fiyat: " << fiyat << std::endl;
    std::cout << " -> Musteri Cat  : " << (int)emir.clientCategory << std::endl;

    std::cout << "\n[HATA METNI DOGRULAMA]" << std::endl;
    std::cout << " -> Kod -800014 : " << get_ouch_error_message(-800014) << std::endl;

    return 0;
}

