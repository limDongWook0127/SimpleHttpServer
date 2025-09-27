////#include <WinSock2.h>
////#include <iostream>
////#include <cstring>
////#include <string>
////#include <sstream>
////#include <map>
////#pragma comment(lib, "ws2_32.lib")
////using namespace std;
////
////int main() {
////    WSADATA wsa;
////    WSAStartup(MAKEWORD(2, 2), &wsa);
////
////    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
////
////    sockaddr_in serverAddr{};
////    serverAddr.sin_family = AF_INET;
////    serverAddr.sin_port = htons(8080);
////    serverAddr.sin_addr.s_addr = INADDR_ANY;
////
////    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
////    listen(serverSocket, 5);
////
////    cout << "HTTP 서버 시작: http://localhost:8080/ 접속해보세요" << endl;
////
////    while (true) 
////    {
////        sockaddr_in clientAddr{};
////        int clientSize = sizeof(clientAddr);
////        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
////
////        char buffer[4096] = { 0 };
////        int recvbyteNum = recv(clientSocket, buffer, sizeof(buffer)-1, 0); 
////        if (recvbyteNum > 0)
////        {
////            buffer[recvbyteNum] = '\0';
////            string stringbuffer(buffer);
////            istringstream reqStream(stringbuffer); //stringbuffer라는 문자열을 reqstream이라는 스트림 안에 넣은 것
////            string requestLine;
////            getline(reqStream, requestLine); //stream에서 한줄을 얻어 requestLine에 넣는다.
////            string method, path, version;
////            istringstream dvStream(requestLine);
////            dvStream >> method >> path >> version;
////
////            //헤더 추출
////            map<string, string> headers;
////            string headerLine;
////
////            while (getline(reqStream, headerLine) && headerLine != "\r") //headerline \r이 아니면 while문 계속
////            {
////                if (!headerLine.empty() && headerLine.back() == '\r')
////                {
////                    headerLine.pop_back();
////                }
////                int pos = headerLine.find(':'); //인덱스 반환 (0부터시작)
////                if (pos != string::npos)
////                {
////                    string mapkey = headerLine.substr(0, pos);
////                    string mapValue = headerLine.substr(pos + 1);
////
////                    auto spacespot = remove_if(mapkey.begin(), mapkey.end(), ::isspace);
////                        mapkey.erase(spacespot, mapkey.end());
////                        if (!mapValue.empty() && mapValue[0] == ' ')
////                        {
////                            mapValue.erase(0, 1);
////                        }
////                        headers[mapkey] = mapValue;
////                }
////            }
////
////            for (auto& map : headers)
////            {
////                cout << map.first << " = " << map.second << endl;
////            }
////
////        }
////      
////    }
////
////    closesocket(serverSocket);
////    WSACleanup();
////}
/////*
////"test 123 aaa 456" 이라는 문자열을 iss라는 문자열 스트림 안에 넣은 거예요.
////이제 iss는 마치 cin처럼 >> 연산자를 써서 데이터를 “꺼낼 수 있는 통로”가 됨.
////iss는 문자열을 담은 스트림이고 >>는 그걸 공백 단위로 잘라서 변수에 넣는다.
////
////
////int main() {
////    istringstream iss("test 123 aaa 456");
////    string s1, s2;
////    int i1, i2;
////    iss >> s1 >> i1 >> s2 >> i2; // 공백을 기준으로 문자열을 parsing하고, 변수 형식에 맞게 변환
////
////    cout << s1 << endl;
////    cout << i1 << endl;
////    cout << s2 << endl;
////    cout << i2 << endl;
////}
////
////*/
//
//
//#include <WinSock2.h>
//#include <iostream>
//#include <cstring>
//#include <string>
//#include <sstream>
//#include <map>
//#pragma comment(lib, "ws2_32.lib")
//using namespace std;
//
//int main() {
//    WSADATA wsa;
//    WSAStartup(MAKEWORD(2, 2), &wsa);
//
//    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
//
//    sockaddr_in serverAddr{};
//    serverAddr.sin_family = AF_INET;
//    serverAddr.sin_port = htons(8080);
//    serverAddr.sin_addr.s_addr = INADDR_ANY;
//
//    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
//    listen(serverSocket, 5);
//
//    cout << "HTTP 서버 시작: http://localhost:8080/ 접속해보세요" << endl;
//
//    while (true)
//    {
//        sockaddr_in clientAddr{};
//        int clientSize = sizeof(clientAddr);
//        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
//
//        char buffer[4096] = { 0 };
//        int recvbyteNum = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
//        if (recvbyteNum > 0)
//        {
//            buffer[recvbyteNum] = '\0';
//            string stringbuffer(buffer);
//            istringstream reqStream(stringbuffer);
//            string lineStr;
//            getline(reqStream, lineStr);
//            string method, path, version;
//            istringstream dvStream(lineStr);
//            dvStream >> method >> path >> version;
//
//            map<string, string> headers;
//            string headerLine;
//            string key, value;
//            while (getline(reqStream, headerLine) && headerLine != "\r")
//            {
//                if (!headerLine.empty() && headerLine.back() == '\r')
//                {
//                    int colonplace = headerLine.find(':');
//                    key = headerLine.substr(0, colonplace);
//                    key.erase(remove_if(key.begin(), key.end(), ::isspace), key.end());
//                    value = headerLine.substr(colonplace + 1);
//                    if (!value.empty() && value[0] == ' ')
//                        value.erase(0, 1);
//
//                    headers[key] = value;
//                }
//            }
//            
//            string body;
//            if (headers.find("Content-Length") != headers.end())
//            {
//                int contentLength = stoi(headers["Content-Length"]);
//                body.resize(contentLength);
//                reqStream.read(&body[0], contentLength); //reqStream에서 contentLength바이트만큼 데이터를 읽어 body에 저장
//            }
//
//            string responseBody;
//
//            if (path == "/")
//                responseBody = "Main";
//            else if (path == "/about")
//                responseBody = "About";
//            else
//                responseBody = "404 error";
//
//            string response =
//                "HTTP/1.1 200 OK\r\n"
//                "Content-Type: text/plain; charset=UTF-8\r\n"
//                "Content-Length: " + to_string(responseBody.size()) + "\r\n"
//                "\r\n" +
//                responseBody;
//            
//            send(clientSocket, response.c_str(), (int)response.size(),0);
//        }
//    }
//
//    closesocket(serverSocket);
//    WSACleanup();
//}

