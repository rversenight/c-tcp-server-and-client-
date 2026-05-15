#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);

    bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));
    listen(serverSocket, 3);

    std::cout << "Client gelmesini bekliyoruz (Port: 8080)...\n";

    SOCKET clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Baglanti kabul edilemedi!\n";
        return 1;
    }

    std::cout << "Client kral bagli su an!\n";

    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        // recv fonksiyonu kaç byte geldiğini döndürür
        int bytesReceived = recv(clientSocket, buffer, 1024, 0);

        if (bytesReceived <= 0) {
            std::cout << "\nClient baglantiyi kesti veya hata olustu.\n";
            break;
        }

        std::cout << "Client: " << buffer << std::endl;

        // Server cevap yazıyor
        std::cout << "Sen: ";
        std::cin.getline(buffer, 1024);

        send(clientSocket, buffer, (int)strlen(buffer), 0);
    }

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    std::cout << "Server kapatildi. Cikmak icin bir tusa basin...";
    std::cin.get();
    return 0;
}