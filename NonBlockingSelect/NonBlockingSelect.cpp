#include <stdio.h>
#include <WinSock2.h>

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET c = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN saddr;
	saddr.sin_addr.S_un.S_addr = inet_addr("17.0.0.1");
	saddr.sin_port = htons(8888);
	saddr.sin_family = AF_INET;
	unsigned long argp = 1;
	int result = ioctlsocket(c, FIONBIO, &argp);
	int r = connect(c, (sockaddr*)&saddr, sizeof(saddr));

	argp = 0;
	result = ioctlsocket(c, FIONBIO, &argp);
	fd_set fdwrite, fderr;
	FD_ZERO(&fdwrite);
	FD_ZERO(&fderr);
	FD_SET(c, &fdwrite);
	FD_SET(c, &fderr);
	TIMEVAL timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	select(0, NULL, &fdwrite, &fderr, &timeout);
	if (FD_ISSET(c, &fdwrite))
	{
		printf("Ket noi thanh cong");
		closesocket(c);
	}
	else
		printf("Ket noi khong thanh cong");
}