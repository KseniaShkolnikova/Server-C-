#define WIN32_LEAN_AND_MEAN //макрос

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
using namespace std;

int main()
{
	setlocale(LC_ALL, "RUS");

	WSADATA wasData;// служебная структура для хранения информации (например семейная структура)
	SOCKET ClientSocket = INVALID_SOCKET; // сокет для подключения


	ADDRINFO hints; // инфо с полями о сокете
	ADDRINFO* addrResult; // адрес хоста
	const char* senBuffer = "Hello from server"; // сообщение
	char recvBuffer[512]; // буфер для приема 1 сообщзения
	char recvBuffer2[512]; // буфер для приема 2 сообщзения


	SOCKET ListenSocket = INVALID_SOCKET; // прослушивающий сокет

	int result = WSAStartup(MAKEWORD(2, 2), &wasData); // начало использования библиотеки сокетов (определение версии, структуры)
	if (result != 0) {
		cout << " WSAStartup FAILED result" << endl;
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints)); // команда для зануления памяти (1. что 2. сколько)
	hints.ai_family = AF_INET; // присвоение 4Байтного эзернета
	hints.ai_socktype = SOCK_STREAM;// потоковый тип сокета
	hints.ai_protocol = IPPROTO_TCP;// протокол TCP
	hints.ai_flags = AI_PASSIVE; // Пассивная сторона, потому что просто ждет соединения

	// result хранит в себе адрес, порт, семейство структур и адрес сокета
	result = getaddrinfo(NULL, "666", &hints, &addrResult);
	if (result != 0) {
		cout << " getaddrinfo FAILED result" << endl;
		WSACleanup();
		return 1;
	}

	// создание сокета, который будет ждать сообщения ( в скобках адрес, тип сокета, протокол)
	ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		cout << " ListenSocket FAILED result" << endl;
		WSACleanup(); // очистка WSAStartup
		FreeAddrInfo(addrResult); // освобождение памяти адреса
		return 1;
	}

	// привязка сокета к определенному адресу и номеру порта ( в скобках указаны дескриптор сокета прослушивания, структура, которая содержит номер и адрес порта, длиня структуры)
	result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	if (result == SOCKET_ERROR) {
		cout << " binding connect" << endl;
		closesocket(ClientSocket);
		ListenSocket = INVALID_SOCKET;
		FreeAddrInfo(addrResult);
		WSACleanup();
		return 1;
	}

	// перевод сокета в режим прослушивания ( в скобках сам сокет и константа, которая указывает максимальное кол-во ожидающих соединений в очереди)
	result = listen(ListenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		cout << " ListenSocket connect" << endl;
		closesocket(ClientSocket);
		ListenSocket = INVALID_SOCKET;
		FreeAddrInfo(addrResult);
		WSACleanup();
		return 1;
	}
	 // сокет принимает входящее соединение от клиента на сокет прослушивания (в скобках указаны дескриптор сокета прослушивания и NULL для адреса и длины адреса)
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		cout << " accept FAILED result" << endl;
		closesocket(ClientSocket);
		ListenSocket = INVALID_SOCKET;
		FreeAddrInfo(addrResult);
		WSACleanup();
		return 1;
	}
	closesocket(ListenSocket);// закрытие сокета прослушивания

	do {
		ZeroMemory(recvBuffer, 512); // зхануление буфера приема
		result = recv(ClientSocket, recvBuffer, 512, 0);// result приниммет данные сокета клиента ( в скобках указаны дескриптор сокета клиента, сообщение, размер буфера и флаг)
		if (result > 0) {
			cout << "Received " << result << "bytes" << endl; // кол-во байтов сообщения
			cout << "Received data " << recvBuffer << endl;// сообщение

			result = send(ClientSocket, senBuffer, (int)strlen(senBuffer), 0);
			if (result == SOCKET_ERROR) {
				cout << " send FAILED result" << endl;
				WSACleanup();
				return 1;
			}

		}
		else if (result == 0)
			cout << "Connection closed" << endl;

		else
			cout << "recv failed with error" << endl;
		// тот же блок кода, но для 2 сообщения
		ZeroMemory(recvBuffer2, 512);
		result = recv(ClientSocket, recvBuffer2, 512, 0);
		if (result > 0) {
			cout << "Received " << result << "bytes" << endl;
			cout << "Received data " << recvBuffer2 << endl;

		}
		else if (result == 0)
			cout << "Connection closed" << endl;

		else
			cout << "recv failed with error" << endl;
		closesocket(ClientSocket); // закрытие сокета клиента
		FreeAddrInfo(addrResult); // очистка инфы об адресе хоста
		WSACleanup();
		return 1;


	} while (result > 0);
	result = shutdown(ClientSocket, SD_SEND); // отключение от сокета клиента (в скобках указаны дескриптор сокета клиента и флаг, который запрещает отправку данных)
	if (result == SOCKET_ERROR) {
		cout << " shutdown FAILED result" << endl;
		FreeAddrInfo(addrResult);
		WSACleanup();
		return 1;
	}
	closesocket(ClientSocket);// закрытие сокета клиента
	freeaddrinfo(addrResult); // освобождение памяти для структуры addrResult
	WSACleanup();//доиницилизируем библиотеку
	return 0;

}
