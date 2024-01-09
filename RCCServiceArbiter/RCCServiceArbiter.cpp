#include <iostream>
#include <spdlog/spdlog.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int StartRCC() {
    const char* command = "start cmd /c 271.exe -console -verbose -placeid:1818 -port 10836";

    return system(command);
}

bool IsPortOpen() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        spdlog::error("Failed to initialize Winsock");
        return false;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        spdlog::error("Socket creation failed");
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(10836);

    int connectResult = connect(sock, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress));

    closesocket(sock);
    WSACleanup();

    if (connectResult != 0) {
        spdlog::critical("RCCService has crashed! Restarting gameserver...");
        StartRCC();
        return false;
    }

    return true;
}

int main() {
    spdlog::info("Starting RCCService in verbose mode");

    int result = StartRCC();

    if (result != 0) {
        spdlog::critical("Couldn't start RCCService");
        system("pause");
        return 1;
    }

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (IsPortOpen()) {
            spdlog::info("RCCService is running as intended!");
        }
        else {
            spdlog::error("Failed to connect to RCCService");
        }
    }

    return 0;
}
