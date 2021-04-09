#include <stdio.h>
#include <WinSock2.h>

#define MAX 1024
SOCKET clients[MAX];
int count;

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKADDR_IN saddr;
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	saddr.sin_addr.S_un.S_addr = INADDR_ANY;
	saddr.sin_port = htons(8888);
	saddr.sin_family = AF_INET;
	bind(s, (sockaddr*)&saddr, sizeof(saddr));
	listen(s, 10);
	fd_set fdread;
	while (true)
	{
		FD_ZERO(&fdread);
		FD_SET(s, &fdread);
		for (int i = 0; i < count; ++i)
		{
			FD_SET(clients[i], &fdread);
		}

		select(0, &fdread, NULL, NULL, NULL);

		if (FD_ISSET(s, &fdread))
		{
			SOCKADDR_IN caddr;
			int clen = sizeof(caddr);
			SOCKET tmp = accept(s, (sockaddr*)&caddr, &clen);
			clients[count] = tmp;
			count += 1;
		}

		for (int i = 0; i < count; ++i)
		{
			if (FD_ISSET(clients[i], &fdread))
			{
				char buffer[1024];
				memset(buffer, 0, sizeof(buffer));
				recv(clients[i], buffer, sizeof(buffer), 0);
				for (int j = 0; j < count; ++j)
				{
					if (j != i)
					{
						send(clients[j], buffer, strlen(buffer), 0);
					}
				}
			}
		}
	}
}