#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cout << "Winsock baslatilamadi!" << std::endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Socket olusturulamadi!" << std::endl;
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);

    // IP adresini metinden ikili formata çeviriyoruz
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {
        std::cout << "Gecersiz adres!" << std::endl;
        return 1;
    }

    // Server'a bağlanmayı dene
    std::cout << "Server'a baglaniliyor..." << std::endl;
    if (connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cout << "Baglanti hatasi! Server acik mi?" << std::endl;
        return 1;
    }
    std::cout << "Baglanti basarili!" << std::endl;

    char mesaj[1024];
    char cevap[1024];

    while (true) {
        std::cout << "Alper: ";
        std::cin.getline(mesaj, 1024);

        if (strlen(mesaj) == 0) continue; // Boş mesaj gönderme

        send(clientSocket, mesaj, (int)strlen(mesaj), 0);

        int bytes = recv(clientSocket, cevap, 1024, 0);
        if (bytes <= 0) {
            std::cout << "Baglanti koptu veya Server kapandi.\n";
            break;
        }

        cevap[bytes] = '\0';
        std::cout << "Server: " << cevap << std::endl;
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}