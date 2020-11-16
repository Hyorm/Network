#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#define MAX 1024

void recv_binary_file(int clnt_sock;);
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sock;
	int clnt_sock;
	int opt_val;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;

	char message[]="Hello World!";
	
	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	setsockopt(serv_sock, IPPROTO_TCP, TCP_NODELAY, &opt_val, sizeof(opt_val));

	if(serv_sock == -1)
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)
		error_handling("bind() error"); 
	
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	
	clnt_addr_size=sizeof(clnt_addr);  
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr,&clnt_addr_size);
	if(clnt_sock==-1)
		error_handling("accept() error");  
	
	//write(clnt_sock, message, sizeof(message));
	//TODO: recv binary file
	recv_binary_file(clnt_sock);
	close(clnt_sock);
	close(serv_sock);
	return 0;
}

void recv_binary_file(int clnt_sock){
	struct stat file_info;
	
	FILE *fp;

	int idx = 0;
	int file_size, name_len, s;
	int len = 0, str_len = 0, nrcv = 0;	
	int* data = 0x0;
       	int* name_data = 0x0;

	char buf_msg[MAX], file_name[MAX];


	//TODO: recv filename
	len = 0;
	s = 0;

	name_data = &name_len;
	while (len < 4 && (s = recv(clnt_sock, name_data, 4 - len, 0 )) > 0) {
		name_data += s;
		len += s;
	}
	len = 0;
	s = 0;

	while(len < name_len && (s = recv(clnt_sock, &file_name[idx++], name_len, 0)) > 0){	
		len += s;
	}
	
	file_name[name_len] = '\0';
	
	printf("filename %s\n", file_name);

	fp = fopen(file_name, "wb");
	
	len = 0;
	s = 0;

	data = &file_size;

	while (len < 4 && (s = recv(clnt_sock, data, 4 - len, 0 )) > 0) {
		data += s;
		len += s;
	}

	len = 0;
	s = 0;

	while(len < file_size){
		if(file_size > MAX)
			s = recv(clnt_sock, buf_msg, MAX, 0);
		else
			s = recv(clnt_sock, buf_msg, file_size - len, 0);

		if(s <= 0)
			break;

		fwrite(buf_msg, s, 1, fp);

		len += s;	
	}
	fclose(fp);
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
