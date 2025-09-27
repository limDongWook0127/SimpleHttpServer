#include <WinSock2.h>
#include <iostream>
#include <cstring>
#include <string>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);

    cout << "HTTP 서버 시작: http://localhost:8080/ 접속해보세요" << endl;

    while (true) {
        sockaddr_in clientAddr{};
        int clientSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);

        char buffer[4096] = { 0 };
        recv(clientSocket, buffer, sizeof(buffer), 0); //clientSocket 이라는 연결(=accept로 생성된 소켓)에서 데이터를 받아서 buffer에 넣어라
        cout << "=== 요청 ===" << endl << buffer << endl;
        cout << "=== 요청끝 ===" << endl;
        // 간단한 HTTP 응답 만들기
        const char* body = "Hello, HTTP!";
        int bodyLength = (int)strlen(body);
        string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + to_string(bodyLength) + "\r\n"
            "\r\n" +
            body;
        
        send(clientSocket, response.c_str(), (int)response.size(), 0); //c_str은 string을 c스타일 const char*로 변환
        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();
}
