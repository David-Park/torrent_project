#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	char buf[BUF_SIZE];
	int file_len, i, file_des, file_size, total;

	struct sockaddr_in serv_addr, clnt_addr;
	socklen_t clnt_addr_sz;

	if (argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	
	if (serv_sock == -1)
		error_handling("socket() error");

	//Initialization
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");

	if (listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	clnt_addr_sz = sizeof(clnt_addr);

	while(1)
	{
		char file_name[20];
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_sz);
		
		if (clnt_sock == -1)
			error_handling("accept() error");
		else
			printf("clinet connected\n");
		
		recv(clnt_sock, &file_name, sizeof(file_name), 0);//block until receiving file name
		printf("%s transmit file : %s\n", inet_ntoa(clnt_addr.sin_addr), file_name);

		//block until receiving file size
		recv(clnt_sock, &file_size, sizeof(file_size),0);
		printf("file size : %d\n", file_size);

		//file create
		file_des = open(file_name, O_WRONLY | O_CREAT | O_TRUNC,0644);

		while (total != file_size)
		{
			file_len = recv(clnt_sock, buf, BUF_SIZE, 0);
			total += file_len;
			write(file_des, buf, file_len);
		}

		printf("file transmission is success\n");

		close(file_des);
		close(clnt_sock);
	}
	close(serv_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}