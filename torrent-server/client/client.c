#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUF_SIZE 1024
void error_handling(char *message);
//test
int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	char message[BUF_SIZE];
	char filename[20];
	int send_file, file_size, file_len, total;
	struct stat file_info;

	if (argc != 3)
	{
		printf("Usage : %s <ip> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);
	
	if (sock == -1)
		error_handling("socket() error");

	//Initialization
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	
	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error!");
	else
		puts("Connected.............");

	while (1)
	{
		fputs("Input file name(Q to quit): ", stdout);
		fgets(message, BUF_SIZE, stdin);

		message[strlen(message) - 1] = 0;// delete '\n'
		strcpy(filename, message);
		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;

		//file open
		send_file = open(filename, O_RDONLY);
		if (!send_file)
		{//open failed
			error_handling("file opens failed");
		}

		//file information
		stat(filename, &file_info);
		file_size = file_info.st_size;

		send(sock, filename, sizeof(filename), 0);
		send(sock, &file_size, sizeof(file_size), 0);

		while (total != file_size)
		{
			file_len = read(send_file, message, BUF_SIZE);
			total += file_len;
			send(sock, message, file_len, 0);
		}

		printf("file tranfer success\n");

		close(send_file);
	}

	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}