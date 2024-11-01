#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")


std::string solveEquation(const std::string& equation) {
    double a, b, c;
    char x;

    
    std::istringstream iss(equation);
    if (!(iss >> a >> x >> x >> b >> x >> c)) {
        return "Error: The equation format is incorrect.";
    }

    
    if (a == 0) {
        return (b == c) ? "Solution: infinitely many solutions" : "Solution: no solutions";
    }

    double result = (c - b) / a;
    return "Solution: x = " + std::to_string(result);
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Winsock initialization error.\n";
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "The socket could not be created.\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Socket binding error.\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    listen(serverSocket, SOMAXCONN);
    std::cout << "Server is running... \n";

    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Client connection error.\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, 1024, 0);
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "An error occurred while receiving the data.\n";
    }
    else {
        buffer[bytesReceived] = '\0';
        std::string equation(buffer);

        // Решаем уравнение
        std::string result = solveEquation(equation);

        // Отправляем результат клиенту
        send(clientSocket, result.c_str(), result.size() + 1, 0);
    }

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}