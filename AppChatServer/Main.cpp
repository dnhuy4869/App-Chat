#include <iostream>
using namespace std;

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <sstream>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define IPV4_ADDRESS "192.168.1.7"
#define DEFAULT_PORT 27015

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

	SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == INVALID_SOCKET)
	{
		cout << "ListenSocket is invalid. ERROR CODE: " << WSAGetLastError() << endl;
		system("pause >nul");
		return EXIT_FAILURE;
	}

	SOCKADDR_IN Hints;
	Hints.sin_family = AF_INET;
	Hints.sin_port = htons(DEFAULT_PORT);
	//Hints.sin_addr.S_un.S_addr = INADDR_ANY;
	inet_pton(AF_INET, IPV4_ADDRESS, &Hints.sin_addr.s_addr);

	WinsockError = bind(ListenSocket, (SOCKADDR*)&Hints, sizeof(Hints));
	if (WinsockError == SOCKET_ERROR)
	{
		cout << "Bind failed. ERROR CODE: " << WSAGetLastError() << endl;
		system("pause >nul");
		return EXIT_FAILURE;
	}

	WinsockError = listen(ListenSocket, SOMAXCONN);
	if (WinsockError == SOCKET_ERROR)
	{
		cout << "Listen failed. ERROR CODE: " << WSAGetLastError() << endl;
		system("pause >nul");
		return EXIT_FAILURE;
	}

	cout << "Server started successfully, waiting for client..." << endl;
	cout << endl;

	FD_SET Master;
	FD_ZERO(&Master);

	FD_SET(ListenSocket, &Master);

	while (true)
	{
		FD_SET CopyMaster = Master;

		INT SocketCount = select(0, &CopyMaster, nullptr, nullptr, nullptr);

		for (int i = 0; i < SocketCount; i++)
		{
			SOCKET Socket = CopyMaster.fd_array[i];
			if (Socket == ListenSocket)
			{
				SOCKET Client = accept(ListenSocket, NULL, NULL);
				
				FD_SET(Client, &Master);

				CHAR Host[NI_MAXHOST];		// Client's remote name
				CHAR Service[NI_MAXSERV];	// Service (i.e port) the client is connecting on

				ZeroMemory(Host, NI_MAXHOST);
				ZeroMemory(Service, NI_MAXSERV);

				WinsockError = GetNameInfoA((SOCKADDR*)&Hints, sizeof(Hints), Host, NI_MAXHOST, Service, NI_MAXSERV, 0);
				if (WinsockError != 0)
				{
					inet_ntop(AF_INET, &Hints.sin_addr, Host, NI_MAXHOST);
					cout << "Unknown#" << to_string(Client) + " (" + Host + ")" + " has joined room chat." << endl;
				}
				else
				{
					cout << "Unknown#" << to_string(Client) + " (" + Host + ")" + " has joined room chat." << endl;
				}

				string WelcomeMessage = "Welcome to room chat Unknown#" + to_string(Client) + "\r\n";
				send(Client, WelcomeMessage.c_str(), WelcomeMessage.size(), 0);
			}
			else
			{
				CHAR Buffer[512];
				ZeroMemory(Buffer, sizeof(Buffer));

				INT BytesRecv = recv(Socket, Buffer, sizeof(Buffer), 0);
				if (BytesRecv <= 0)
				{
					cout << "Unknown#" << to_string(Socket) + " has quited room chat." << endl;
					closesocket(Socket);
					FD_CLR(Socket, &Master);
				}
				else
				{
					for (int i = 0; i < Master.fd_count; i++)
					{
						SOCKET OutSocket = Master.fd_array[i];
						if (OutSocket != ListenSocket && OutSocket != Socket)
						{
							ostringstream ss;
							ss << "Unknown#" << Socket << ": " << Buffer << "\r\n";
							string Message = ss.str();

							send(OutSocket, Message.c_str(), Message.size(), 0);
						}
					}

				}
			}
		}
	}

	WSACleanup();
	system("pause >nul");
	return EXIT_SUCCESS;
}