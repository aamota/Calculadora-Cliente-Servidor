/*
Projeto Calculadora Cliente/Servidor
Conexão por Socket - CLIENTE
programador: Aisson Abreu Mota
24/10/2019
*/

#include <iostream>
#include <string>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")
using namespace std;

int main()
{
	string ipAdress = "127.0.0.1";  // ip do servidor
	int port = 54000;  // porta do listening do servidor
/////////////////////////////////////////////////////////////////////////

	// Inicializa o sockte do windows
	WSAData wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsResult = WSAStartup(ver, &wsData);
	if (wsResult != 0) {
		cerr << " Não foi possível inicializar o WinSock , Erro #" << wsResult << endl;
		return 0;
	}
	////////////////////////////////////////////////////////////////////////////////

	// Cria um socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		cerr << " Não foi possível criar o WinSock , Erro #" << WSAGetLastError() << endl;
		WSACleanup();
		return 0;
	}
	////////////////////////////////////////////////////////////////////////////////////
	//Preenche a estrutura do hint para a conexão do socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAdress.c_str(), &hint.sin_addr);

	/////////////////////////////////////////////////////////////////////////////////////	
	// Conecta com o servidor
	int connResult = connect(sock, (sockaddr *)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR) {
		cerr << " Não foi possível conectar ao servidor, Erro #" << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return 0;
	}
	////////////////////////////////////////////////////////////////////////////////////

	char serverMSG[4096]; // buffer para receber a resposta do servidor
	string userMSG; // buffer para enviar msg para o servidor

	cout << "Digite teu nome: ";
	getline(cin, userMSG);
	cout << endl;

	// Loop - enviar e receber mensagen
	do
	{
		if (userMSG.size() > 0) {// Certifica que foi digitado alguma coisa

			// Envia o texto para o servidor
			int sendResult = send(sock, userMSG.c_str(), userMSG.size() + 1, 0);

			// Espera pela resposta
			if (sendResult != SOCKET_ERROR) {

				ZeroMemory(serverMSG, 4094);
				int bytesRecebidos = recv(sock, serverMSG, sizeof(serverMSG), 0);

				if (bytesRecebidos > 0) {
					// Escreve a resposta no console
					cout << " SERVIDOR > " << string(serverMSG, 0, bytesRecebidos);
				}
			}
			getline(cin, userMSG);
			cout << endl;
		}
	} while (userMSG.c_str() > 0);

	//Close everything
	closesocket(sock);
	WSACleanup();

	return 0;
}

