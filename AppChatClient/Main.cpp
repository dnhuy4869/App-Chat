#include <iostream>
using namespace std;

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <sstream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define IPV4_ADDRESS "192.168.1.7"
#define DEFAULT_PORT 27015

void PrintMessageThread();

SOCKET ConnectSocket;

int main()
{
	INT WinsockError;

	WSADATA WSAData;
	WinsockError = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (WinsockError != 0)
	{
		cout << "WSAStartup failed. ERROR CODE: " << WSAGetLastError() << endl;
		system("pause >nul");
		return EXIT_FAILURE;
	}

	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET)
	{
		cout << "ConnectSocket is invalid. ERROR CODE: " << WSAGetLastError() << endl;
		system("pause >nul");
		return EXIT_FAILURE;
	}

	SOCKADDR_IN Hints;
	Hints.sin_family = AF_INET;
	Hints.sin_port = htons(DEFAULT_PORT);
	//Hints.sin_addr.S_un.S_addr = INADDR_ANY;
	inet_pton(AF_INET, IPV4_ADDRESS, &Hints.sin_addr.s_addr);

	WinsockError = connect(ConnectSocket, (SOCKADDR*)&Hints, sizeof(Hints));
	if (WinsockError == SOCKET_ERROR)
	{
		cout << "Can't connect to the server. ERROR CODE: " << WSAGetLastError() << endl;
		system("pause >nul");
		return EXIT_FAILURE;
	}
	else
	{
		cout << "Connected successfully, chat room is open now." << endl;
		cout << endl;
	}

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)PrintMessageThread, 0, 0, 0);

	CHAR Buffer[512];
	string UserInput;

	while (true)
	{
		ZeroMemory(Buffer, sizeof(Buffer));

		getline(cin, UserInput);
		cout << endl;

		if (UserInput.size() > 0)
		{
			send(ConnectSocket, UserInput.c_str(), UserInput.size(), 0);
		}
	}

	WSACleanup();
	system("pause >nul");
	return EXIT_SUCCESS;
}

void PrintMessageThread()
{
	bool IsFirstInitialize = false;
	CHAR Buffer[512];

	while (true)
	{
		if (IsFirstInitialize == false)
		{
			ZeroMemory(Buffer, sizeof(Buffer));
			INT BytesRecv = recv(ConnectSocket, Buffer, sizeof(Buffer), 0);
			if (BytesRecv > 0)
			{
				cout << "[SYSTEM]: " << string(Buffer, 0, BytesRecv) << endl;
			}

			IsFirstInitialize = true;
		}

		ZeroMemory(Buffer, sizeof(Buffer));

		INT BytesReceived = recv(ConnectSocket, Buffer, sizeof(Buffer), 0);
		if (BytesReceived > 0)
		{
			cout << Buffer << endl;
		}
	}
}