#include <WinSock2.h>
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <map>
#include <fstream>   // 파일 입출력용
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

    while (true)
    {
        sockaddr_in clientAddr{};
        int clientSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);

        char buffer[4096] = { 0 };
        int recvbyteNum = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (recvbyteNum > 0)
        {
            buffer[recvbyteNum] = '\0';
            string stringbuffer(buffer);
            istringstream reqStream(stringbuffer);
            string lineStr;
            getline(reqStream, lineStr);
            string method, path, version;
            istringstream dvStream(lineStr);
            dvStream >> method >> path >> version;

            map<string, string> headers;
            string headerLine;
            string key, value;
            while (getline(reqStream, headerLine) && headerLine != "\r")
            {
                if (!headerLine.empty() && headerLine.back() == '\r')
                {
                    int colonplace = headerLine.find(':');
                    key = headerLine.substr(0, colonplace);
                    key.erase(remove_if(key.begin(), key.end(), ::isspace), key.end());
                    value = headerLine.substr(colonplace + 1);
                    if (!value.empty() && value[0] == ' ')
                        value.erase(0, 1);

                    headers[key] = value;
                }
            }

            string body;
            if (headers.find("Content-Length") != headers.end())
            {
                int contentLength = stoi(headers["Content-Length"]);
                body.resize(contentLength);
                reqStream.read(&body[0], contentLength);
            }

            // -----------------------------
            // 라우팅 + 정적 파일 처리
            // -----------------------------
            string responseBody;
            string statusLine = "HTTP/1.1 200 OK\r\n";
            string contentType = "text/plain; charset=UTF-8";

            if (path == "/") 
            {
                // index.html 읽기
                ifstream file("index.html");
                if (file) 
                {
                    responseBody.assign((istreambuf_iterator<char>(file)),
                        istreambuf_iterator<char>());
                    contentType = "text/html; charset=UTF-8";
                }
                else 
                {
                    statusLine = "HTTP/1.1 404 Not Found\r\n";
                    responseBody = "index.html 파일을 찾을 수 없습니다";
                }
            }
            else if (path == "/about") 
            {
                responseBody = "<h1>About Page</h1>";
                contentType = "text/html; charset=UTF-8";
            }
            else 
            {
                statusLine = "HTTP/1.1 404 Not Found\r\n";
                responseBody = "404 error";
            }

            string response =
                statusLine +
                "Content-Type: " + contentType + "\r\n" +
                "Content-Length: " + to_string(responseBody.size()) + "\r\n"
                "\r\n" +
                responseBody;

            send(clientSocket, response.c_str(), (int)response.size(), 0);
        }
    }

    closesocket(serverSocket);
    WSACleanup();
}