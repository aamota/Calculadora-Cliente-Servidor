/*********************************************************
Projeto Calculadora Cliente/Servidor
Conexão por Socket - SERVIDOR
Programador: Alisson Abreu Mota
24/10/2019
*/
//********************************************************

#include <iostream>
#include <ws2tcpip.h>

// link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

using namespace std;
int main()
{
	// Inicializa o winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	int wsOK = WSAStartup(ver, &wsData);

	if (wsOK != 0) {

		cerr << "Não foi possivel inicializar o SOCKET " << endl;
		return 0;
	}
	//////////////////////////////////////////////////////////////////////
	// create a sockte
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {

		cerr << "Não foi possivel cria o socket listening" << endl;
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////
	// Prepara um enderecço ip e uma porta para o socket
	sockaddr_in  hint;

	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);

	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Para qualquer endereço
	//inet_pton(AF_INET, "127.0.0.1", &hint.sin_addr); // Endereço específico

	bind(listening, (sockaddr *)&hint, sizeof(hint));

	/////////////////////////////////////////////////////////////////////////
	// Comunica o sockt que está preparado
	listen(listening, SOMAXCONN);
	////////////////////////////////////////////////////////////////////////

	// Espera por uma conexão do cliente
	sockaddr_in cliente;
	int clienteSize = sizeof(cliente);

	SOCKET  clienteSocket = accept(listening, (sockaddr *)&cliente, &clienteSize);

	char host[NI_MAXHOST];
	char porta[NI_MAXHOST];
	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(porta, NI_MAXHOST);

	if (getnameinfo((sockaddr *)&cliente, sizeof(cliente), host, sizeof(NI_MAXHOST), porta, sizeof(NI_MAXHOST), 0) == 0) {
		cout << host << " foi conectado na porta " << porta << endl;
	}
	else {
		inet_ntop(AF_INET, &cliente.sin_addr, host, NI_MAXHOST);
		cout << host << " conectado na porta " << ntohs(cliente.sin_port) << endl << endl;
	}

	closesocket(listening);
	/////////////////////////////////////////////////////////////////////////////////

	// Dados para calculadora
	struct dados {
		int pos = 0;
		int option = 0;
		float valor1 = 0;
		float valor2 = 0;
		float resultado = 0;
	};
	struct dados cli;

/////////////////////////////////////////////////////////////////////////////////////////
	char buf_MSG_Cli[4096];
	char buf_MSG_Ser[4096];
	char nomeCliente[4096]; 
	bool sair = false;
	int bytesRecv;
	char result[4096];

	// Inicia um loop - recebe e envia msg ao cliente
	do
	{
		ZeroMemory(buf_MSG_Cli, 4096);
		ZeroMemory(buf_MSG_Ser, 4096);

		// Recebe a msg do cliente
		bytesRecv = recv(clienteSocket, buf_MSG_Cli, 4096, 0);
		if (bytesRecv == 0) {

			cout << " Não foi recebido nenhuma mensagem do Cliente!" << endl;
			break;
		}
		cout << "CLIENTE < " << buf_MSG_Cli << endl;

		switch (cli.pos) {
		case 0:
			strcpy_s(nomeCliente, buf_MSG_Cli);// Salva o nome do cliente
			sprintf_s(buf_MSG_Ser, "%s voce deseja fazer uma operacao ? (S/N)", nomeCliente);

			cout << buf_MSG_Ser << endl;
			cli.pos = 1;
			break;
		case 1:
			if (tolower(buf_MSG_Cli[0]) == 's') {
				strcpy_s(buf_MSG_Ser, "Escolha uma option:\n(A) Adicao \n(S) Subtracao \n(M) Multiplicacao \n(D) Divisao: ");
				cout << "SERVER < " << buf_MSG_Ser << endl;
			}
			else {
				sprintf_s(buf_MSG_Ser, "Obrigado e ateh a proxima %s! \nServidor desconectado.", nomeCliente);
				sair = true;
			}
			cli.pos = 2;
			break;
		case 2:
			switch (tolower(buf_MSG_Cli[0])) {
				case 'a': cli.option = 0;  break;
				case 's': cli.option = 1;  break;
				case 'm': cli.option = 2;  break;
				case 'd': cli.option = 3;  break;
			default: cli.option = 0;
			}

			strcpy_s(buf_MSG_Ser, "Primeiro numero: ");
			cli.pos = 3;

			break;
		case 3:
			cli.valor1 = (float)atol(buf_MSG_Cli);
			strcpy_s(buf_MSG_Ser, " Segundo numero: ");
			cli.pos = 4;

			break;
		case 4:
			cli.valor2 = (float)atol(buf_MSG_Cli);

			switch (cli.option) {
				case 0: cli.resultado = cli.valor1 + cli.valor2; break;
				case 1: cli.resultado = cli.valor1 - cli.valor2; break;
				case 2: cli.resultado = cli.valor1 * cli.valor2; break;
				case 3: cli.resultado = (cli.valor2 > 0) ? cli.valor1 / cli.valor2 : 0; break;// Evita divisão por zero.
			}

			_ltoa_s((long)cli.resultado, result, 10);

			sprintf_s(buf_MSG_Ser, "Resultado da operacao = %.2f \n\n %s voce deseja fazer mais uma operacao ? (S/N)", cli.resultado, nomeCliente);

			cli.pos = 1;
			cli.option = 0;
			break;
		}

		// Retorna a msg para o cliente
		long bytesSend = send(clienteSocket, buf_MSG_Ser, sizeof(buf_MSG_Ser), 0);
		if (bytesSend == 0) {
			cout << " Não foi possível retornar a mensagem ao Cliente! " << endl;
			break;
		}

	} while (sair == false);

	// close the socket
	closesocket(clienteSocket);

	// clearn the winsocket
	WSACleanup();

	return 1;
}

