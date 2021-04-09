#include <stdio.h>
#include <WinSock2.h>

#define MAX 1024
#define N 2
SOCKET* clients = NULL;
int count = 0;

DWORD WINAPI ClientThread(LPVOID param)
{
	int startIndex = (int)param;
	fd_set fdread;
	printf("Thread %d, start index: %d\n", GetCurrentThreadId(), startIndex);
	while (true)
	{
		FD_ZERO(&fdread);
		for (int i = startIndex; i < startIndex + N && i < count; ++i)
		{
			FD_SET(clients[i], &fdread);
		}
		timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		select(0, &fdread, NULL, NULL, &tv);
		for (int i = startIndex; i < startIndex + N && i < count; ++i)
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
			if (i < N)
			{
				FD_SET(clients[i], &fdread);
			}
			
		}

		select(0, &fdread, NULL, NULL, NULL);

		if (FD_ISSET(s, &fdread))
		{
			SOCKADDR_IN caddr;
			int clen = sizeof(caddr);
			SOCKET tmp = accept(s, (sockaddr*)&caddr, &clen);
			clients = (SOCKET*)realloc(clients, (count + 1) * sizeof(SOCKET));
			clients[count] = tmp;
			count += 1;
			if (count % N == 0)
			{
				CreateThread(NULL, 0, ClientThread, (LPVOID*) count, 0, NULL);
			}
		}

		for (int i = 0; i < count; ++i)
		{
			if (i < N)
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
}