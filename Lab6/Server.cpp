#include <iostream>
#include <WinSock2.h>
#include <ws2bth.h>
#include <BluetoothAPIs.h>
#include <fstream>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Bthprops.lib")

int main() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
        return 1;
    }

    SOCKET s = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
    if (s == INVALID_SOCKET) {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    SOCKADDR_BTH sockAddr = { 0 };
    sockAddr.addressFamily = AF_BTH;
    sockAddr.serviceClassId = RFCOMM_PROTOCOL_UUID;
    sockAddr.port = 3;

    result = bind(s, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(SOCKADDR_BTH));
    if (result == SOCKET_ERROR) {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }

    result = listen(s, 1);
    if (result == SOCKET_ERROR) {
        std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }

    std::cout << "Waiting for incoming Bluetooth connection..." << std::endl;

    SOCKADDR_BTH clientAddr = { 0 };
    int clientAddrSize = sizeof(SOCKADDR_BTH);
    SOCKET clientSocket = accept(s, reinterpret_cast<SOCKADDR*>(&clientAddr), &clientAddrSize);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }

    std::cout << "Bluetooth connection established" << std::endl;

    std::ofstream outFile("D:/IpU/received.avi", std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file for writing" << std::endl;
        closesocket(clientSocket);
        closesocket(s);
        WSACleanup();
        return 1;
    }

    char buffer[1024];
    int totalBytesReceived = 0;
    int bytesReceived;
    do {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            outFile.write(buffer, bytesReceived);
            totalBytesReceived += bytesReceived;
        }
    } while (bytesReceived > 0);

    std::cout << "Received " << totalBytesReceived << " bytes of data" << std::endl;

    outFile.close();

    closesocket(clientSocket);
    closesocket(s);
    WSACleanup();

    return 0;
}
