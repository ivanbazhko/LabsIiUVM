#include <iostream>
#include <WinSock2.h>
#include <ws2bth.h>
#include <BluetoothAPIs.h>
#include <vector>
#include <fstream>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Bthprops.lib")

int main() {

    system("cls");

    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
        WSACleanup();
        return 0;
    }

    std::cout << "Winsock2 initialized successfully" << std::endl;

    std::vector<BLUETOOTH_DEVICE_INFO> devices;

    HBLUETOOTH_RADIO_FIND m_radio = NULL;
    BLUETOOTH_FIND_RADIO_PARAMS m_bt_find_radio = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS) };
    HBLUETOOTH_RADIO_FIND m_bt = BluetoothFindFirstRadio(&m_bt_find_radio, &m_radio);
    if (m_bt == NULL) {
        std::cerr << "BluetoothFindFirstRadio failed with error: " << GetLastError() << std::endl;
        WSACleanup();
        return 0;
    }

    std::cout << "Bluetooth radio found" << std::endl;

    BLUETOOTH_DEVICE_SEARCH_PARAMS searchParams = { sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS) };
    searchParams.fReturnAuthenticated = TRUE;
    searchParams.fReturnRemembered = TRUE;
    searchParams.fReturnConnected = TRUE;
    searchParams.fReturnUnknown = TRUE;
    searchParams.fIssueInquiry = TRUE;
    searchParams.cTimeoutMultiplier = 4;
    searchParams.hRadio = m_radio;

    BLUETOOTH_DEVICE_INFO deviceInfo = { sizeof(BLUETOOTH_DEVICE_INFO) };
    HBLUETOOTH_DEVICE_FIND hDeviceFind = BluetoothFindFirstDevice(&searchParams, &deviceInfo);
    if (hDeviceFind != NULL) {
        int cindex = 0;
        do {
            std::cout << cindex++;
            std::cout << " Bluetooth device: ";
            std::wcout << deviceInfo.szName;
            std::cout << std::endl;
            devices.push_back(deviceInfo);
        } while (BluetoothFindNextDevice(hDeviceFind, &deviceInfo));
        BluetoothFindDeviceClose(hDeviceFind);
    }

    int selectedDeviceIndex;

    int conntry = 1;

    while (1) {

        std::cout << "Enter the index of the device you want to connect to: ";

        std::cin >> selectedDeviceIndex;

        if (selectedDeviceIndex == -1) {
            conntry = 0;
            break;
        }

        if (selectedDeviceIndex < 0 || selectedDeviceIndex >= devices.size()) {
            std::cerr << "Invalid device index" << std::endl;
        }
        else break;

    };

    if (conntry) {

        int success = 1;

        SOCKADDR_BTH addr = { 0 };
        addr.addressFamily = AF_BTH;
        addr.btAddr = devices[selectedDeviceIndex].Address.ullLong;
        addr.serviceClassId = RFCOMM_PROTOCOL_UUID;
        addr.port = 4;

        SOCKET s = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
        if (s == INVALID_SOCKET) {
            std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
            BluetoothFindRadioClose(m_bt);
            success = 0;
        }

        if (success) {
            int connectResult = connect(s, reinterpret_cast<SOCKADDR*>(&addr), sizeof(SOCKADDR_BTH));

            if (connectResult == SOCKET_ERROR) {
                std::cerr << "Failed to connect to the device: " << WSAGetLastError() << std::endl;
                closesocket(s);
                success = 0;
            }
        };

        if (success) {
            std::cout << "Connected to the Bluetooth device" << std::endl;

            std::string filePath = "D:/IpU/541.avi";
            std::ifstream file(filePath, std::ios::binary);
            if (!file.is_open()) {
                std::cerr << "Failed to open file for reading" << std::endl;
                closesocket(s);
                success = 0;
            };
            if (success) {
                char buffer[1024];
                int y = 1;
                while (!file.eof()) {
                    file.read(buffer, sizeof(buffer));
                    int bytesRead = file.gcount();
                    int u = send(s, buffer, bytesRead, 0);
                    if(bytesRead == -1) std::cerr << "Failed to send" << std::endl;
                    std::cout << y++ << " ";
                }

                file.close();

                closesocket(s);
            };
        };
    };

    BluetoothFindRadioClose(m_bt);

    WSACleanup();

    return 0;
}