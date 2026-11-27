#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <ctime>
#include "Config.h"
#include "OuchClient.h"
#include "Logger.h"
#include "Database.h"

std::time_t get_target_epoch(int target_hour, int target_min, int target_sec) {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm target_tm = *std::localtime(&now_c);
    target_tm.tm_hour = target_hour;
    target_tm.tm_min = target_min;
    target_tm.tm_sec = target_sec;
    return std::mktime(&target_tm);
}

void sync_orders_and_wait(OuchConfig& config, std::vector<OrderItem>& active_orders) {
    std::time_t target_epoch = get_target_epoch(config.target_hour, config.target_min, config.target_sec);
    std::time_t lock_epoch = target_epoch - 60; 

    Logger::log("INFO", "T-1 dakikaya kadar veritabani (orders.db) senkronizasyonu basladi.");

    while (true) {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);

        if (now_c < lock_epoch) {
            auto latest_orders = Database::fetch_active_orders("orders.db");
            if (latest_orders.size() != active_orders.size()) {
                Logger::log("DB", "Guncel emir listesi cekildi. Aktif emir sayisi: " + std::to_string(latest_orders.size()));
            }
            active_orders = latest_orders;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        } else {
            break;
        }
    }

    Logger::log("LOCK", "Son 1 dakikaya girildi! Veritabani okumasi durduruldu, " + 
                std::to_string(active_orders.size()) + " emir hafizada kilitlendi.");

    while (true) {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        auto local_time = *std::localtime(&now_c);

        if (local_time.tm_hour == config.target_hour && 
            local_time.tm_min == config.target_min && 
            local_time.tm_sec == config.target_sec) {
            
            auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()
            ).count() % 1000;
            
            if (config.target_ms - millis <= 50) {
                while (true) {
                    auto high_res_now = std::chrono::system_clock::now();
                    auto current_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        high_res_now.time_since_epoch()
                    ).count() % 1000;
                    
                    if (current_ms >= config.target_ms) {
                        break;
                    }
                    std::this_thread::yield();
                }
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

int main() {
    OuchConfig config;
    if (!ConfigLoader::load("setting.json", config)) {
        Logger::log("HATA", "setting.json okunamadi!");
        return -1;
    }

    Logger::log("INFO", "BISTECH OUCH Client Baslatildi (DB Sync + High-Precision Mode)");

    std::vector<OrderItem> active_orders = Database::fetch_active_orders("orders.db");
    Logger::log("DB", "Ilk yukleme tamamlandi. Hazir emir sayisi: " + std::to_string(active_orders.size()));

    OuchClient client(config);
    
    if (client.connect_server()) {
        client.send_logon();

        sync_orders_and_wait(config, active_orders);

        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (const auto& ord : active_orders) {
            client.send_order(ord.symbol, ord.side, ord.price, ord.quantity, ord.id);
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
        
        Logger::log("PERF", "Toplam " + std::to_string(active_orders.size()) + 
                    " emrin iletim suresi: " + std::to_string(duration_us) + " us");

        client.process_incoming_data();
    }

    return 0;
}
