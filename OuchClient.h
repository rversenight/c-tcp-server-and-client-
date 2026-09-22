#ifndef OUCH_CLIENT_H
#define OUCH_CLIENT_H

#include <iostream>
#include <cstring>
#include <algorithm>
#include <cstdint>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

#include "Config.h"

// --- BISTECH LOGON PACKET ---
#pragma pack(push, 1)
struct BistechOuchLogon {
    char messageType;       // 'L'
    char username[6];
    char password[10];
    char activeSession[10];
};

// --- BISTECH 114 BYTE ENTER ORDER PACKET ---
struct BistechOuchEnterOrder {
    char     messageType;       // 'O'
    char     orderToken[14];
    uint32_t orderBookID;
    char     side;
    uint64_t quantity;
    uint32_t price;
    uint8_t  timeInForce;
    uint8_t  openClose;
    char     clientAccount[16];
    char     customerInfo[15];
    char     exchangeInfo[32];
    uint64_t displayQuantity;
    uint8_t  clientCategory;
    uint8_t  offHours;
    uint8_t  smpLevel;
    uint8_t  smpMethod;
    char     smpID[3];
    char     reserved[2];
};
#pragma pack(pop)

class OuchClient {
private:
    int sock = -1;
    OuchConfig config;

public:
    OuchClient(const OuchConfig& cfg) : config(cfg) {}
    ~OuchClient() { disconnect(); }

    bool connect_server() {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) return false;

        int flags = fcntl(sock, F_GETFL, 0);
        fcntl(sock, F_SETFL, flags | O_NONBLOCK);

        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(config.port);

        if (inet_pton(AF_INET, config.ip.c_str(), &server_addr.sin_addr) <= 0) {
            close(sock);
            sock = -1;
            return false;
        }

        std::cout << "[AG] " << config.ip << ":" << config.port << " adresine baglaniliyor..." << std::endl;
        
        int res = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
        if (res < 0 && errno == EINPROGRESS) {
            fd_set myset;
            struct timeval tv{3, 0};
            FD_ZERO(&myset);
            FD_SET(sock, &myset);
            
            if (select(sock + 1, NULL, &myset, NULL, &tv) <= 0) {
                std::cerr << "[TIMEOUT HATASI] Sunucuya baglanilamadi!" << std::endl;
                close(sock);
                sock = -1;
                return false;
            }
        }

        std::cout << "[BAŞARILI] TCP Baglantisi Kuruldu!" << std::endl;
        return true;
    }

    bool send_logon() {
        if (sock < 0) return false;

        BistechOuchLogon logon{};
        logon.messageType = 'L';
        std::memcpy(logon.username, config.username.c_str(), std::min((size_t)6, config.username.length()));
        std::memcpy(logon.password, config.pass.c_str(), std::min((size_t)10, config.pass.length()));

        ssize_t sent = send(sock, &logon, sizeof(logon), 0);
        if (sent > 0) {
            std::cout << "[LOGON] " << sent << " byte Logon paketi gonderildi (" << config.username << ")" << std::endl;
            return true;
        }
        return false;
    }

    bool send_order(const std::string& hisse, double fiyat, uint64_t lot) {
        if (sock < 0) return false;

        BistechOuchEnterOrder emir{};
        emir.messageType = 'O';
        std::string token = "ORD" + hisse + "001";
        std::memcpy(emir.orderToken, token.c_str(), std::min((size_t)14, token.length()));
        emir.orderBookID = (hisse == "THYAO") ? 1045 : 2088;
        emir.side = 'B';
        emir.quantity = lot;
        emir.price = static_cast<uint32_t>(fiyat * 100);
        emir.clientCategory = static_cast<uint8_t>(config.client_category);
        std::memcpy(emir.clientAccount, config.account_number.c_str(), std::min((size_t)16, config.account_number.length()));

        ssize_t sent = send(sock, &emir, sizeof(emir), 0);
        if (sent > 0) {
            std::cout << "[EMIR] " << sent << " byte Emir paketi gonderildi (" << hisse << " - " << lot << " Lot)" << std::endl;
            return true;
        }
        return false;
    }

    void disconnect() {
        if (sock >= 0) {
            close(sock);
            sock = -1;
            std::cout << "[AG] Baglanti kapatildi." << std::endl;
        }
    }
};

#endif
