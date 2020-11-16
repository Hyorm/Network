#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX 1024

void send_binary_file(int sock, char *filename);
void error_handling(char *message);

int main(int argc, char* argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	char message[30];
	int str_len=0;
	int idx=0, read_len=0;
	
	if(argc!=4){
		printf("Usage : %s <IP> <port> <filename>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
		
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1) 
		error_handling("connect() error!");

	/*while(read_len=read(sock, &message[idx++], 1))
	{
		if(read_len==-1)
		{
			error_handling("read() error!");
			break;
		}
		str_len+=read_len;
	}

	printf("Message from server: %s \n", message);
	printf("Function read call count: %d \n", str_len);
	*/
	
	//TODO: Send binary file
	send_binary_file(sock, argv[3]);

	//TODO: time checks
	close(sock);
	return 0;
}

void send_binary_file(int sock, char *filename){
	struct timeval send_strt, send_end;
	struct stat file_info;

	long sum_time = 0;

	int file_bin, file_size, left_size;
	int len = 0, s = 0, read_size = 0, nfile = 0, sum_buf = 0;

	char buf[MAX], file_content[MAX];
	
	file_bin = open(filename, O_RDONLY);
	stat(filename, &file_info);

	if(!file_bin){
		perror("Error : ");
		exit(1);
	}

	//send filename
	
	len = 0;
	int name_len = strlen(filename);
	char* namebuf = (char*)&name_len;
	
	while(len < 4 && (s = send(sock, namebuf, 4 - len, 0)) > 0){
		namebuf += s;
		len += s;
	}
	len = 0;

	while(len < name_len && (s = send(sock, filename, strlen(filename), 0))>0){
		len += s;
	}

	file_size = (int) file_info.st_size;

	//send size of file
	char* pbuf = (char*)&file_size;
	
	len = 0;
	
	while(len < 4 && (s = send(sock, pbuf, 4 - len, 0)) > 0){
		pbuf += s;
		len += s;
	}

	char* pfile_content;

	//send file
	len = 0;
	gettimeofday(&send_strt, NULL);
	while(len < file_size){
		int send_len;
		read_size = read(file_bin, file_content, MAX);
		len += read_size;

		//send
		pfile_content = file_content ;
		send_len = 0 ;

		while(send_len < read_size &&  (nfile = send(sock, pfile_content, read_size - send_len, 0))> 0 ){
			pfile_content += nfile;
			send_len += nfile;
		}	
	}
	gettimeofday(&send_end, NULL);
	
	close(file_bin);
	
	sum_time = (send_end.tv_sec - send_strt.tv_sec)*1000000 + (send_end.tv_usec - send_strt.tv_usec);
	fprintf(stderr, "-----------------------------------------\n   elapsed time: %lf sec\n   throughput: %lf bps\n-----------------------------------------\n", (double)sum_time /1000000.0, (double)(file_size * 8)/((double)sum_time/1000000.0));

};

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
