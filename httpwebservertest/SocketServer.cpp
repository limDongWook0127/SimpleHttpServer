#include <WinSock2.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")
using namespace std;
int main()
{
	/*윈도우는 네트워크 api가 winsock dll(ws2_32.dll)에 들어있음
	그래서 쓰기전에 dll을 초기화해야 함->WSAStartup
	*/
	
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);//매개변수(버전, WINSOCK 초기화 결과를 채워주는 구조체의 주소)
	//MAKEWORD(a,b) -> 하위바이트 =a 상위 =b  로 묶어서 WORD(16비트) 값 변환
	//0x0202

	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in serverAddress;
	u_short portNum = 8080;
	char buffer[1024] = { 0 };
	//주소 체계
	serverAddress.sin_family = AF_INET; //AF_INET : IPv4
	//포트 번호
	serverAddress.sin_port = htons(portNum); //host to network short -엔디안 변환 함수 h = host(내컴퓨터) to n = network(빅 엔디안 규칙)
	//ip 주소 구조체
	serverAddress.sin_addr.s_addr = INADDR_ANY; //소켓을 특정 IP에 바인딩하지 않고 대신 사용 가능한 모든 IP를 수신하도록 할 때 사용.
	/*
	 특정 IP 주소 바인딩할 때는 inet_addr 사용
	serverAddress.sin_addr.s_addr = inet_addr("192.168.0.100");
	*/
	//serverSocket을 ip,port와 연결
	bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));
	listen(serverSocket, 5);

	sockaddr_in clientAddress{};
	int clientAddressSize = sizeof(clientAddress);

	//accept에서 블로킹상태
	int clientSocket = accept(serverSocket, (sockaddr*)& clientAddress, &clientAddressSize); //클라이언트와 연결된 새 소켓 디스크립터(정수 번호표같은거) 반환

	//클라이언트가 메시지 보낼때까지 블로킹상태
	recv(clientSocket, buffer, sizeof(buffer), 0);
	cout << "Message from client: " << buffer
		<< endl;

	closesocket(clientSocket);
	closesocket(serverSocket);
	WSACleanup();
}
