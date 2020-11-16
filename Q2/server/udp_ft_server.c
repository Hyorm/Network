#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>

#define MAX 1024
#define BUF_SIZE 30

void recv_binary_file(int serv_sock, struct sockaddr_in clnt_adr, socklen_t clnt_adr_sz);
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int serv_sock;
	char message[BUF_SIZE];
	int str_len;
	socklen_t clnt_adr_sz;
	
	struct sockaddr_in serv_adr, clnt_adr;
	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_DGRAM, 0);
	if(serv_sock==-1)
		error_handling("UDP socket creation error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");

	/*while(1) 
	{
		clnt_adr_sz=sizeof(clnt_adr);
		str_len=recvfrom(serv_sock, message, BUF_SIZE, 0,(struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		printf("Message from client: %s", message);
		sendto(serv_sock, message, str_len, 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);

	}
	*/
	recv_binary_file(serv_sock, clnt_adr, clnt_adr_sz);
	
	close(serv_sock);
	return 0;
}

void recv_binary_file(int serv_sock, struct sockaddr_in clnt_adr, socklen_t clnt_adr_sz){

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
	while (len < 4 && (s = recvfrom(serv_sock, name_data, 4 - len, 0,(struct sockaddr*)&clnt_adr, &clnt_adr_sz)) > 0) {
		name_data += s;
		len += s;
	}
	len = 0;
	s = 0;

	while(len < name_len && (s = recvfrom(serv_sock, &file_name[idx++], name_len, 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz)) > 0){	
		len += s;
	}
	
	file_name[name_len] = '\0';
	
	printf("filename %s\n", file_name);

	fp = fopen(file_name, "wb");
	
	len = 0;
	s = 0;

	data = &file_size;

	while (len < 4 && (s = recvfrom(serv_sock, data, 4 - len, 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz)) > 0) {
		data += s;
		len += s;
	}

	len = 0;
	s = 0;

	while(len < file_size){
		if(file_size > MAX)
			s = recvfrom(serv_sock, buf_msg, MAX, 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		else
			s = recvfrom(serv_sock, buf_msg, file_size - len, 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

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
