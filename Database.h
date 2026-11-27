#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <vector>
#include <string>
#include <cstdint>
#include "Logger.h"

struct OrderItem {
    int id;
    std::string symbol;
    char side;
    double price;
    uint64_t quantity;
};

class Database {
public:
    static std::vector<OrderItem> fetch_active_orders(const std::string& db_path) {
        std::vector<OrderItem> orders;
        sqlite3* db;
        
        if (sqlite3_open(db_path.c_str(), &db) != SQLITE_OK) {
            Logger::log("DB_HATA", "Veritabani acilamadi: " + std::string(sqlite3_errmsg(db)));
            return orders;
        }

        const char* sql = "SELECT id, symbol, side, price, quantity FROM orders WHERE active = 1;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                OrderItem item;
                item.id = sqlite3_column_int(stmt, 0);
                item.symbol = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                const char* side_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                item.side = side_str ? side_str[0] : 'B';
                item.price = sqlite3_column_double(stmt, 3);
                item.quantity = static_cast<uint64_t>(sqlite3_column_int64(stmt, 4));
                orders.push_back(item);
            }
            sqlite3_finalize(stmt);
        } else {
            Logger::log("DB_HATA", "SQL sorgu hatasi: " + std::string(sqlite3_errmsg(db)));
        }

        sqlite3_close(db);
        return orders;
    }
};

#endif
